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
    printf("Parsing\n");
    ParseFen(START_FEN, &board);
    PrintBoard(&board);
    CheckBoard(&board);

    return 0;
}
