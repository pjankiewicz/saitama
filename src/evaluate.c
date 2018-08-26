#include "defs.h"

// clang-format off

const int WhitePieceSquareEvals[7][64] = {
        {}, // to match the empty piece
        {
                0, 0,   0,   0,  0,  0,   0,   0,
                10, 10, 0, -10, -10, 0, 10, 10,
                5, 0, 0,  5,  5,  0,  0, 5,
                0, 0,  10, 20, 20, 10, 0,  0,
                5, 5,  5,  10, 10, 5,  5,  5,
                10, 10, 10, 20, 20, 10, 10, 10,
                20, 20, 20, 30, 30, 20, 20, 20,
                0, 0, 0, 0,  0,  0, 0, 0
        },
        {
                0, -10, 0,   0,  0,  0,   -10, 0,
                0,  0,  0, 5,   5,   0, 0,  0,
                0, 0, 10, 10, 10, 10, 0, 0,
                0, 0,  10, 20, 20, 10, 5,  0,
                5, 10, 15, 20, 20, 15, 10, 5,
                5,  10, 10, 20, 20, 10, 10, 5,
                0,  0,  5,  10, 10, 5,  0,  0,
                0, 0, 0, 0,  0,  0, 0, 0
        },
        {
                0, 0,   -10, 0,  0,  -10, 0,   0,
                0,  0,  0, 10,  10,  0, 0,  0,
                0, 0, 10, 15, 15, 10, 0, 0,
                0, 10, 15, 20, 20, 15, 10, 0,
                0, 10, 15, 20, 20, 15, 10, 0,
                0,  0,  10, 15, 15, 10, 0,  0,
                0,  0,  0,  10, 10, 0,  0,  0,
                0, 0, 0, 0,  0,  0, 0, 0
        },
        {
                0, 0,   5,   10, 10, 5,   0,   0,
                0,  0,  5, 10,  10,  5, 0,  0,
                0, 0, 5,  10, 10, 5,  0, 0,
                0, 0,  5,  10, 10, 5,  0,  0,
                0, 0,  5,  10, 10, 5,  0,  0,
                0,  0,  5,  10, 10, 5,  0,  0,
                25, 25, 25, 25, 25, 25, 25, 25,
                0, 0, 5, 10, 10, 5, 0, 0
        },
        {
                0, 0,   0,   0,  0,  0,   0,   0,
                0,  0,  0, 10,  10,  0, 0,  0,
                0, 0, 10, 15, 15, 10, 0, 0,
                0, 10, 15, 20, 20, 15, 10, 0,
                0, 10, 15, 20, 20, 15, 10, 0,
                0,  0,  10, 15, 15, 10, 0,  0,
                0,  0,  0,  10, 10, 0,  0,  0,
                0, 0, 0, 0,  0,  0, 0, 0
        },
        {
                0, 0,   0,   0,  0,  0,   0,   0,
                0,  0,  0, 0,   0,   0, 0,  0,
                0, 0, 0,  0,  0,  0,  0, 0,
                0, 0,  0,  0,  0,  0,  0,  0,
                0, 0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,
                0, 0, 0, 0,  0,  0, 0, 0
        }
};

const int Mirror64[64] = {
        56, 57, 58, 59, 60, 61, 62, 63,
        48, 49, 50, 51, 52, 53, 54, 55,
        40, 41, 42, 43, 44, 45, 46, 47,
        32, 33, 34, 35, 36, 37, 38, 39,
        24, 25, 26, 27, 28, 29, 30, 31,
        16, 17, 18, 19, 20, 21, 22, 23,
        8, 9, 10, 11, 12, 13, 14, 15,
        0, 1, 2, 3, 4, 5, 6, 7
};

// clang-format on

#define MIRROR64(sq) (Mirror64[(sq)])

int SquarePieceEvals[13][64];

int EvalPosition(S_BOARD *pos) {
    int sq = OFFBOARD;
    int black_offset = 6; // by how many indices move pieces
    int pce_white;
    int pce_black;

    int score = pos->material[WHITE] - pos->material[BLACK];
    for (pce_white = wP; pce_white <= wK; pce_white++) {
        for (int pce_num=0; pce_num < pos->pceNum[pce_white]; pce_num++) {
            sq = pos->pList[pce_white][pce_num];
            assert(SqOnBoard(sq));
            score += WhitePieceSquareEvals[pce_white][SQ64(sq)];
        }

        pce_black = pce_white + black_offset;
        for (int pce_num=0; pce_num < pos->pceNum[pce_black]; pce_num++) {
            sq = pos->pList[pce_black][pce_num];
            assert(SqOnBoard(sq));
            score -= WhitePieceSquareEvals[pce_white][MIRROR64(SQ64(sq))];
        }
    }

    if (pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }
}