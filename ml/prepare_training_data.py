import chess
import joblib
import numpy as np
from tqdm import tqdm

from features import FeatureGenerator
from lsmat import LSMATBuilder


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


def make_dataset(data, feature_generator):
    seen = set()
    allboards = []
    n = 0
    for boards in tqdm(map(epd_to_board, data)):
        for f in boards:
            if f[0] not in seen:
                allboards.append(f)
                n += 1
                seen.add(f[0])

    _, rnds, boards, yraw = list(zip(*allboards))
    data.close()

    lsmat = LSMATBuilder()
    for row_ind, features in tqdm(
            enumerate(map(feature_generator.make, boards))):
        for col_ind, val in enumerate(features):
            if val != 0:
                lsmat.add_raw(row_ind, col_ind, val)
        if row_ind > 1000000:
            break

    X = lsmat.get_coo_matrix().tocsr()
    return (X, np.array(yraw), np.array(rnds))


def make_validation_datasets():
    feature_generator = FeatureGenerator()
    with open("datasets/positions_chunks.json00") as data:
        X, yraw, rnds = make_dataset(data, feature_generator)
    joblib.dump((X, yraw, rnds), "datasets/positions_1_squares_flags.joblib", compress=2)

    feature_generator = FeatureGenerator(attacked_squares=True)
    with open("datasets/positions_chunks.json00") as data:
        X, yraw, rnds = make_dataset(data, feature_generator)
    joblib.dump((X, yraw, rnds), "datasets/positions_1_squares_flags_attackers.joblib", compress=2)

    feature_generator = FeatureGenerator(attacked_squares=True, pins=True)
    with open("datasets/positions_chunks.json00") as data:
        X, yraw, rnds = make_dataset(data, feature_generator)
    joblib.dump((X, yraw, rnds), "datasets/positions_1_squares_flags_attackers_pins.joblib", compress=2)


if __name__ == '__main__':
    make_validation_datasets()
    # feature_generator = FeatureGenerator()
    # data = open(sys.argv[1])
    # X, yraw, rnds = make_dataset(data)
    # joblib.dump((X, yraw, rnds), sys.argv[2], compress=2)
