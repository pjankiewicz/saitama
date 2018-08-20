#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


typedef unsigned long long U64;

#define NAME "Saitama 1.0"
#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048 // max half moves

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum {
    EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK
};

enum {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE
};
enum {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE
};

enum {
    WHITE, BLACK, BOTH
};

// clang-format off
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8,
    NO_SQ,
    OFFBOARD
};
// clang-format on

enum {
    FALSE, TRUE
};

enum {
    WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8
};

typedef struct {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey;
} S_UNDO;

typedef struct {

    int pieces[BRD_SQ_NUM]; // main board
    U64 pawns[3];           // 1 bit for each square for 3 colors
    int kingSq[2];          // where are the kings

    int side;  // which side to move
    int enPas; // which square is to take en passant

    int fiftyMove; // counter for fifty move rule

    int ply;
    int hisPly;

    int castlePerm;

    U64 posKey;     // unique key generated for each position
    int pceNum[13]; // number of pieces of each type
    int bigPce[2];  // number of non pawns
    int majPce[2];  // number of major pieces
    int minPce[2];  // number of min pieces
    int material[2];

    S_UNDO history[MAXGAMEMOVES];

    int pList[13][10]; // where is each piece
} S_BOARD;

extern int sq120to64[BRD_SQ_NUM];
extern int sq64to120[64];

/* MACROS */

#define FR2SQ(f,r) ((21 + (f) + ((r)*10)))
#define SQ64(sq120) (sq120to64[(sq120)])
#define SQ120(sq64) (sq64to120[(sq64)])
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define IsKn(p) ((p) == wN || (p) == bN)
#define IsKi(p) ((p) == wK || (p) == bK)
#define IsBQ(p) ((p) == wB || (p) == bB || (p) == wQ || (p) == bQ)
#define IsRQ(p) ((p) == wR || (p) == bR || (p) == wQ || (p) == bQ)

/* GLOBALS */
extern int sq120to64[BRD_SQ_NUM];
extern int sq64to120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

/* FUNCTION */

// init.c
extern void AllInit();

// bitboards.c
extern const int BitTable[64];

extern int PopBit(U64 *bb);

extern int CountBits(U64 b);

extern void PrintBitBoard(U64 bb);

// hashkeys.c
extern U64 rand64();

extern U64 GeneratePosKey(const S_BOARD *pos);

// board.c
extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern int CheckBoard(S_BOARD *pos);

extern void UpdateListsMaterial(S_BOARD *pos);

extern int ParseFen(char *fen, S_BOARD *pos);

extern void ResetBoard(S_BOARD *pos);

extern void PrintBoard(const S_BOARD *pos);

// data.c

extern int PiecePawn[13];
extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PieceSlides[13];

extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];

// attack.c
extern int KnightMoves[8];
extern int BishopMoves[4];
extern int QueenMoves[8];
extern int KingMoves[8];
extern int RookMoves[4];

extern int SqAttacked(S_BOARD *pos, int sq, int side);
extern void VisualizeAttackedSquares(S_BOARD *pos, int side);