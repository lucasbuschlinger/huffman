#ifndef COMPRESS_H
#define COMPRESS_H

#include "tree.h"
#include "bitfile.h"

typedef struct {
    unsigned int code;
    int length;
} CODE_MAPPING;

typedef struct {
    CODE_MAPPING *table[256];
} CODE_TABLE;

void compress(const char* inFileName, const char* outFileName);
void update_forest(TREE *forest[], TREE *min, int min_pos, TREE *second_min, int second_pos, int no_trees);
CODE_TABLE* make_table(TREE* tree);
void make_table_helper( TREE* tree, CODE_TABLE* table, unsigned int code, int length);
void encode(FILE* file, BITFILE* bf, CODE_TABLE* table);
void encode_tree(BITFILE* bf, TREE* tree);

#endif