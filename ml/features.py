import chess

pieces_list = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
pieces_value = [0.1, 0.3, 0.3, 0.4, 0.9, 10, -0.1, -0.3, -0.3, -0.4, -0.9, -10]

class FeatureGenerator():

    def __init__(self, board=True, flags=True, attacked_squares=False, pins=False):
        self.board = board
        self.flags = flags
        self.attacked_squares = attacked_squares
        self.pins = pins

    def make(self, board):
        features = []
        if self.board: features.extend(self.get_sparse_board(board))
        if self.flags: features.extend(self.get_flags(board))
        if self.attacked_squares: features.extend(self.get_attacked_squares(board))
        if self.pins: features.extend(self.get_pins(board))
        return features

    def get_sparse_board(self, board):
        sparse_board = [0] * (8 * 8 * 12)
        for rank in range(8):
            for file in range(8):
                piece = str(board.piece_at(rank * 8 + file))
                if piece in pieces_list:
                    piece_ind = pieces_list.index(piece)
                    sparse_vector_ind = piece_ind * 8 * 8 + rank * 8 + file
                    sparse_board[sparse_vector_ind] = 1
        return sparse_board

    def get_flags(self, board):
        flags = []
        flags.append(int(board.turn))
        for cas in 'KQkq':
            is_valid = cas in board.castling_xfen()
            flags.append(int(is_valid))
        return flags

    def get_attacked_squares(self, board):
        return [int(board.is_attacked_by(chess.WHITE, sq)) for sq in range(64)] + \
               [int(board.is_attacked_by(chess.BLACK, sq)) for sq in range(64)]

    def get_pins(self, board):
        return [board.is_pinned(chess.WHITE, sq) for sq in range(64)] + \
               [board.is_pinned(chess.BLACK, sq) for sq in range(64)]

    def get_attack_difference(self):
        pass
        # features.extend([len(board.attackers(chess.WHITE, sq)) for sq in range(64)])
        # features.extend([len(board.attackers(chess.BLACK, sq)) for sq in range(64)])
