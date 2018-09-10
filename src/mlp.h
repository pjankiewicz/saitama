#pragma once

#include "defs.h"

#define NN_FEATURES 773
#define NN_NEURONS 256

#define NN_FEAT_TURN (12 * 8 * 8)
#define NN_FEAT_WKCA (12 * 8 * 8 + 1)
#define NN_FEAT_WQCA (12 * 8 * 8 + 2)
#define NN_FEAT_BKCA (12 * 8 * 8 + 3)
#define NN_FEAT_BQCA (12 * 8 * 8 + 4)

typedef struct {
    int input[NN_FEATURES];
    double layer_1_coefs[NN_FEATURES][NN_NEURONS];
    double layer_1_bias[NN_NEURONS];
    double layer_1_act[NN_NEURONS];
    double layer_2_coefs[NN_NEURONS];
    double layer_2_bias[1];
    double layer_2_act[1];
} S_NN;

extern S_NN nn[1];

extern int NNEvalPositionWhite(S_BOARD *pos);

extern int NNEvalPositionBlack(S_BOARD *pos);

extern int NNEvalPositionInc(S_BOARD *pos);

extern void NNReadWeights();

extern void NNChangePiece(int pce, int sq, int add);

extern void NNMovePiece(int pce, int old_sq, int new_sq);

extern void NNChangeFlag(int flag_ind, int add);

extern void NNReset();