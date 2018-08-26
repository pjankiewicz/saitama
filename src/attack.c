
#include "defs.h"

/*
 *
 *

Board

0	1	2	3	4	5	6	7	8	9
10	11	12	13	14	15	16	17	18	19
20	21	22	23	24	25	26	27	28	29
30	31	32	33	34	35	36	37	38	39
40	41	42	43	44	45	46	47	48	49
50	51	52	53	54	55	56	57	58	59
60	61	62	63	64	65	66	67	68	69
70	71	72	73	74	75	76	77	78	79
80	81	82	83	84	85	86	87	88	89
90	91	92	93	94	95	96	97	98	99
100	101	102	103	104	105	106	107	108	109
110	111	112	113	114	115	116	117	118	119


21	22	23	24	25	26	27	28
31	32	33	34	35	36	37	38
41	42	43	44	45	46	47	48
51	52	53	54	55	56	57	58
61	62	63	64	65	66	67	68
71	72	73	74	75	76	77	78
81	82	83	84	85	86	87	88
91	92	93	94	95	96	97	98


 */

int KnightMoves[8] = {-19, -8, 12, 21, 19, 8, -12, -21};
int BishopMoves[4] = {-9, 11, 9, -11};
int QueenMoves[8] = {-9, 11, 9, -11, -10, 1, 10, -1};
int KingMoves[8] = {-9, 11, 9, -11, -10, 1, 10, -1};
int RookMoves[4] = {-10, 1, 10, -1};

/*
 * Is a square attacked by side
 */
int SqAttacked(const S_BOARD *pos, int sq, int side) {

    int index, cur_sq, piece;

    assert(SqOnBoard(sq));
    assert(SideValid(side));
    assert(CheckBoard(pos));

    // attacked by pawn
    if (side == WHITE && ((pos->pieces[sq - 9] == wP) || (pos->pieces[sq - 11] == wP))) {
        return TRUE;
    } else if (side == BLACK && ((pos->pieces[sq + 11] == bP) || (pos->pieces[sq + 9] == bP))) {
        return TRUE;
    }

    // attacked by knights
    for (index = 0; index < 8; ++index) {
        piece = pos->pieces[sq + KnightMoves[index]];
        if (piece != OFFBOARD && IsKn(piece) && PieceCol[piece] == side) {
            return TRUE;
        }
    }

    // rooks & queens
    for (index = 0; index < 4; ++index) {
        cur_sq = sq;
        while (TRUE) {
            cur_sq += RookMoves[index];
            piece = pos->pieces[cur_sq];
            if (cur_sq == OFFBOARD) {
                break;
            } else if (IsRQ(piece) && (PieceCol[piece] == side)) {
                return TRUE;
            } else if (piece != EMPTY) {
                break;
            }
        }
    }

    // bishop & queen
    for (index = 0; index < 4; ++index) {
        cur_sq = sq;
        while (TRUE) {
            cur_sq += BishopMoves[index];
            piece = pos->pieces[cur_sq];
            if (cur_sq == OFFBOARD) {
                break;
            } else if (IsBQ(piece) && PieceCol[piece] == side) {
                return TRUE;
            } else if (piece != EMPTY) {
                break;
            }
        }
    }

    // attacked by king
    for (index = 0; index < 8; ++index) {
        cur_sq = sq + KingMoves[index];
        if (cur_sq != NO_SQ && pos->kingSq[side] == cur_sq) {
            return TRUE;
        }
    }

    return FALSE;
}

void VisualizeAttackedSquares(S_BOARD *pos, int side) {
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        printf("\n");
        for (int file = FILE_A; file <= FILE_H; file++) {
            if (SqAttacked(pos, FR2SQ(file, rank), side)) {
                printf(" X ");
            } else {
                printf(" . ");
            }
        }
    }
}