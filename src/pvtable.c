#include "defs.h"
#include "stdio.h"

#include "defs.h"
#include "stdio.h"

int GetPvLine(S_BOARD *pos, const int depth) {
    assert(depth < MAXDEPTH);

    int move = ProbePvTable(pos);
    int count = 0;

    while (move != NOMOVE && count < depth) {
        assert(count < MAXDEPTH);
        if (MoveExists(pos, move)) {
            MakeMove(pos, move);
            pos->pvarray[count++] = move;
        } else {
            break;
        }
        move = ProbePvTable(pos);
    }

    while (pos->ply > 0) {
        TakeMove(pos);
    }

    return count;
}

void ClearPvTable(S_PVTABLE *table) {
    S_PVENTRY *pv_entry;
    for (pv_entry = table->p_table; pv_entry < table->p_table + table->count; pv_entry++) {
        pv_entry->posKey = 0ULL;
        pv_entry->move = NOMOVE;
    }
}

void InitPvTable(S_PVTABLE *table) {
    const int PvSize = 0x100900 * 2;
    table->count = PvSize / sizeof(S_PVENTRY);
    table->count -= 2; // just to make sure we don't go over
    if (table->p_table != NULL) {
        free(table->p_table);
    }
    table->p_table = (S_PVENTRY *)malloc(table->count * sizeof(S_PVENTRY));
    ClearPvTable(table);
}

void StorePvMove(const S_BOARD *pos, const int move) {
    U64 index = pos->posKey % pos->pvtable->count;
    assert(index >= 0 && index && index < pos->pvtable->count);
    pos->pvtable->p_table[index].move = move;
    pos->pvtable->p_table[index].posKey = pos->posKey;
}

int ProbePvTable(const S_BOARD *pos) {
    U64 index = pos->posKey % pos->pvtable->count;
    assert(index >= 0 && index && index < pos->pvtable->count);
    if (pos->pvtable->p_table[index].posKey == pos->posKey) {
        return pos->pvtable->p_table[index].move;
    }
    return NOMOVE;
}