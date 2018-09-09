from multiprocessing import Pool

import chess
import numpy as np
from lightgbm import LGBMClassifier
from scipy.sparse import csr_matrix
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import roc_auc_score
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from tqdm import tqdm

from lgbm_dump_model import parseAllTrees

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
    features = [0] * (8 * 8 * 12)
    for rank in range(8):
        for file in range(8):
            piece = str(board.piece_at(rank * 8 + file))
            if piece in pieces_list:
                piece_ind = pieces_list.index(piece)
                vector_ind = piece_ind * 8 * 8 + rank * 8 + file
                features[vector_ind] = 1
    return (features, result)


data = open("datasets/quiet-labeled.v7.epd").readlines()

with Pool(7) as pool:
    boards = [f for f in tqdm(pool.imap_unordered(epd_to_board, data))]

with Pool(7) as pool:
    features = [f for f in tqdm(pool.imap_unordered(prepare_features, boards))]

X, y = list(zip(*features))

X = np.array(X)
y = (np.array(y) == '1-0').astype(np.int)


X = csr_matrix(X)

X_tr, X_te, y_tr, y_te = train_test_split(X, y)

model_lr = LogisticRegression()
model_lr.fit(X_tr, y_tr)
pred_lr = model_lr.predict_proba(X_te)[:, 1]
print("auc lr", roc_auc_score(y_te, pred_lr))

model_nn = MLPClassifier(hidden_layer_sizes=(5,), verbose=True)
model_nn.fit(X_tr, y_tr)
pred_nn = model_lr.predict_proba(X_te)[:, 1]
print("auc lr", roc_auc_score(y_te, pred_nn))

model_lgbm = LGBMClassifier(n_estimators=100)
model_lgbm.fit(X_tr.astype(np.float64), y_tr)
pred_lgbm = model_lgbm.predict_proba(X_te.astype(np.float64))[:, 1]
print("auc lgbm", roc_auc_score(y_te, pred_lgbm))

print("auc", roc_auc_score(y_te, pred_lr + pred_lgbm))

with open("lgbm_if_else.c", "wt") as out:
    out.write(parseAllTrees(model_lgbm.booster_.dump_model()['tree_info']).replace("1.0000000180025095e-35","1"))


"""
print("{},")
for chunk in chunks(weights, 64):
    print("{%s}," % ",".join([str(i) for i in chunk]))
"""
