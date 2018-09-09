
def prepare_features_v2(data):
    epd, result = data.rsplit(" ", 1)
    result = result.replace("\n", "").replace(";", "").strip('""')
    board = chess.Board()
    board.set_epd(epd)
    pieces_list = list('.RNBKQPrnbkqp')
    #     pieces_list = ["r","n","b","k","q","p"]

    features = [0] * 8 * 8 * 12

    # pieces
    for rank in range(8):
        for file in range(8):
            piece = board.piece_at(rank * 8 + file)
            piece = str(piece) if piece else '.'
            piece_ind = pieces_list.index(piece)
            if piece == ".":
                value = 0
            elif piece.isupper() and board.turn:
                value = 1
            elif piece.isupper() and not board.turn:
                value = -1
            elif piece.islower() and board.turn:
                value = -1
            elif piece.islower() and not board.turn:
                value = 1
            else:
                assert False, "Shouldn't happen"
            features[rank * 8 * 8 + file * 12 + piece_ind] = value

    for cas in 'KQkq':
        is_valid = cas in board.castling_xfen()
        features.append(int(is_valid))

    features.append(int(board.turn))

    return (features, result)


def prepare_features_v3(data):
    epd, result = data.rsplit(" ", 1)
    result = result.replace("\n", "").replace(";", "").strip('""')
    board = chess.Board()
    board.set_epd(epd)
    pieces_list = list('.RNBKQPrnbkqp')
    material_count = [0] * len(pieces_list)

    features = []  # [0]*8*8*12

    # pieces
    for rank in range(8):
        for file in range(8):
            sq = rank * 8 + file
            piece = board.piece_at(rank * 8 + file)
            piece_str = str(piece) if piece else '.'
            features.append(piece_str)
            material_count[pieces_list.index(piece_str)] += 1

    features.extend([board.is_attacked_by(chess.WHITE, sq) for sq in range(64)])
    features.extend([board.is_attacked_by(chess.BLACK, sq) for sq in range(64)])
    features.extend([board.is_pinned(chess.WHITE, sq) for sq in range(64)])
    features.extend([board.is_pinned(chess.BLACK, sq) for sq in range(64)])
    features.extend([len(board.attackers(chess.WHITE, sq)) for sq in range(64)])
    features.extend([len(board.attackers(chess.BLACK, sq)) for sq in range(64)])
    #     features.extend([str(board.pin(chess.WHITE, sq)) for sq in range(64)])
    #     features.extend([str(board.pin(chess.BLACK, sq)) for sq in range(64)])

    features.extend([int(sq) for sq in list("{0:64b}".format(board.occupied).replace(" ", "0"))])

    #     #any piece
    #     for rank in range(8):
    #         for file in range(8):
    #             if piece:
    #                 features.append(1)
    #             else:
    #                 features.append(0)

    #     #pawns
    #     for rank in range(8):
    #         for file in range(8):
    #             piece = int(piece in ['p','P'])
    #             features.append(piece)

    for cas in 'KQkq':
        is_valid = cas in board.castling_xfen()
        features.append(int(is_valid))

    features.append(str(board.ep_square))
    features.append(int(board.turn))
    features.extend(material_count)
    features.append("".join([str(m) for m in material_count[1:]]))
    features.append(int(board.connected_kings))
    return (features, result)