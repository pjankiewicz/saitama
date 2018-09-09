import io
import re
from collections import namedtuple
from multiprocessing.pool import ThreadPool

import chess
import chess.pgn
import chess.uci
from tqdm import tqdm

FAKE_HEADER = """
[Event "F/S Return Match"]
[Site "Belgrade, Serbia JUG"]
[Date "1992.11.04"]
[Round "29"]
[White "Fischer, Robert J."]
[Black "Spassky, Boris V."]
[Result "1/2-1/2"]
"""


def find_eval(comment):
    comment = comment.replace("M", "1000")
    f = re.findall("([+-][0-9\.]+)", comment)
    if f:
        try:
            return float(f[0])
        except:
            return None
    else:
        return None


def sign(v):
    if v < 0:
        return -1
    else:
        return 1


def extract_positions(game_str):
    game = chess.pgn.read_game(io.StringIO(game_str))
    headers = game.headers
    result = headers['Result']
    winner = {"1-0": chess.WHITE, "0-1": chess.BLACK, "1/2-1/2": None}[result]
    positions = []
    if winner in [chess.WHITE, chess.BLACK]:
        turn = chess.WHITE
        move_num = 1
        while not game.is_end():
            node = game.variations[0]
            evaluation = find_eval(node.comment.replace("\n", ""))
            board = game.board()
            if evaluation:
                positions.append((headers['Round'], board.epd(), turn, evaluation))
            game = node
            turn ^= 1
            move_num += 1

        if len(positions) >= 2:
            if winner in [chess.BLACK, chess.WHITE]:
                if not ((positions[-2][3] > 0 and positions[-1][3] > 0) or (
                        positions[-2][3] < 0 and positions[-1][3] < 0)):
                    # print("Reversed", result, headers['Round'])
                    return [(rnd, positions[ply + 1][1], turn ^ 1, ev * -1 if turn == 0 else ev) for
                            ply, (rnd, epd, turn, ev) in
                            enumerate(positions) if int(ply / 2) % 5 == 0 and ply < len(positions) - 1]
                else:
                    return [(rnd, positions[ply + 1][1], turn ^ 1, ev) for ply, (rnd, epd, turn, ev) in
                            enumerate(positions) if
                            int(ply / 2) % 5 == 0 and ply < len(positions) - 1]
            else:
                return []
        else:
            return []
    return positions


def find_eval_lichess(comment):
    comment = comment.replace("#-", "-300")
    comment = comment.replace("#", "300")
    try:
        return float(re.findall("%eval ([\-0-9.]+)", comment)[0])
    except:
        return None
print(find_eval_lichess("[%eval -4.67] [%clk 0:09:17]"))
print(find_eval_lichess("[%eval #5] [%clk 0:03:18]"))
print(find_eval_lichess("[%eval #-5] [%clk 0:03:18]"))

class Position(namedtuple('Position', 'id,epd,turn,eval')):
    def serialize(self):
        return "{}\t{}\t{}\t{}".format(self.id, self.epd, self.turn, self.eval)


def extract_positions_lichess(game_str):
    rnd, game_str = game_str
    node = chess.pgn.read_game(io.StringIO(FAKE_HEADER + game_str))
    turn = chess.WHITE
    move_num = 1
    positions = []
    while not node.is_end():
        next_node = node.variations[0]
        evaluation = find_eval_lichess(node.comment.replace("\n", ""))
        board = node.board()
        next_move = str(board.san(next_node.move))
        board.push_san(next_move)
        if evaluation:
            positions.append(Position(rnd, board.epd(), turn, evaluation))
        node = next_node
        turn ^= 1
        move_num += 1
    return [pos for i, pos in enumerate(positions) if i % 5 == 0]


def iter_games(pgn_path, n_games=None):
    pgn_dataset = open(pgn_path, encoding="ISO-8859-1")
    n = 0
    while True:
        n += 1
        game = chess.pgn.read_game(pgn_dataset)
        if game is None or (n_games and n > n_games):
            break
        yield str(game)
    pgn_dataset.close()


def iter_games_v2(path):
    print("Reading...")
    data = open(path, encoding="ISO-8859-1").read()
    print("Done...")
    for pgn in data.split('[Event "CCRL 40/40"]'):
        if len(pgn) > 0:
            yield pgn


def iter_games_lichess(path):
    with open(path) as inp:
        for i, line in enumerate(inp):
            yield (i, line)


def parse():
    # path = "datasets/CCRL-4040-commented.[885965].pgn"
    # path = "datasets/CCRL-404FRC-commented.[224100].pgn"
    # path = "datasets/sample.pgn"
    path = "datasets/lichess_db_standard_rated_2018-08-evaluations.txt"
    output = open("datasets/positions.json", "wt")
    pool = ThreadPool(6)
    n = 0
    for positions in tqdm(pool.imap_unordered(extract_positions_lichess, iter_games_lichess(path), chunksize=100)):
    # for positions in tqdm(map(extract_positions_lichess, iter_games_lichess(path))):
        if positions:
            n += 1
            for position in positions:
                output.write("{}\n".format(position.serialize()))
    pool.close()
    output.close()


parse()
