#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <stdio.h>
#include "tree.h"
#include "bitfile.h"

void decompress(const char* inFileName, const char* outFileName);
TREE* decode_tree(BITFILE* bf);
TREE* decode_tree_helper(BITFILE* bf, TREE* tree);
void decode_file(BITFILE *bf, TREE* tree, FILE* file);

#endif