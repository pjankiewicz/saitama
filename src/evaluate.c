
#include "defs.h"
#include "mlp.h"

// clang-format off

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


static int GamePhase(S_BOARD *pos) {
    int allPieces = 0;
    for (int i = 0; i < 13; i++)
        allPieces += pos->pceNum[i];
    if (allPieces >= 25) {
        return 0;
    } else if (allPieces >= 15) {
        return 1;
    } else {
        return 2;
    }
}

int IsThreefold(S_BOARD *board) {
    if (board->hisPly < 8) {
        return 0;
    }

    int repeats = 0;

    for (int i = 0; i <= board->ply; i++) {
        assert(board->ply - i >= 0);
        if (board->history[board->ply - i].posKey == board->posKey) {
            repeats++;
            if (repeats >= 3) {
                return 1;
            }
        }
    }
    return 0;
}

int EvalPosition(S_BOARD *pos) {
//    NNEvalPositionInc(pos);
    if (pos->side == WHITE) {
        return NNEvalPositionWhite(pos);
    } else {
        return NNEvalPositionBlack(pos);
    }
}
