#include "defs.h"
#include "mlp.h"

int CheckBoard(const S_BOARD *pos) {
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
    //    PrintBoard(pos);
    for (piece = wP; piece <= bK; ++piece) {
        for (int pce_num = 0; pce_num < pos->pceNum[piece]; ++pce_num) {
            sq120 = pos->pList[piece][pce_num];
            //            printf("piece=%d %s pce_num=%d/%d\n", piece, PrSq(sq120), (pce_num+1), pos->pceNum[piece]);
            assert(pos->pieces[sq120] == piece);
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
        assert(pos->pceNum[i] == t_pceNum[i]);
    }

    assert(pos->material[WHITE] == t_material[WHITE]);
    assert(pos->material[BLACK] == t_material[BLACK]);
    assert(pos->bigPce[WHITE] == t_bigPce[WHITE]);
    assert(pos->bigPce[BLACK] == t_bigPce[BLACK]);
    assert(pos->majPce[WHITE] == t_majPce[WHITE]);
    assert(pos->majPce[BLACK] == t_majPce[BLACK]);
    assert(pos->minPce[WHITE] == t_minPce[WHITE]);
    assert(pos->minPce[BLACK] == t_minPce[BLACK]);

    assert(pos->posKey == GeneratePosKey(pos));

    if ((pos->side == WHITE) && (pos->enPas != NO_SQ))
        assert(RanksBrd[pos->enPas] == RANK_6);
    if ((pos->side == BLACK) && (pos->enPas != NO_SQ))
        assert(RanksBrd[pos->enPas] == RANK_3);

    pawn_count = CNT(t_pawns[WHITE]);
    assert(pawn_count == pos->pceNum[wP]);
    pawn_count = CNT(t_pawns[BLACK]);
    assert(pawn_count == pos->pceNum[bP]);
    pawn_count = CNT(t_pawns[BOTH]);
    assert(pawn_count == pos->pceNum[wP] + pos->pceNum[bP]);

    while (t_pawns[WHITE]) {
        sq64 = POP(&t_pawns[WHITE]);
        assert(pos->pieces[SQ120(sq64)] == wP);
    }
    while (t_pawns[BLACK]) {
        sq64 = POP(&t_pawns[BLACK]);
        assert(pos->pieces[SQ120(sq64)] == bP);
    }
    while (t_pawns[BOTH]) {
        sq64 = POP(&t_pawns[BOTH]);
        assert((pos->pieces[SQ120(sq64)] == wP) || (pos->pieces[SQ120(sq64)] == bP));
    }

    assert(pos->side == WHITE || pos->side == BLACK);

    return TRUE;
}

void UpdateListsMaterial(S_BOARD *pos) {
    int piece, sq, index, color;
    for (index = 0; index < BRD_SQ_NUM; ++index) {
        sq = index;
        piece = pos->pieces[index];
        if (piece != OFFBOARD && piece != EMPTY) {
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

            pos->pceNum[piece]++;
        }
    }
}

int ParseFen(char *fen, S_BOARD *pos) {
    /*
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
     */

    assert(fen != NULL);
    assert(pos != NULL);

    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;

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
            file = FILE_A;
            continue;
        default:
            printf("FEN error\n");
            return 0;
        }

        for (int i = 0; i < count; i++) {
            int sq = FR2SQ(file, rank);
            pos->pieces[sq] = piece;
            if (PieceValid(piece)) {
                NNChangePiece(piece, sq, 1);
            }
            file += 1;
        }
        fen++;
    }

    assert(*fen == 'w' || *fen == 'b');

    pos->side = (*fen == 'w') ? WHITE : BLACK;
    if (pos->side) {
//        NNChangeFlag(NN_FEAT_TURN, 1);
    }

    fen += 2;

    // castle permissions
    while (*fen != ' ') {
        switch (*fen) {
        case 'K':
            pos->castlePerm |= WKCA;
            NNChangeFlag(NN_FEAT_WKCA, 1);
            break;
        case 'Q':
            pos->castlePerm |= WQCA;
            NNChangeFlag(NN_FEAT_WQCA, 1);
            break;
        case 'k':
            pos->castlePerm |= BKCA;
            NNChangeFlag(NN_FEAT_BKCA, 1);
            break;
        case 'q':
            pos->castlePerm |= BQCA;
            NNChangeFlag(NN_FEAT_BQCA, 1);
            break;
        case '-':
            break;
        default:
            break;
        }
        fen++;
    }
    fen++;

    assert(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    // en passant square
    if (*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';
        assert(file >= FILE_A && file <= FILE_H);
        assert(rank == RANK_3 || rank == RANK_6);
        pos->enPas = FR2SQ(file, rank);
    } else {
        fen += 2;
    }

    pos->posKey = GeneratePosKey(pos);

    UpdateListsMaterial(pos);

    return 1;
}

void ResetBoard(S_BOARD *pos) {
    int i = 0;
    int j = 0;

    for (i = 0; i < 13; i++) {
        for (j = 0; j < 10; j++) {
            pos->pList[i][j] = OFFBOARD;
        }
    }

    for (i = 0; i < BRD_SQ_NUM; ++i) {
        pos->pieces[i] = OFFBOARD;
    }

    for (i = 0; i < 64; ++i) {
        pos->pieces[SQ120(i)] = EMPTY;
    }

    for (i = 0; i < 2; i++) {
        pos->bigPce[i] = 0;
        pos->majPce[i] = 0;
        pos->minPce[i] = 0;
        pos->material[i] = 0;
    }

    for (i = 0; i < 3; i++) {
        pos->pawns[i] = 0ULL;
    }

    for (i = 0; i < 13; i++) {
        pos->pceNum[i] = 0;
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

    NNReset();
}

void PrintBoard(const S_BOARD *pos) {
    int rank = 0;
    int file = 0;
    printf("Board:\n");
    for (rank = RANK_8; rank >= RANK_1; rank--) {
        printf("\n");
        for (file = FILE_A; file <= FILE_H; file++) {
            printf("%3c", PceChar[pos->pieces[FR2SQ(file, rank)]]);
        }
    }
    printf("\n");
    for (int file = FILE_A; file <= FILE_H; file++) {
        printf("%3c", 'a' + file);
    }
    printf("\n");
    printf("Side to move: %c\n", SideChar[pos->side]);

    printf("enPas:%d\n", pos->enPas);
    printf("castle:%c%c%c%c\n", pos->castlePerm & WKCA ? 'K' : '-', pos->castlePerm & WQCA ? 'Q' : '-',
           pos->castlePerm & BKCA ? 'k' : '-', pos->castlePerm & BQCA ? 'q' : '-');
    printf("PosKey:%0llx\n", pos->posKey);
    printf("History: ");
    for (int his = 0; his < pos->hisPly; his++) {
        printf("%s ", PrMove(pos->history[his].move));
    }
    printf("\n");
}

S_BOARD *CreateNewBoard() {
    static S_BOARD board[1];
    board->pvtable->p_table = NULL;
    InitPvTable(board->pvtable);
    return board;
}