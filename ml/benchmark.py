import chess
import chess.uci

# engine = chess.uci.popen_engine("stockfish")
# print(engine.go(movetime=2000))

######
#             0.1s   1s   3s   10s
# saitama       30   37   34    40
# stockfish     19   51        105
# ethereal      11   11         51


# engine = chess.uci.popen_engine("/home/pawel/projects/saitama/saitama")
engine = chess.uci.popen_engine("/home/pawel/projects/chessengines/Ethereal/src/Ethereal")
# engine = chess.uci.popen_engine("stockfish")
found = 0
tries = 0
for line in open("arasan20.epd.txt", encoding="iso-8859-1"):
    board = chess.Board()
    board, info = board.from_epd(line)
    if 'bm' not in info:
        continue

    engine.position(board)
    bm = engine.go(movetime=100, ponder=None)
    if bm.bestmove in info['bm']:
        found += 1
    tries += 1
    print('True %s Found %s [%s] [%d/%d]' % (info['bm'], bm, bm.bestmove in info['bm'], found, tries))