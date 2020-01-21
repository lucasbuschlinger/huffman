#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

TREE* new_tree() {
    TREE* t = malloc(sizeof(TREE));
    if (t == NULL) {
        printf("Something went wrong while allocating memory for a new tree, exiting...\n");
        exit(1);
    }
    t->chr = -1;
    t->freq = 0;
    t->left = NULL;
    t->right = NULL;

    return t;
}

void print_tree(TREE* tree) {
    if (tree->chr == -1) { //not a leaf
        printf("Node - %d\n", tree->freq);
        print_tree(tree->left);
        print_tree(tree->right);
    }
    else { //leaf
        printf("Leaf - %c (%d) - %d\n", tree->chr, tree->chr, tree->freq);
    }
}