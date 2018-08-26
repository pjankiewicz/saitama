#include "defs.h"

static void CheckUp() {
    // .. check if time up or interrupt from GUI
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) { return 0; }

int IsRepetition(const S_BOARD *pos) {
    for (int i = pos->hisPly - pos->fiftyMove; i < pos->hisPly - 1; i++) {
        if (pos->posKey == pos->history[i].posKey) {
            return TRUE;
        }
    }
    return FALSE;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int do_null) { return 0; }

void SearchPosition(S_BOARD *pos) {
    // .. iterative deepening, search init
}