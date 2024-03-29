#ifndef REGION_LAYER_H
#define REGION_LAYER_H
#ifdef __cplusplus
 extern "C"{
#endif

#include "layer.h"
#include "network.h"

typedef layer region_layer;

region_layer make_region_layer(int batch, int h, int w, int n, int classes, int coords);
void forward_region_layer(const region_layer l, network_state state);
void backward_region_layer(const region_layer l, network_state state);
// void get_region_boxes(layer l, int w, int h, float thresh, float **probs, box *boxes, int only_objectness);
void get_region_boxes(layer l, int w, int h, float thresh, float **probs, box *boxes, int only_objectness, int *map,
                      float tree_thresh);
void resize_region_layer(layer *l, int w, int h);

#ifdef GPU
void forward_region_layer_gpu(const region_layer l, network_state state);
void backward_region_layer_gpu(region_layer l, network_state state);
#endif
#ifdef __cplusplus
}
#endif
#endif
