#include "defs.h"

int sq120to64[BRD_SQ_NUM];
int sq64to120[64];
U64 SetMask[64];
U64 ClearMask[64];
int FilesBrd[BRD_SQ_NUM];
int RanksBrd[BRD_SQ_NUM];

static void InitHashKeys() {
    for (int square = 0; square < 120; square++) {
        for (int piece = 0; piece < 13; piece++) {
            PieceKeys[piece][square] = rand64();
        }
    }
    SideKey = rand64();
    for (int castle_ind = 0; castle_ind < 16; castle_ind++) {
        CastleKeys[castle_ind] = rand64();
    }
}

static void InitBitMasks() {
    int index = 0;
    for (index = 0; index < 64; index++) {
        SetMask[index] = 0ULL;
        ClearMask[index] = 0ULL;
    }

    for (index = 0; index < 64; index++) {
        SetMask[index] |= (1ULL << index);
        ClearMask[index] = ~SetMask[index];
    }
}

static void InitSqMaps() {
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

    for (index = 0; index < BRD_SQ_NUM; ++index) {
        sq120to64[index] = 65;
    }

    for (index = 0; index < 64; ++index) {
        sq64to120[index] = 120;
    }

    for (rank = RANK_1; rank <= RANK_8; ++rank) {
        for (file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file, rank);
            sq64to120[sq64] = sq;
            sq120to64[sq] = sq64;
            sq64++;
        }
    }
}

static void InitFilesRanksBrd() {

    int sq = A1;

    for (int index = 0; index < BRD_SQ_NUM; index++) {
        FilesBrd[index] = OFFBOARD;
        RanksBrd[index] = OFFBOARD;
    }

    for (int rank = RANK_1; rank <= RANK_8; rank++) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

void AllInit() {
    InitHashKeys();
    InitSqMaps();
    InitBitMasks();
    InitFilesRanksBrd();
    InitMvvLva();
}
