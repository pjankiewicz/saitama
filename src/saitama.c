#include "defs.h"

int printBoard() {
    int index = 0;
    for (index = 0; index < BRD_SQ_NUM; ++index) {
        if (index % 10 == 0)
            printf("\n");
        printf("%5d", sq120to64[index]);
    }
    printf("\n\n");
    for (index = 0; index < 64; ++index) {
        if (index % 8 == 0)
            printf("\n");
        printf("%5d", sq64to120[index]);
    }
    printf("\n\r");
}

void PrintMove(int move) {
    int from = FROMSQ(move);
    int to = TOSQ(move);
    int captured = CAPTURED(move);
    printf("Move: %s\n", PrMove(move));
    printf("From square: %d\n", from);
    printf("To square: %d\n", to);
    printf("Captured: %d\n", captured);
}

void StupidGame() {
    S_BOARD *board = CreateNewBoard();
    S_MOVELIST moveslist[1];

    ParseFen(START_FEN, board);
    GenerateAllMoves(board, moveslist);

    int movenum = 0;
    int move = 0;

    for (int i = 0; i < 2024; i++) {
        GenerateAllMoves(board, moveslist);
        for (movenum = 0; movenum < moveslist->count; ++movenum) {
            move = moveslist->moves[movenum].move;
            printf("%d %d %d %s\n", i, movenum, moveslist->count, PrMove(move));
            if (!MakeMove(board, move)) {
                continue;
            }
            PrintBoard(board);
            break;
        }
    }
}

void GUI() {
    S_BOARD *board = CreateNewBoard();
    S_MOVELIST list[1];

    ParseFen(START_FEN, board);
    printf("Initial evaluation %d\n", EvalPosition(board));
    // PerftTest(3,board);

    char input[6];
    int Move = NOMOVE;
    while (TRUE) {
        PrintBoard(board);
        printf("Please enter a move > ");
        fgets(input, 6, stdin);

        if (input[0] == 'q') {
            break;
        } else if (input[0] == 't') {
            TakeMove(board);
        } else {
            Move = ParseMove(input, board);
            if (Move != NOMOVE) {
                MakeMove(board, Move);
            } else {
                printf("Move Not Parsed:%s\n", input);
            }
        }

        fflush(stdin);
    }
}

void Benchmark() {
    char *fen = "rnb1kbnr/ppp2ppp/8/3pp1q1/8/2N5/PPPPPPPP/R1BQKB1R w KQkq - 0 4";
    S_BOARD *pos = CreateNewBoard();
    S_SEARCHINFO info[1];
    ParseFen(START_FEN, pos);

    printf("eval1: %d\n", EvalPosition(pos));

    info->depth = 8;
    SearchPosition(pos, info);
    free(pos->pvtable->p_table);
}

int main(int argc, char **argv) {
    AllInit();
    if (argc == 2) {
        switch (*argv[1]) {
        case 'p':
            TestPerf();
            break;
        case 'g':
            GUI();
            break;
        case 'b':
            Benchmark();
            break;
        default:
            UciLoop();
            break;
        }
    } else {
        UciLoop();
    }
    return 0;
}
