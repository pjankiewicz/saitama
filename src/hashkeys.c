#include "defs.h"

U64 rand64() {
    // http://vigna.di.unimi.it/ftp/papers/xorshift.pdf
    static U64 seed = 1070372ull;
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    return seed * 2685821657736338717ull;
}

U64 GeneratePosKey(const S_BOARD *pos) {
    U64 finalKey = 0;
    int piece = EMPTY;

    // hash pieces
    for (int sq = 0; sq < BRD_SQ_NUM; ++sq) {
        piece = pos->pieces[sq];
        if (piece != OFFBOARD && piece != EMPTY) {
            finalKey ^= PieceKeys[piece][sq];
        }
    }

    // hash side
    if (pos->side == WHITE)
        finalKey ^= SideKey;

    if (pos->enPas != NO_SQ) {
        assert(pos->enPas > 0 && pos->enPas <= BRD_SQ_NUM);
        finalKey ^= PieceKeys[EMPTY][pos->enPas];
    }

    assert(pos->castlePerm >= 0 && pos->castlePerm < 16);

    finalKey ^= CastleKeys[pos->castlePerm];

    return finalKey;
}