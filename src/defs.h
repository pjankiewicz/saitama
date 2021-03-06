#pragma once

#define NDEBUG
//#define NNINC //incremental nn updates

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long U64;

#define NAME "Saitama 1.0 PS"
#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048 // max half moves
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define PRINTARRAY(array, length)                                                                                      \
    for (int i = 0; i < (length); i++)                                                                                 \
        printf("%d\t", (array)[i]);

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
    int score;
} S_MOVE;

/* GAME MOVE */

/*

encode whole move in a single int

 0000 0000 0000 0000 0000 0111 1111 -> From (7 bits) 0x7F
 0000 0000 0000 0011 1111 1000 0000 -> To   (7 bits) >> 7, 0x7F
 0000 0000 0011 1100 0000 0000 0000 -> Captured (4 bits) >> 14, 0xF
 0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
 0000 0000 1000 0000 0000 0000 0000 -> Pawn start 0x80000
 0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20 0xF
 0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000

 These days I prefer:
* from square: 6-bits
* to square: 6-bits
* promotion: 4 bits (Knight=0...Queen=3)

 You don't need flags for en passant, castling, or promotion:
* move is promotion is from square has a pawn on it, to square is on the (relative) eighth rank
* move is en passant if to square is en passant square, and from square has a pawn on it
* move is castling if king captures his own rook (handles chess and chess960 in a unified way).
 */

#define FROMSQ(m) ((m)&0x7f)
#define TOSQ(m) (((m) >> 7) & 0x7f)
#define CAPTURED(m) (((m) >> 14) & 0xf)
#define PROMOTED(m) (((m) >> 20) & 0xf)

#define MFLAGEP 0x00040000
#define MFLAGPS 0x00080000
#define MFLAGCA 0x01000000
#define MFLAGCAP 0x0007C000
#define MFLAGPROM 0x00f00000
#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
#define NOMOVE (0)

typedef struct {
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;

typedef struct {
    U64 posKey;
    int move;
} S_PVENTRY;

typedef struct {
    S_PVENTRY *p_table;
    int count;
} S_PVTABLE;

typedef struct {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey;
} S_UNDO;

typedef struct {
    int pieces[BRD_SQ_NUM]; // main board
    int kingSq[2];          // where are the kings
    int side;  // which side to move
    int enPas; // which square is to take en passant
    int fiftyMove; // counter for fifty move rule
    int ply;
    int hisPly;
    int castlePerm;
    U64 posKey;     // unique key generated for each position
    int pceNum[13]; // number of pieces of each type
    S_UNDO history[MAXGAMEMOVES];
    int pList[13][10]; // where is each piece
    S_PVTABLE pvtable[1];
    int pvarray[MAXDEPTH];
    int search_history[13][BRD_SQ_NUM];
    int search_killers[2][MAXDEPTH];
} S_BOARD;

typedef struct {
    U64 starttime;
    U64 stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;
    long nodes;
    int quit;
    int stopped;
    int fh;
    int fhf;
} S_SEARCHINFO;

extern int sq120to64[BRD_SQ_NUM];
extern int sq64to120[64];

/* MACROS */ //

#define FR2SQ(f, r) ((21 + (f) + ((r)*10)))
#define SQ64(sq120) (sq120to64[(sq120)])
#define SQ120(sq64) (sq64to120[(sq64)])
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define IsKn(p) ((p) == wN || (p) == bN)
#define IsKi(p) ((p) == wK || (p) == bK)
#define IsBQ(p) ((p) == wB || (p) == bB || (p) == wQ || (p) == bQ)
#define IsQ(p) ((p) == wQ || (p) == bQ)
#define IsB(p) ((p) == wB || (p) == bB)
#define IsRQ(p) ((p) == wR || (p) == bR || (p) == wQ || (p) == bQ)
#define IsR(p) ((p) == wR || (p) == bR)

/* GLOBALS */

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

extern int CheckBoard(const S_BOARD *pos);

extern void UpdateListsMaterial(S_BOARD *pos);

extern int ParseFen(char *fen, S_BOARD *pos);

extern void ResetBoard(S_BOARD *pos);

extern void PrintBoard(const S_BOARD *pos);

extern S_BOARD *CreateNewBoard();

// data.c

extern int PiecePawn[13];
extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PieceSlides[13];

extern int PieceCol[13];

// attack.c
extern int KnightMoves[8];
extern int BishopMoves[4];
extern int QueenMoves[8];
extern int KingMoves[8];
extern int RookMoves[4];

extern int SqAttacked(const S_BOARD *pos, int sq, int side);

extern void VisualizeAttackedSquares(S_BOARD *pos, int side);

// io.c

extern char *PrSq(int sq);

extern char *PrMove(int move);

extern char *PrMoveDet(int move);

extern void PrintMoveList(const S_MOVELIST *list);

extern int ParseMove(const char *ptrChar, S_BOARD *pos);

// movegen.c

extern const int UP;
extern const int DOWN;
extern const int LEFT;
extern const int RIGHT;

extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *moves);

extern void GenerateAllCaptures(const S_BOARD *pos, S_MOVELIST *list);

extern int MoveExists(S_BOARD *pos, int move);

extern void InitMvvLva();

// validate.c

int SqOnBoard(int sq);

int SideValid(int side);

int FileRankValid(int fr);

int PieceValidEmpty(int pce);

int PieceValid(int pce);

// makemove.c

extern int MakeMove(S_BOARD *pos, int move);

extern void TakeMove(S_BOARD *pos);

// perft.c
extern void TestPerf();

// search.c
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

extern int IsRepetition(const S_BOARD *pos);

// misc.c
extern U64 getRealTime();

extern int InputWaiting();

extern void ReadInput(S_SEARCHINFO *info);

// pvtable.c
extern int GetPvLine(S_BOARD *pos, const int depth);

extern void InitPvTable(S_PVTABLE *table);

extern void StorePvMove(const S_BOARD *pos, int move);

extern int ProbePvTable(const S_BOARD *pos);

extern void ClearPvTable(S_PVTABLE *table);

// evaluate.c

const int PieceSquareEvals[13][64];

extern int EvalPosition(S_BOARD *pos);

extern int EvalPieceOnSquare(int piece, int square);

// uci.c

extern void ParseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos);

extern void ParsePosition(char *lineIn, S_BOARD *pos);

extern void UciLoop();