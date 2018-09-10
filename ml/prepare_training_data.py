import sys

import chess
import joblib
from tqdm import tqdm

from lsmat import LSMATBuilder

pieces_list = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
pieces_value = [0.1, 0.3, 0.3, 0.4, 0.9, 10, -0.1, -0.3, -0.3, -0.4, -0.9, -10]


def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in range(0, len(l), n):
        yield l[i:i + n]


def epd_to_board(data):
    try:
        rnd, epd, turn, result = data.split("\t")
    except:
        print(data.split("\t"))
        assert False
    result = float(result)
    board = chess.Board()
    board.set_epd(epd)
    board_mirror = board.mirror()
    return [(str(board), rnd, board, result),
            (str(board_mirror), rnd, board_mirror, -result)]


def prepare_features(board_result):
    _, rnd, board, result = board_result
    sparse_board = [0] * (8 * 8 * 12)
    for rank in range(8):
        for file in range(8):
            piece = str(board.piece_at(rank * 8 + file))
            if piece in pieces_list:
                piece_ind = pieces_list.index(piece)
                sparse_vector_ind = piece_ind * 8 * 8 + rank * 8 + file
                sparse_board[sparse_vector_ind] = 1  # pieces_value[piece_ind]

    flags = []
    flags.append(int(board.turn))
    for cas in 'KQkq':
        is_valid = cas in board.castling_xfen()
        flags.append(int(is_valid))

    return (rnd, sparse_board + flags, result)


def prepare_features_only(board):
    return prepare_features((None, None, board, None))[1]


data = open(sys.argv[1])

seen = set()
allboards = []
n = 0
for boards in tqdm(map(epd_to_board, data)):
    for f in boards:
        if f[0] not in seen:
            allboards.append(f)
            n += 1
            seen.add(f[0])

data.close()

lsmat = LSMATBuilder()
yraw = []
rnds = []
for row_ind, (rnd, features, result) in tqdm(
        enumerate(map(prepare_features, allboards))):
    yraw.append(result)
    rnds.append(rnd)
    # index first row to have all columns in the right order
    for col_ind, val in enumerate(features):
        if val != 0:
            lsmat.add_raw(row_ind, col_ind, val)

X = lsmat.get_coo_matrix().tocsr()
joblib.dump((X, yraw, rnds), sys.argv[2], compress=2)
