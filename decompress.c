#include <stdlib.h>
#include "decompress.h"

void decompress(const char* inFileName, const char* outFileName) {
    BITFILE* bf;
    FILE* inFile = inFileName ? fopen(inFileName, "rb") : stdin;
    FILE* outFile = outFileName ? fopen(outFileName, "wb") : stdout;
    if (inFileName && !regularFile(inFileName)) {
        fprintf(stderr, "File '%s' is not a regular file, exiting...\n", inFileName);
        exit(1);
    }
    if (emptyFile(inFile)) {
        fprintf(stderr, "File '%s' is empty, nothing to decompress, exiting...\n", inFileName);
        exit(1);
    }
    bf = bitOpen_decomp(inFile);
    TREE *rec_tree = decode_tree(bf);

    decode_file(bf, rec_tree, outFile);
    fflush(outFile);
    fclose(outFile);
}

TREE* decode_tree(BITFILE* bf) {
    TREE* t = new_tree();
    return decode_tree_helper(bf, t);
}

TREE* decode_tree_helper(BITFILE* bf, TREE* tree) {
    if (getbit(bf)) { //node
        tree->left = decode_tree_helper(bf, new_tree());
        tree->right = decode_tree_helper(bf, new_tree());
    } else { // leaf
        unsigned int temp = 0;
        for (int i = 0; i < 8; i++) { // get 8 bit = 1 byte = 1 char
            temp <<= 1;
            temp |= getbit(bf);
        }
        tree->chr = temp;
    }
    return tree;
}

void decode_file(BITFILE *bf, TREE* tree, FILE* file) {
    int currentBit;

    if (tree->left == NULL && tree->right == NULL) { //special case where we have a one-node-tree
        while ((currentBit = getbit(bf)) != -1) {
            fputc(tree->chr, file);
        }
    }
    else {
        TREE *currentTree = tree;
        while ((currentBit = getbit(bf)) != -1) {
            if (currentTree->left == NULL) { // we're at a leaf, lets first put that
                fputc(currentTree->chr, file);
                currentTree = tree;
                //continue;
            }
            if (currentBit) { // go right
                currentTree = currentTree->right;
            } else { // go left
                currentTree = currentTree->left;
            }
        }

        // check if on leaf (= no padding was present)
        if (currentTree->left == NULL) {
            fputc(currentTree->chr, file);
            currentTree = tree;
        }
    }
}