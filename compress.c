#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "compress.h"

CODE_TABLE* new_table() {
    CODE_TABLE* t = malloc(sizeof(CODE_TABLE));
    for (int i = 0; i < 256; i++) {
        t->table[i] = malloc(sizeof(CODE_MAPPING));
    }
    return t;
}

void compress(const char* inFileName, const char* outFileName) {
    int c, i;
    int occurances[256];
    memset(occurances, 0, sizeof(occurances));

    FILE* inFile = inFileName ? fopen(inFileName, "rb") : stdin;
    FILE* outFile = outFileName ? fopen(outFileName, "wb+") : stdout;

    if (inFile == NULL) {
        fprintf(stderr, "Couldn't open file '%s', exiting...\n", inFileName);
        exit(1);
    }
    if (outFile == NULL) {
        fprintf(stderr, "Couldn't open file '%s', exiting...\n", outFileName);
        exit(1);
    }

    if (inFileName && !regularFile(inFileName)) {
        fprintf(stderr, "File '%s' is not a regular file, exiting...\n", inFileName);
        exit(1);
    }

    if (emptyFile(inFile)) {
        fprintf(stderr, "File '%s' is empty, nothing to compress, exiting...\n", inFileName);
        exit(1);
    }

    BITFILE *bf = bitOpen(outFile);
    putbits(3, 0b000, bf);      // padding
    putbits(5, SIGNATURE, bf);    // signature

    while (fread(&c, sizeof(char), 1, inFile)) {
        occurances[c]++;
    }

    TREE* forest[256];
    int no_trees = 0;

    for(i = 0; i < 256; i++){
        int freq = occurances[i];
        if (freq > 0) {
            TREE* t = new_tree();
            t->chr = i;
            t->freq = freq;
            forest[no_trees++] = t;
        } else {
        }
   }


   for (i = 0; i < no_trees; i++) {
       TREE* t = forest[i];
   }

    while (no_trees > 1) {
        int min_pos, second_min_pos = __INT_MAX__;
        TREE* min = new_tree();
        TREE* second_min = new_tree();
        min->freq = __INT_MAX__;
        second_min->freq = __INT_MAX__;
        for (int i = 0; i < no_trees; i++) {
            TREE* t = forest[i];
            if (t->freq < min->freq) {
                second_min = min;
                min = forest[i];
                second_min_pos = min_pos;
                min_pos = i;
            }
            else if (t->freq < second_min->freq) {
                second_min = forest[i];
                second_min_pos = i;
            }
        }
        
        update_forest(forest, min, min_pos, second_min, second_min_pos, no_trees);
        no_trees--;
    }

    TREE* final_tree = forest[0];

    CODE_TABLE* table = make_table(final_tree);

    fseek(inFile, 0L, SEEK_SET);
    encode_tree(bf, final_tree);
    encode(inFile, bf, table);
    bitClose(bf);
}

void update_forest(TREE *forest[], TREE *min, int min_pos, TREE *second_min, int second_pos, int no_trees) {
    TREE* new_leaf = new_tree();
    new_leaf->left = min;
    new_leaf->right = second_min;
    new_leaf->chr = -1;
    new_leaf->freq = (min->freq + second_min->freq);

    forest[min_pos] = new_leaf;

    for (int i = second_pos; i < (no_trees - 1); i++) {
        forest[i] = forest[i + 1];
    }
    
}

CODE_TABLE* make_table(TREE* tree) {
    CODE_TABLE* table = new_table();

    make_table_helper(tree, table, 0, 0);

    return table;
}

void make_table_helper(TREE* tree, CODE_TABLE* table, unsigned int code, int length) {
    if (tree->chr == -1) { //not a leaf, extend code pass down
        make_table_helper(tree->left,  table, code << 1, length + 1);
        make_table_helper(tree->right, table, ((code << 1) | 1), length + 1);
    }
    else { //leaf, update char table with code
        table->table[tree->chr]->code = code;
        table->table[tree->chr]->length = length? length : length + 1;
    }
}

void encode(FILE* file, BITFILE* bf, CODE_TABLE* table) {
    int c = 0;
    while (fread(&c, sizeof(char), 1, file)) {
        putbits(table->table[c]->length, table->table[c]->code, bf);
    }
}

void encode_tree(BITFILE* bf, TREE* tree) {
    if (tree->left != NULL) { //not a leaf
        putbits(1, 1, bf);
        encode_tree(bf, tree->left);
        encode_tree(bf, tree->right);
    } else { // leaf
        putbits(9, tree->chr, bf);
    }
}