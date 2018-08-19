#include "defs.h"

int CheckBoard(S_BOARD *pos) {
    int t_pceNum[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int t_bigPce[] = {0, 0};
    int t_majPce[] = {0, 0};
    int t_minPce[] = {0, 0};
    int t_material[] = {0, 0};

    int sq64, sq120, piece, color, pawn_count;

    U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

    t_pawns[WHITE] = pos->pawns[WHITE];
    t_pawns[BLACK] = pos->pawns[BLACK];
    t_pawns[BOTH] = pos->pawns[BOTH];

    // check piece list
    for (piece = wP; piece <= bK; ++piece) {
        for (int pce_num = 0; pce_num < pos->pceNum[piece]; ++pce_num) {
            sq120 = pos->pList[piece][pce_num];
            ASSERT(pos->pieces[sq120] == piece);
        }
    }

    for (int index = 0; index < 64; index++) {
        sq120 = SQ120(index);
        piece = pos->pieces[sq120];
        color = PieceCol[piece];
        t_pceNum[piece]++;
        if (PieceBig[piece])
            t_bigPce[color]++;
        if (PieceMaj[piece])
            t_majPce[color]++;
        if (PieceMin[piece])
            t_minPce[color]++;
        t_material[color] += PieceVal[piece];
    }

    for (int i = wP; i <= bK; i++) {
        ASSERT(pos->pceNum[i] == t_pceNum[i])
    }

    ASSERT(pos->material[WHITE] == t_material[WHITE]);
    ASSERT(pos->material[BLACK] == t_material[BLACK]);
    ASSERT(pos->bigPce[WHITE] == t_bigPce[WHITE]);
    ASSERT(pos->bigPce[BLACK] == t_bigPce[BLACK]);
    ASSERT(pos->majPce[WHITE] == t_majPce[WHITE]);
    ASSERT(pos->majPce[BLACK] == t_majPce[BLACK]);
    ASSERT(pos->minPce[WHITE] == t_minPce[WHITE]);
    ASSERT(pos->minPce[BLACK] == t_minPce[BLACK]);

    ASSERT(pos->posKey == GeneratePosKey(pos));

    if ((pos->side == WHITE) && (pos->enPas != NO_SQ))
        ASSERT(RanksBrd[pos->enPas] == 6);
    if ((pos->side == BLACK) && (pos->enPas != NO_SQ))
        ASSERT(RanksBrd[pos->enPas] == 3);

    pawn_count = CNT(t_pawns[WHITE]);
    ASSERT(pawn_count == pos->pceNum[wP])
    pawn_count = CNT(t_pawns[BLACK]);
    ASSERT(pawn_count == pos->pceNum[bP])
    pawn_count = CNT(t_pawns[BOTH]);
    ASSERT(pawn_count == pos->pceNum[wP] + pos->pceNum[bP])

    while (t_pawns[WHITE]) {
        sq64 = POP(&t_pawns[WHITE]);
        ASSERT(pos->pieces[sq64] == wP)
    }
    while (t_pawns[BLACK]) {
        sq64 = POP(&t_pawns[BLACK]);
        ASSERT(pos->pieces[sq64] == wP)
    }
    while (t_pawns[BOTH]) {
        sq64 = POP(&t_pawns[BOTH]);
        ASSERT((pos->pieces[sq64] == wP) || (pos->pieces[sq64] == bP))
    }

    ASSERT(pos->side == WHITE || pos->side == BLACK)

    return TRUE;
}

void UpdateListsMaterial(S_BOARD *pos) {
    int piece, sq, index, color;
    for (index = 0; index < BRD_SQ_NUM; ++index) {
        sq = index;
        piece = pos->pieces[index];
        if (piece != OFFBOARD && piece != EMPTY) {
            pos->pceNum[piece]++;
            color = PieceCol[piece];
            if (PieceBig[piece])
                pos->bigPce[color]++;
            if (PieceMaj[piece])
                pos->majPce[color]++;
            if (PieceMin[piece])
                pos->minPce[color]++;
            pos->material[color] += PieceVal[piece];

            pos->pList[piece][pos->pceNum[piece]] = sq;

            if (piece == wK)
                pos->kingSq[WHITE] = sq;
            if (piece == bK)
                pos->kingSq[BLACK] = sq;

            if (piece == wP) {
                SETBIT(pos->pawns[WHITE], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            } else if (piece == bP) {
                SETBIT(pos->pawns[BLACK], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            }
        }
    }
}

int ParseFen(char *fen, S_BOARD *pos) {
    /*
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
     */

    ASSERT(fen != NULL);
    ASSERT(pos != NULL);

    int rank = RANK_8;
    int file = FILE_NONE;
    int piece = 0;
    int current_sq = 0;
    int count = 0;

    printf("Reseting board\n");
    ResetBoard(pos);

    while ((rank >= RANK_1) && *fen) {
        count = 1;
        switch (*fen) {
            case 'p':
                piece = bP;
                break;
            case 'r':
                piece = bR;
                break;
            case 'n':
                piece = bN;
                break;
            case 'b':
                piece = bB;
                break;
            case 'k':
                piece = bK;
                break;
            case 'q':
                piece = bQ;
                break;
            case 'P':
                piece = wP;
                break;
            case 'R':
                piece = wR;
                break;
            case 'N':
                piece = wN;
                break;
            case 'B':
                piece = wB;
                break;
            case 'K':
                piece = wK;
                break;
            case 'Q':
                piece = wQ;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;
            case '/':
            case ' ':
                fen++;
                rank--;
                continue;
            default:
                printf("FEN error\n");
                return 0;
        }

        for (int i = 0; i < count; i++) {
            pos->pieces[SQ120(current_sq)] = piece;
            current_sq++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b')

    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    // castle permissions
    while (*fen != ' ') {
        switch (*fen) {
            case 'K':
                pos->castlePerm |= WKCA;
                break;
            case 'Q':
                pos->castlePerm |= WQCA;
                break;
            case 'k':
                pos->castlePerm |= BKCA;
                break;
            case 'q':
                pos->castlePerm |= BQCA;
                break;
            case '-':
                break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm > 0 && pos->castlePerm <= 15);

    // en passant square
    if (*fen != '-') {
        int file = fen[0] - 'a';
        int rank = fen[1] - '1';
        ASSERT(file >= FILE_A && file <= FILE_H);
        ASSERT(rank >= RANK_1 && rank <= RANK_8);
        pos->enPas = FR2SQ(file, rank);
    } else {
        fen += 2;
    }

    pos->posKey = GeneratePosKey(pos);

    UpdateListsMaterial(pos);

    return 1;
}

void ResetBoard(S_BOARD *pos) {
    int index = 0;

    for (index = 0; index < BRD_SQ_NUM; ++index) {
        pos->pieces[index] = OFFBOARD;
    }

    for (index = 0; index < 64; ++index) {
        pos->pieces[SQ120(index)] = EMPTY;
    }

    for (index = 0; index < 2; index++) {
        pos->bigPce[index] = 0;
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->material[index] = 0;
    }

    for (index = 0; index < 3; index++) {
        pos->pawns[index] = 0ULL;
    }

    for (index = 0; index < 13; index++) {
        pos->pceNum[index] = 0;
    }

    pos->kingSq[WHITE] = pos->kingSq[BLACK] = NO_SQ;

    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;

    pos->ply = 0;
    pos->hisPly = 0;

    pos->castlePerm = 0;
    pos->posKey = 1ULL;

    pos->material[WHITE] = pos->material[BLACK] = 0;
}

void PrintBoard(const S_BOARD *pos) {
    int rank = 8;
    printf("Board:\n");
    for (int sq = 0; sq < 64; sq++) {
        if ((sq % 8 == 0)) {
            printf("\n");
            printf("%d", rank--);
        }
        printf("%3c", PceChar[pos->pieces[SQ120(sq)]]);
    }
    printf("\n\n ");
    for (int file = FILE_A; file <= FILE_H; file++) {
        printf("%3c", 'a' + file);
    }
    printf("\n");
    printf("Side to move: %c\n", SideChar[pos->side]);

    printf("enPas:%d\n", pos->enPas);
    printf("castle:%c%c%c%c\n", pos->castlePerm & WKCA ? 'K' : '-', pos->castlePerm & WQCA ? 'Q' : '-',
           pos->castlePerm & BKCA ? 'k' : '-', pos->castlePerm & BQCA ? 'q' : '-');
    printf("PosKey:%0llx\n", pos->posKey);
}