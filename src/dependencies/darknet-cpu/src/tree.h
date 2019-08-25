#ifndef TREE_H
#define TREE_H
#ifdef __cplusplus
 extern "C"{
#endif

typedef struct{
    int *leaf;
    int n;
    int *parent;
    int *child;
    int *group;
    char **name;

    int groups;
    int *group_size;
    int *group_offset;
} tree;

tree *read_tree(char *filename);
int hierarchy_top_prediction(float *predictions, tree *hier, float thresh);
#ifdef __cplusplus
}
#endif
#endif
