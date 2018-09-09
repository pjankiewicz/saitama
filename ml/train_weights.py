from multiprocessing import Pool

import chess
import numpy as np
from scipy.sparse import csr_matrix
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import roc_auc_score
from sklearn.model_selection import train_test_split
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
    features = [0] * 8 * 8 * 12
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

X_tr, X_te, y_tr, y_te = train_test_split(X, y)
X_tr = csr_matrix(X_tr)
X_te = csr_matrix(X_te)

for model in [
    LogisticRegression(C=0.01),
    LogisticRegression(C=0.03),
    LogisticRegression(C=0.1),
    LogisticRegression(C=0.3),
    LogisticRegression(C=1.0),
    LogisticRegression(C=3.0),
]:
    model.fit(X_tr, y_tr)
    pred = model.predict_proba(X_te)[:, 1]
    print("auc", roc_auc_score(y_te, pred))

"""
print("{},")
for chunk in chunks(weights, 64):
    print("{%s}," % ",".join([str(i) for i in chunk]))
"""
