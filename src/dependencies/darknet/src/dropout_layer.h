#ifndef DROPOUT_LAYER_H
#define DROPOUT_LAYER_H

#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef layer dropout_layer;

dropout_layer make_dropout_layer(int batch, int inputs, float probability);

void forward_dropout_layer(dropout_layer l, network_state state);
void backward_dropout_layer(dropout_layer l, network_state state);
void resize_dropout_layer(dropout_layer *l, int inputs);

#ifdef GPUDARKNET
void forward_dropout_layer_gpu(dropout_layer l, network_state state);
void backward_dropout_layer_gpu(dropout_layer l, network_state state);

#endif

#ifdef __cplusplus
}
#endif

#endif
