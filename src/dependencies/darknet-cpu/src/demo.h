#ifndef DEMO
#define DEMO
#ifdef __cplusplus
 extern "C"{
#endif

#include "image.h"
void demo(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int frame_skip, char *prefix);

#endif
#ifdef __cplusplus
}
#endif
