#ifndef ROUTE_LAYER_H
#define ROUTE_LAYER_H
#ifdef __cplusplus
 extern "C"{
#endif
#include "network.h"
#include "layer.h"

typedef layer route_layer;

route_layer make_route_layer(int batch, int n, int *input_layers, int *input_size);
void forward_route_layer(const route_layer l, network_state state);
void backward_route_layer(const route_layer l, network_state state);

#ifdef GPU
void forward_route_layer_gpu(const route_layer l, network_state state);
void backward_route_layer_gpu(const route_layer l, network_state state);
#endif
#ifdef __cplusplus
}
#endif
#endif
