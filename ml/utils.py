def dump_sklearn_mlp(model_nn):
    def dump_weights(model_nn):
        weights = []
        for w in model_nn.coefs_:
            weights.extend(list(w.reshape(-1)))
        for w in model_nn.intercepts_:
            weights.extend(list(w.reshape(-1)))
        return weights

    weights = dump_weights(model_nn)
    with open("weights_100_keras.txt", "wt") as out:
        for w in weights:
            out.write("%s\n" % (str(w)))


def dump_keras_mlp(model_nn, path):
    with open(path, "wt") as out:
        for layer in model_nn.get_weights():
            for w in layer.reshape(-1):
                out.write("%s\n" % (str(w)))
