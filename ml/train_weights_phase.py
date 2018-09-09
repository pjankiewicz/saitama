from multiprocessing import Pool

import chess
import numpy as np
from scipy.sparse import csr_matrix
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import roc_auc_score
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from tqdm import tqdm

pieces_list = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']


def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in range(0, len(l), n):
        yield l[i:i + n]


def epd_to_board(data):
    epd, result = data.rsplit(" ", 1)
    result = result.replace("\n", "").replace(";", "").strip('""')
    board = chess.Board()
    board.set_epd(epd)
    return (board, result)


def prepare_features(board_result):
    board, result = board_result
    material = [0] * len(pieces_list)
    features = [0] * 8 * 8 * 12
    for rank in range(8):
        for file in range(8):
            piece = str(board.piece_at(rank * 8 + file))
            if piece in pieces_list:
                piece_ind = pieces_list.index(piece)
                material[piece_ind] += 1
                vector_ind = piece_ind * 8 * 8 + rank * 8 + file
                features[vector_ind] = 1
    features = features + material
    return (features, result)

data = open("datasets/quiet-labeled.v7.epd").readlines()

with Pool(7) as pool:
    boards = [f for f in tqdm(pool.imap_unordered(epd_to_board, data))]

with Pool(7) as pool:
    features = [f for f in tqdm(pool.imap_unordered(prepare_features, boards))]

X, y = list(zip(*features))

X = np.array(X)
y = (np.array(y) == '1-0').astype(np.int)

X_tr, X_te, y_tr, y_te = train_test_split(X, y)
X_tr = csr_matrix(X_tr)
X_te = csr_matrix(X_te)

for model in [
    LogisticRegression(),
    MLPClassifier(hidden_layer_sizes=(5), activation="logistic", max_iter=100, verbose=True),
    MLPClassifier(hidden_layer_sizes=(5), activation="tanh", max_iter=100, verbose=True),
]:
    model.fit(X_tr, y_tr)

    pred = model.predict_proba(X_te)[:, 1]
    print("auc", roc_auc_score(y_te, pred))

assert False

weights = [int(c * 100) for c in model.coef_[0]]

print("{},")
for chunk in chunks(weights, 64):
    print("{%s}," % ",".join([str(i) for i in chunk]))

# separate models for n pieces
n_pieces_tr = X_tr.sum(axis=1)
n_pieces_te = X_te.sum(axis=1)


def classify_phase(n_pieces):
    if n_pieces >= 25:
        return 1
    elif n_pieces >= 15:
        return 2
    else:
        return 3


phase_tr = np.array(list(map(classify_phase, n_pieces_tr)))
phase_te = np.array(list(map(classify_phase, n_pieces_te)))

coefs = []
pred_te_all = np.zeros_like(y_te, dtype=np.float)
for phase in [1, 2, 3]:
    model = LogisticRegression()  # MLPClassifier(hidden_layer_sizes=(100), max_iter=10, verbose=True)
    model.fit(X_tr[phase_tr == phase], y_tr[phase_tr == phase])
    coefs.append(list(model.coef_[0]))
    pred = model.predict_proba(X_te[phase_te == phase])[:, 1]
    print("auc", roc_auc_score(y_te[phase_te == phase], pred))
    pred_te_all[np.where(phase_te == phase)[0]] = pred

coefs_by_phase = np.array(coefs).T.reshape(12, 8, 8, 3)
weights = json.dumps(np.round(100 * coefs_by_phase.transpose(3, 0, 1, 2).reshape(3, 12, -1)).astype(np.int).tolist())
weights = weights.replace("[", "{").replace("]", "}")

with open("weights.c", "wt") as out:
    out.write("const int weights[3][12][64] = %s" % weights)
