#include "defs.h"

#define HASH_PCE(pce, sq) (pos->posKey ^= PieceKeys[(pce)][(sq)])
#define HASH_CA (pos->posKey ^= CastleKeys[pos->castlePerm])
#define HASH_SIDE (pos->posKey ^= SideKey)
#define HASH_EP (pos->posKey ^= PieceKeys[EMPTY][pos->enPas])

// these constants define castling permissions

// no changes in castling perm
const int CA_ALL = WKCA | WQCA | BKCA | BQCA;

// losing rights to castle when moving one of the pieces
const int CA_W = BKCA | BQCA;
const int CA_WKC = WQCA | BKCA | BQCA;
const int CA_WQC = WKCA | BKCA | BQCA;
const int CA_B = WKCA | WQCA;
const int CA_BKC = WKCA | WQCA | BQCA;
const int CA_BQC = WKCA | WQCA | BKCA;

const int CastlePerm[120] = {
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_WQC, CA_ALL, CA_ALL, CA_ALL, CA_W, CA_ALL, CA_ALL, CA_WKC, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_BQC, CA_ALL, CA_ALL, CA_ALL, CA_B, CA_ALL, CA_ALL, CA_BKC, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL,
        CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL, CA_ALL
};

static void ClearPiece(const int sq, S_BOARD *pos) {
    assert(SqOnBoard(sq));

    int pce = pos->pieces[sq];
    int col = PieceCol[pce];

    assert(PieceValid(pce));

    int index = 0;
    int t_pceNum = -1;

    HASH_PCE(pce, sq);

    if (PieceBig[pce]) {
        pos->bigPce[col]--;

        if (PieceMaj[pce]) {
            pos->majPce[col]--;
        }
        if (PieceMin[pce]) {
            pos->minPce[col]--;
        }
    } else {
        CLRBIT(pos->pawns[col], SQ64(sq));
        CLRBIT(pos->pawns[BOTH], SQ64(sq));
    }
    pos->material[col] -= PieceVal[pce];

    // update pList
    for (int i = 0; i < pos->pceNum[pce]; i++) {
        if (pos->pList[pce][i] == sq) {
            t_pceNum = i;
            break;
        }
    }
    pos->pceNum[pce]--;
    pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce]];
    pos->pieces[sq] = EMPTY;
}

