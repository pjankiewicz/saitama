#include "defs.h"
#include "mlp.h"

S_NN nn[1];

static double relu(double x) {
    if (x < 0) {
        return 0;
    } else {
        return x;
    }
}

void NNReadWeights() {
    FILE *wfile;
    wfile = fopen("ml/weights_256.txt", "r");
    double checksum = 0;
    int cnt = 0;
    for (int j = 0; j < NN_FEATURES; j++) {
        for (int i = 0; i < NN_NEURONS; i++) {
            fscanf(wfile, "%lf", &nn->layer_1_coefs[j][i]);
            checksum += nn->layer_1_coefs[j][i];
            cnt++;
        }
    }
    for (int i = 0; i < NN_NEURONS; i++) {
        fscanf(wfile, "%lf", &nn->layer_1_bias[i]);
        checksum += nn->layer_1_bias[i];
        cnt++;
    }

    for (int i = 0; i < NN_NEURONS; i++) {
        fscanf(wfile, "%lf", &nn->layer_2_coefs[i]);
        checksum += nn->layer_2_coefs[i];
        cnt++;
    }

    fscanf(wfile, "%lf", &nn->layer_2_bias[0]);
    checksum += nn->layer_2_bias[0];
    cnt++;

    NNReset();
}

void NNReset() {
    for (int i = 0; i < NN_NEURONS; i++) {
        nn->layer_1_act[i] = 0;
    }
}

int NNEvalPositionAll(S_BOARD *pos) {
    int featureInd = 0;
    int sq = 0;
    int rank, file, neuron_ind;
    for (int i = 0; i < NN_NEURONS; i++) {
        nn->layer_1_act[i] = 0;
    }

    for (int pce = wP; pce <= bK; pce++) {
        for (int pceInd = 0; pceInd < pos->pceNum[pce]; pceInd++) {
            sq = pos->pList[pce][pceInd];
            rank = RanksBrd[sq];
            file = FilesBrd[sq];
            featureInd = (pce - 1) * 8 * 8 + rank * 8 + file;
            assert(featureInd < NN_NEURONS * NN_FEATURES);
            for (neuron_ind = 0; neuron_ind < NN_NEURONS; neuron_ind++) {
                nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[featureInd][neuron_ind];
            }
        }
    }

    int turnInd = 12 * 8 * 8;
    int wkcaInd = turnInd + 1;
    int wqcaInd = turnInd + 2;
    int bkcaInd = turnInd + 3;
    int bqcaInd = turnInd + 4;

    double s = 0;
    for (neuron_ind = 0; neuron_ind < NN_NEURONS; neuron_ind++) {
        // bias
        if (pos->side == WHITE) nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[turnInd][neuron_ind];
        if (pos->castlePerm & WKCA) nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[wkcaInd][neuron_ind];
        if (pos->castlePerm & WQCA) nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[wqcaInd][neuron_ind];
        if (pos->castlePerm & BKCA) nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[bkcaInd][neuron_ind];
        if (pos->castlePerm & BQCA) nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[bqcaInd][neuron_ind];
        s += relu(nn->layer_1_act[neuron_ind] + nn->layer_1_bias[neuron_ind]) * nn->layer_2_coefs[neuron_ind];
    }

    s += nn->layer_2_bias[0];

    if (pos->side == WHITE) {
        return (int) (s * 100);
    } else {
        return (int) (-s * 100);
    }
}

int NNEvalPositionInc(S_BOARD *pos) {
    int neuron_ind;

    int turnInd = 12 * 8 * 8;
    int wkcaInd = turnInd + 1;
    int wqcaInd = turnInd + 2;
    int bkcaInd = turnInd + 3;
    int bqcaInd = turnInd + 4;

    double s = 0;
    double layer_value = 0;
    for (neuron_ind = 0; neuron_ind < NN_NEURONS; neuron_ind++) {
//         bias
        layer_value = nn->layer_1_act[neuron_ind];
        if (pos->side == WHITE) layer_value += nn->layer_1_coefs[turnInd][neuron_ind];
//        if (pos->castlePerm & WKCA) layer_value += nn->layer_1_coefs[wkcaInd][neuron_ind];
//        if (pos->castlePerm & WQCA) layer_value += nn->layer_1_coefs[wqcaInd][neuron_ind];
//        if (pos->castlePerm & BKCA) layer_value += nn->layer_1_coefs[bkcaInd][neuron_ind];
//        if (pos->castlePerm & BQCA) layer_value += nn->layer_1_coefs[bqcaInd][neuron_ind];
        s += relu((layer_value) + nn->layer_1_bias[neuron_ind]) * nn->layer_2_coefs[neuron_ind];
    }

    s += nn->layer_2_bias[0];

    if (pos->side == WHITE) {
        return (int) (s * 100);
    } else {
        return (int) (-s * 100);
    }
}

void NNChangePiece(int pce, int sq, int add) {
    int rank = RanksBrd[sq];
    int file = FilesBrd[sq];
    int featureInd = (pce - 1) * 8 * 8 + rank * 8 + file;
    assert(featureInd < NN_NEURONS * NN_FEATURES);
    assert(PieceValid(pce));
    for (int neuron_ind = 0; neuron_ind < NN_NEURONS; neuron_ind++) {
        if (add) {
            nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[featureInd][neuron_ind];
        } else {
            nn->layer_1_act[neuron_ind] -= nn->layer_1_coefs[featureInd][neuron_ind];
        }
    }
}

void NNChangeFlag(int flag_ind, int add) {
    for (int neuron_ind = 0; neuron_ind < NN_NEURONS; neuron_ind++) {
        if (add) {
            nn->layer_1_act[neuron_ind] += nn->layer_1_coefs[flag_ind][neuron_ind];
        } else {
            nn->layer_1_act[neuron_ind] -= nn->layer_1_coefs[flag_ind][neuron_ind];
        }
    }
}