#include "defs.h"
#include "stdio.h"

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

int main() {
    printf("%s\n", NAME);
    printf("Initializing.\n");
    AllInit();

    S_BOARD board;
//    printf("Parsing\n");
//    ParseFen(START_FEN, &board);
//    PrintBoard(&board);
//    printf("Hello");
//    CheckBoard(&board);

//    char fen[] = "6k1/5p2/6p1/8/7p/8/6PP/6K1 b - - 0 0";
//    char fen[] = "3k4/2n2B2/1KP5/2B2p2/5b1p/7P/8/8 b - - 0 0";
    char fen[] = "8/3q4/8/8/4Q3/8/8/8 w  - - 0 0";
    S_BOARD pos;
    ParseFen(fen, &pos);
    PrintBoard(&pos);
    printf("\n");
    VisualizeAttackedSquares(&pos, WHITE);
    printf("\n");
    VisualizeAttackedSquares(&pos, BLACK);

    return 0;
}
