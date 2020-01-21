#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "decompress.h"

void printHelp();

int main(int argc, const char* argv[]) {

    if (argc == 1) {
        fprintf(stderr, "No input/output files specified, switching to STDIN/STDOUT for compression\n");
        compress(NULL, NULL);
    }
    else if (argc == 2) {
        if (!strcmp(argv[1], "-d")) {
            fprintf(stderr, "No input/output files specified, switching to STDIN/STDOUT for decompression\n");
            decompress(NULL, NULL);
        }
        else if (!strcmp(argv[1], "-h")) {
            printHelp();
        }
        else {
            fprintf(stderr, "Invalid arguments given, use -h to print the help\n");
            exit(1);
        }
    }
    else if (argc == 3) {
        compress(argv[1], argv[2]);
    }
    else if (argc == 4) {
        decompress(argv[2], argv[3]);
    }
    else {
        fprintf(stderr, "Invalid arguments given, use -h to print the help\n");
        exit(1);
    }
}

void printHelp() {
    printf("==================================================\n");
    printf("====== Help for the Huffman De-/Compressor =======\n");
    printf("==================================================\n\n");
    printf("./huffmann inFile outfile\t | Compress the inFile and receive the compressed data in the outFile\n");
    printf("./huffmann < STDIN > STDOUT\t | Compress the input from STDIN and receive the compressed data on STDOUT\n");
    printf("./huffmann -d inFile outfile\t | Decompress the inFile and receive the compressed data in the outFile\n");
    printf("./huffmann -d < STDIN > STDOUT\t | Decompress the input from STDIN and receive the decompressed data on STDOUT\n");
    printf("./huffmann -h\t\t\t | Print this help and exit\n\n");
    exit(0);
}