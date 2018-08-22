
#include "defs.h"

#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)

const int UP = 10;
const int DOWN = -10;
const int LEFT = -1;
const int RIGHT = 1;
const int WHITE_PAWN_CAPTURES[] = {UP + LEFT, UP + RIGHT};
const int BLACK_PAWN_CAPTURES[] = {DOWN + LEFT, DOWN + RIGHT};

/*
 * enum {
    EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK
};
 */
const int slidingPieces[2][3] = {{wB, wR, wQ}, {bB, bR, bQ}};
const int slidingPiecesDir[3][8] = {
    {UP + LEFT, UP + RIGHT, DOWN + LEFT, DOWN + RIGHT, 0, 0, 0, 0},
    {UP, DOWN, LEFT, RIGHT, 0, 0, 0, 0},
    {UP + LEFT, UP + RIGHT, DOWN + LEFT, DOWN + RIGHT, UP, DOWN, LEFT, RIGHT},
};

const int nonSlidingPieces[2][2] = {{wN, wK}, {bN, bK}};
const int nonSlidingPiecesDir[2][8] = {
    {UP + UP + RIGHT, RIGHT + RIGHT + UP, RIGHT + RIGHT + DOWN, DOWN + DOWN + RIGHT, DOWN + DOWN + LEFT,
     LEFT + LEFT + DOWN, LEFT + LEFT + UP, UP + UP + LEFT},
    {UP + LEFT, UP + RIGHT, DOWN + LEFT, DOWN + RIGHT, UP, DOWN, LEFT, RIGHT},
};

void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    if (RanksBrd[from] == RANK_7) {
        AddCaptureMove(pos, MOVE(from, to, cap, wQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wB, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wN, 0), list);
    } else {
        AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}

void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    if (RanksBrd[from] == RANK_7) {
        AddQuietMove(pos, MOVE(from, to, 0, wQ, 0), list);
        AddQuietMove(pos, MOVE(from, to, 0, wR, 0), list);
        AddQuietMove(pos, MOVE(from, to, 0, wB, 0), list);
        AddQuietMove(pos, MOVE(from, to, 0, wN, 0), list);
    } else {
        AddQuietMove(pos, MOVE(from, to, 0, EMPTY, 0), list);
    }
}

void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    if (RanksBrd[from] == RANK_2) {
        AddCaptureMove(pos, MOVE(from, to, cap, bQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bB, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bN, 0), list);
    } else {
        AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}

void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    if (RanksBrd[from] == RANK_2) {
        AddQuietMove(pos, MOVE(from, to, 0, bQ, 0), list);
        AddQuietMove(pos, MOVE(from, to, 0, bR, 0), list);
        AddQuietMove(pos, MOVE(from, to, 0, bB, 0), list);
        AddQuietMove(pos, MOVE(from, to, 0, bN, 0), list);
    } else {
        AddQuietMove(pos, MOVE(from, to, 0, EMPTY, 0), list);
    }
}

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list) {
    assert(CheckBoard(pos));

    list->count = 0;

    int pce = EMPTY;
    int side = pos->side;
    int j = 0;
    int sq = 0;
    int t_sq = 0;
    int pceNum = 0;
    int dir = 0;

    if (side == WHITE) {
        for (pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
            sq = pos->pList[wP][pceNum];
            assert(SqOnBoard(sq));
            if (pos->pieces[sq + UP] == EMPTY) {
                AddWhitePawnMove(pos, sq, sq + UP, list);
                if (RanksBrd[sq] == RANK_2 && pos->pieces[sq + UP + UP] == EMPTY) {
                    AddQuietMove(pos, MOVE(sq, (sq + UP + UP), EMPTY, EMPTY, MFLAGPS), list);
                }
            }

            for (int cap_ind = 0; cap_ind < 2; ++cap_ind) {
                int cap_sq = sq + WHITE_PAWN_CAPTURES[cap_ind];
                if (SqOnBoard(cap_sq) && PieceCol[pos->pieces[cap_sq]] == BLACK) {
                    AddWhitePawnCapMove(pos, sq, cap_sq, pos->pieces[cap_sq], list);
                }
                if (cap_sq == pos->enPas) {
                    AddWhitePawnCapMove(pos, sq, cap_sq, pos->pieces[cap_sq], list);
                }
            }
        }
    } else {
        for (pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
            sq = pos->pList[bP][pceNum];
            assert(SqOnBoard(sq));
            if (pos->pieces[sq + DOWN] == EMPTY) {
                AddBlackPawnMove(pos, sq, sq + DOWN, list);
                if (RanksBrd[sq] == RANK_7 && pos->pieces[sq + DOWN + DOWN] == EMPTY) {
                    AddQuietMove(pos, MOVE(sq, (sq + DOWN + DOWN), EMPTY, EMPTY, MFLAGPS), list);
                }
            }

            for (int cap_ind = 0; cap_ind < 2; ++cap_ind) {
                int cap_sq = sq + BLACK_PAWN_CAPTURES[cap_ind];
                if (SqOnBoard(cap_sq) && PieceCol[pos->pieces[cap_sq]] == WHITE) {
                    AddBlackPawnCapMove(pos, sq, cap_sq, pos->pieces[cap_sq], list);
                }
                if (cap_sq == pos->enPas) {
                    AddBlackPawnCapMove(pos, sq, cap_sq, pos->pieces[cap_sq], list);
                }
            }
        }
    }

    /* Loop for slide pieces : bishop rook queen*/
    for (int i = 0; i < 3; i++) {
        pce = slidingPieces[side][i];
        for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            for (j = 0; j < 8; j++) {
                dir = slidingPiecesDir[i][j];
                t_sq = sq;
                while (TRUE) {
                    t_sq += dir;
                    if (pos->pieces[t_sq] == OFFBOARD || PieceCol[pos->pieces[t_sq]] == side) {
                        break;
                    } else if (pos->pieces[t_sq] == EMPTY) {
                        AddQuietMove(pos, MOVE(sq, t_sq, 0, 0, 0), list);
                    } else if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], 0, 0), list);
                        break;
                    } else {
                        assert(0); // this shouldn't happen
                    }
                }
            }
        }
    }

    /* Loop for non slide pieces: Knight King */
    for (int i = 0; i < 2; i++) {
        pce = nonSlidingPieces[side][i];
        for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            printf("\n");
            for (j = 0; j < 8; j++) {
                dir = nonSlidingPiecesDir[i][j];
                printf("%d\n", dir);
                if (dir == 0) {
                    continue;
                }
                t_sq = sq + dir;
                if (pos->pieces[t_sq] == OFFBOARD || PieceCol[pos->pieces[t_sq]] == side) {
                    continue;
                } else if (pos->pieces[t_sq] == EMPTY) {
                    AddQuietMove(pos, MOVE(sq, t_sq, 0, 0, 0), list);
                } else if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                    AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], 0, 0), list);
                } else {
                    assert(0); // this shouldn't happen
                }
            }
        }
    }
}