static void AddPiece(const int sq, S_BOARD *pos, int pce) {
    assert(SqOnBoard(sq));

    int col = PieceCol[pce];

    HASH_PCE(pce, sq);

    if (PieceBig[pce]) {
        pos->bigPce[col]++;

        if (PieceMaj[pce]) {
            pos->majPce[col]++;
        }
        if (PieceMin[pce]) {
            pos->minPce[col]++;
        }
    } else {
        SETBIT(pos->pawns[col], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
    }
    pos->material[col] += PieceVal[pce];

    // update pList
    pos->pList[pce][pos->pceNum[pce]++] = sq;
    pos->pieces[sq] = pce;
}

static void MovePiece(const int sq, const int to_sq, S_BOARD *pos) {
    assert(SqOnBoard(sq));
    assert(SqOnBoard(to_sq));
    assert(pos->pieces[sq] != EMPTY);

    int pce = pos->pieces[sq];
    int col = PieceCol[pce];
    int t_pceNum = 0;

    assert(PieceValid(pce));

    HASH_PCE(pce, sq);
    HASH_PCE(pce, to_sq);

    if (PiecePawn[pce]) {
        CLRBIT(pos->pawns[col], SQ64(sq));
        SETBIT(pos->pawns[col], SQ64(to_sq));
        CLRBIT(pos->pawns[BOTH], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(to_sq));
    }

    // update pList
    int found = 0;
    for (int i = 0; i < pos->pceNum[pce]; i++) {
        if (pos->pList[pce][i] == sq) {
            pos->pList[pce][i] = to_sq;
            found = 1;
            break;
        }
    }
    assert(found);

    pos->pieces[sq] = EMPTY;
    pos->pieces[to_sq] = pce;
}

/* Returns 0 is the move is invalid */
int MakeMove(S_BOARD *pos, int move) {
    assert(CheckBoard(pos));

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;
    int pce = pos->pieces[from];

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));
    assert(SideValid(side));
    assert(PieceValid(pce));

    pos->history[pos->hisPly].posKey = pos->posKey;
    pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPas = pos->enPas;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    if (move & MFLAGEP) {
        if (side == WHITE) {
            assert(pos->pieces[to+DOWN] == bP);
            ClearPiece(to+DOWN, pos);
        } else {
            assert(pos->pieces[to+UP] == wP);
            ClearPiece(to+UP, pos);
        }
    } else if (move & MFLAGCA) {
//        printf("%d %s\n", PrSq(to));

        assert(pos->pieces[from] == wK || pos->pieces[from] ==bK);
        switch (to) {
            case C1:
                MovePiece(A1, D1, pos);
                break;
            case C8:
                MovePiece(A8, D8, pos);
                break;
            case G1:
                MovePiece(H1, F1, pos);
                break;
            case G8:
                MovePiece(H8, F8, pos);
                break;
            default:
                assert(FALSE);
                break;
        }
    }

    if (pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPas = NO_SQ;

    HASH_CA;

    int captured = CAPTURED(move);
    pos->fiftyMove++;

    if (captured != EMPTY) {
        assert(PieceValid(captured));
        ClearPiece(to, pos);
        pos->fiftyMove = 0;
    }

    pos->hisPly++;
    pos->ply++;

    if (PiecePawn[pos->pieces[from]]) {
        pos->fiftyMove = 0;
        if (move & MFLAGPS) {
            if (side == WHITE) {
                pos->enPas = from+UP;
                assert(RanksBrd[from+UP]==RANK_3);
            } else {
                pos->enPas = from+DOWN;
                assert(RanksBrd[from+DOWN]==RANK_6);
            }
            HASH_EP;
        }
    }

    MovePiece(from, to, pos);

    int prPce = PROMOTED(move);
    if (prPce != EMPTY) {
        ClearPiece(to, pos);
        AddPiece(to, pos, prPce);
    }

    if (PieceKing[pos->pieces[to]]) {
        pos->kingSq[pos->side] = to;
    }

    pos->side ^= 1;
    HASH_SIDE;

    assert(CheckBoard(pos));

    if (SqAttacked(pos, pos->kingSq[side], pos->side)) {
        TakeMove(pos);
        return FALSE;
    }
    return TRUE;
}

void TakeMove(S_BOARD *pos) {
    assert(CheckBoard(pos));

    assert(pos->hisPly > 0);
    S_UNDO undo = pos->history[pos->hisPly-1];
    int move = undo.move;

    if (pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->castlePerm = undo.castlePerm;
    pos->enPas = undo.enPas;
    pos->fiftyMove = undo.fiftyMove;
    pos->hisPly--;
    pos->ply--;

    if (pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    if (move & MFLAGEP) {
        if (side == WHITE) {
            AddPiece(to+DOWN, pos, bP);
        } else {
            AddPiece(to+UP, pos, wP);
        }
    } else if (move & MFLAGCA) {
        switch (to) {
            case C1:
                MovePiece(D1, A1, pos);
                break;
            case C8:
                MovePiece(D8, A8, pos);
                break;
            case G1:
                MovePiece(F1, H1, pos);
                break;
            case G8:
                MovePiece(F8, H8, pos);
                break;
            default:
                assert(FALSE);
                break;
        }
    }

    MovePiece(to, from, pos);

    int captured = CAPTURED(move);
    if (captured != EMPTY) {
        AddPiece(to, pos, captured);                       // add back the captured piece
    }

    if (PROMOTED(move) != EMPTY) {
        ClearPiece(from, pos);                               // remove the promoted piece
        AddPiece(from, pos, pos->side == WHITE ? wP : bP); // add back the pawn
    }

    if (PieceKing[pos->pieces[from]]) {
        pos->kingSq[pos->side] = from;
    }

//    pos->posKey = undo.posKey;
    assert(CheckBoard(pos));
}