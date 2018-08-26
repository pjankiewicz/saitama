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
    S_BOARD board[1];
    S_MOVELIST moveslist[1];

    ParseFen(START_FEN, board);
    GenerateAllMoves(board, moveslist);

    int movenum = 0;
    int move = 0;

    for (int i=0; i<2024; i++) {
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

// vice.c

#define PAWNMOVES "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVES2 "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR b KQkq - 0 1"
#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1 "
#define BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1 "

int main() {
    AllInit();
    TestPerf();
    return 0;
}
