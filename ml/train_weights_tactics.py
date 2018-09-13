from functools import reduce
from multiprocessing.pool import Pool

import chess
import joblib
import keras as ks
import numpy as np
from scipy import sparse
from scipy.stats import pearsonr
from sklearn.model_selection import train_test_split

from utils import dump_keras_mlp

"""
squares flags
corr (0.8478230557896308, 0.0)
sign acc 0.8310349453886676


"""

def mean_squared_error_with_sign(y_true, y_pred):
    return K.mean(K.square(y_pred - y_true), axis=-1) - K.sign(y_true) * K.sign(y_pred)


pieces_list = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
pieces_value = [0.1, 0.3, 0.3, 0.4, 0.9, 10, -0.1, -0.3, -0.3, -0.4, -0.9, -10]
from keras import backend as K



with Pool(8) as pool:
    positions = pool.map(joblib.load, ["datasets/positions_%d_squares_flags_attackers_pins.joblib" % n for n in range(1, 2)])
Xs, yraws, rndss = list(zip(*positions))

X = sparse.vstack(Xs)
y = np.array(reduce(lambda x, y: x + y, yraws)).clip(-20, 20)
rnds = np.array(reduce(lambda x, y: x + y, rndss))

rnds_set = set(rnds)
rnds_tr, rnds_te = train_test_split(list(rnds_set), random_state=1)
tr_ind = np.where(np.in1d(rnds, rnds_tr))[0]
te_ind = np.where(np.in1d(rnds, rnds_te))[0]

X_tr = X[tr_ind, :]
y_tr = y[tr_ind]
X_te = X[te_ind, :]
y_te = y[te_ind]

test_brd = chess.Board("rnb1kbnr/ppp2ppp/8/3pp1q1/8/2N5/PPPPPPPP/R1BQKB1R w KQkq - 0 4")


def get_model_dnn(loss):
    model_in = ks.Input(shape=(X_tr.shape[1],), dtype='float32', sparse=True)
    out = ks.layers.Dense(256, activation='relu')(model_in)
    # out = ks.layers.Dropout(0.5)(out)
    out = ks.layers.Dense(1, activation='linear')(out)
    model_dnn = ks.Model(model_in, out)
    model_dnn.compile(loss=loss,
                      optimizer=ks.optimizers.Adam(lr=0.001, beta_1=0.90, beta_2=0.99, epsilon=1e-08))
    return model_dnn


model_dnn = get_model_dnn("mean_squared_error")
model_dnn.fit(X_tr, y_tr, batch_size=4096, epochs=1, validation_data=(X_te, y_te))
K.set_value(model_dnn.optimizer.lr, 0.0001)
model_dnn.fit(X_tr, y_tr, batch_size=4096, epochs=1, validation_data=(X_te, y_te))
pred_dnn_te = model_dnn.predict(X_te)[:, 0]
print("corr", pearsonr(y_te, pred_dnn_te))
print("sign acc", (np.sign(y_te) == np.sign(pred_dnn_te)).mean())
# pred_dnn_tr = model_dnn.predict(X_tr)[:, 0]
# print(pearsonr(y_tr, pred_dnn_tr))
# print(model_dnn.predict(np.array(prepare_features_only(test_brd)).reshape(1, -1)))
# print()

# dump_keras_mlp(model_dnn, "weights_256.txt")

# model_nn = MLPRegressor(hidden_layer_sizes=(100,), activation="relu", alpha=0.01, max_iter=20, verbose=True)
# model_nn.fit(X_tr, y_tr)
# pred_nn_te = model_nn.predict(X_te)
# print(pearsonr(y_te, pred_nn_te))
# print("corr", pearsonr(y_te, pred_nn_te))
# print("sign acc", (np.sign(y_te) == np.sign(pred_nn_te)).mean())
# pred_nn_tr = model_nn.predict(X_tr)
# print(pearsonr(y_tr, pred_nn_tr))
# print(model_nn.predict(np.array(prepare_features_only(brd)).reshape(1, -1)))
# print()


# y_tr_cat = np.round(y_tr)
# y_te_cat = np.round(y_te)
# lb = LabelBinarizer()
# y_tr_cat = lb.fit_transform(y_tr_cat)
# y_te_cat = lb.transform(y_te_cat)
#
# model_in = ks.Input(shape=(X_tr.shape[1],), dtype='float32', sparse=True)
# out = ks.layers.Dense(25, activation='sigmoid')(model_in)
# out = ks.layers.Dropout(0.5)(out)
# out = ks.layers.Dense(y_tr_cat.shape[1], activation='softmax')(out)
# model_dnn = ks.Model(model_in, out)
# model_dnn.compile(loss="binary_crossentropy",
#                   optimizer=ks.optimizers.Adam(lr=0.01, beta_1=0.90, beta_2=0.99, epsilon=1e-08))
# model_dnn.fit(X_tr, y_tr_cat, batch_size=128, epochs=10, validation_data=(X_te, y_te_cat))
# print("eval", model_dnn.predict(csr_matrix(np.array(prepare_features_only(brd)).reshape(1, -1))))
#
# print("test pearson", pearsonr(y_te, pred_dnn_te))
#     # pred_dnn_tr = model.predict(X_tr).reshape(-1)
#     # print(pearsonr(y_tr, pred_dnn_tr))
#
# print(pearsonr(y_te, pred_lgbm_te + pred_nn_te + pred_dnn_te))
