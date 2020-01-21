#ifndef TREE_H
#define TREE_H

typedef struct TREE{
    /* if left==0, it is a leaf. Otherwise it has two branches */
    struct TREE* left;
    struct TREE* right;   /* not  in a leaf */
    int chr;              /* only in a leaf */
    int freq;            /* occurence rate of the tree */
} TREE;

void print_tree(TREE* tree);
TREE* new_tree();

#endif