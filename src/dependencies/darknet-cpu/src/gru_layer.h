
#ifndef GRU_LAYER_H
#define GRU_LAYER_H
#ifdef __cplusplus
 extern "C"{
#endif

#include "activations.h"
#include "layer.h"
#include "network.h"

layer make_gru_layer(int batch, int inputs, int outputs, int steps, int batch_normalize);

void forward_gru_layer(layer l, network_state state);
void backward_gru_layer(layer l, network_state state);
void update_gru_layer(layer l, int batch, float learning_rate, float momentum, float decay);

#ifdef GPU
void forward_gru_layer_gpu(layer l, network_state state);
void backward_gru_layer_gpu(layer l, network_state state);
void update_gru_layer_gpu(layer l, int batch, float learning_rate, float momentum, float decay);
void push_gru_layer(layer l);
void pull_gru_layer(layer l);
#endif
#ifdef __cplusplus
}
#endif
#endif
