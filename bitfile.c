#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bitfile.h"

static int BUFFER_SIZE = sizeof(unsigned int) * 8;
static int BUFFER_SIZE_BYTES = sizeof(unsigned int);
static int MSB_MASK = (1 << (sizeof(unsigned int) * 8 - 1));
static int SIGNATURE = 0b00000;
int PADDING = -1;

BITFILE* bitOpen(FILE* f) {
    BITFILE* retVal = malloc(sizeof(BITFILE));
    if (retVal == NULL) {
        printf("Something went wrong while opening the bitfile (allocation)...");
        exit(1);
    }
    retVal->buffer = 0;
    retVal->counter = 0;
    retVal->f = f;
    return retVal;
}

BITFILE* bitOpen_decomp(FILE* f) {
    fseek(f, 0L, SEEK_END);
    NUMBER_BYTES_FILE = ftell(f);
    fseek(f, 0L, SEEK_SET);

    BITFILE* bf = bitOpen(f);
    PADDING = getPadding(bf);

    if (checkSignature(bf)) {
        fprintf(stderr, "Signature bits in file incorrect, cannot decompress...\n");
        exit(1);
    }
    return bf;
}

void* bitClose(BITFILE* bf) {
    unsigned int padding = 0;
    if (bf->counter == BUFFER_SIZE) { // no need for padding
        flushbuffer(bf);
    }
    else {
        padding = BUFFER_SIZE - bf->counter;
        bf->buffer <<= padding;
        flushbuffer_final(bf, padding); //special flush to not waste up to 3 byes
    }
    fflush(bf->f);
    // write padding to beginning 3 bits of file
    unsigned char temp_buffer = 0;
    long bytesWritten =  ftell(bf->f);
    padding <<= 5;
    temp_buffer |= padding;
    if (bytesWritten < 4) {
        fseek(bf->f, 0L, SEEK_SET); // write to byte 0 (until now 000 + 5 bits sig), no offset because bytes written in row
    } else {
        fseek(bf->f, 3L, SEEK_SET); // write to byte 0 (until now 000 + 5 bits sig), offset 3 because of endianess
    }
    fwrite(&temp_buffer, sizeof(unsigned char), 1, bf->f);
    fflush(bf->f);

    fclose(bf->f);
    free(bf);
}

void putbits(unsigned nbits, unsigned long word, BITFILE* bf){
    if ((bf->counter + nbits) > BUFFER_SIZE) {
        unsigned long copy = word;
        int push = (bf->counter + nbits) - BUFFER_SIZE;
        unsigned int mask = 0;
        int i;
        for (i = 0; i  < push; i++) {
            mask <<= 1;
            mask++;
        }

        word >>= push;
        bf->buffer <<= nbits-push;
        bf->buffer |= word;
        bf->counter += nbits-push;
        flushbuffer(bf);

        copy &= mask;
        bf->buffer = copy;
        bf->counter = push;

    }
    else {
        bf->buffer <<= nbits;
        bf->buffer |= word;
        bf->counter += nbits;
    }
}

int getbit(BITFILE* bf) {
    if (bf->counter == 0) {
        if (fillbuffer(bf))
            return -1;
    }
    int retVal = MSB_MASK & bf->buffer;
    bf->buffer <<= 1;
    bf->counter--;
    if (retVal) {
        return 1;
    }
    else {
        return 0;
    }
}

void flushbuffer(BITFILE* bf) {
    fwrite(&(bf->buffer), sizeof(bf->buffer), 1, bf->f);
    bf->buffer = 0;
    bf->counter = 0;
}

void flushbuffer_final(BITFILE* bf, int padding) {
    int number_bytes_to_write = (bf->counter / 8);
    if (bf->counter % 8) { // if we have a rest -> dangling bits, have to write the next byte too
        number_bytes_to_write++;
    }
    for (int i = 3; i >= (4 - number_bytes_to_write); i--) {
        unsigned int mask = 0xff;
        int out = (bf->buffer >> (8*i)) & mask;
        fwrite(&out, sizeof(char), 1, bf->f);
    }
    bf->buffer = 0;
    bf->counter = 0;
}

int fillbuffer(BITFILE* bf) {
    int bytes_remaining = NUMBER_BYTES_FILE - NUMBER_BYTES_READ;
    if (!(bytes_remaining)) {
        return 1;
    }

    if (bytes_remaining <= (BUFFER_SIZE_BYTES)) {
        if(!PADDING && bytes_remaining == 4) {
            goto regular_fillbuffer;
        }
        unsigned char temp[4] = {0};
        int read = fread(&temp, sizeof(char), 4, bf->f);
        NUMBER_BYTES_READ += read * sizeof(char);
            unsigned int temp_buffer = 0;
            for (int i = 0; i < read; i++) {
                temp_buffer <<= 8;
                temp_buffer |= temp[i];
            }
            temp_buffer <<= (8 * (BUFFER_SIZE_BYTES - read));
            bf->buffer = temp_buffer;
            bf->counter = BUFFER_SIZE - (((BUFFER_SIZE_BYTES - read) *8) + PADDING);
            if (NUMBER_BYTES_FILE < 4) { // first 3 bits already read when getting Padding
                bf->buffer <<= 3;
                bf->counter -= 3;
            } 
            return 0;
    } else {
        regular_fillbuffer:;
        int read = fread(&bf->buffer, sizeof(bf->buffer), 1, bf->f);
        NUMBER_BYTES_READ += read * BUFFER_SIZE_BYTES;
        bf->counter = read * BUFFER_SIZE;
        return 0;
    }
}

int getPadding(BITFILE* bf) {
    if (NUMBER_BYTES_FILE < 4) {
        unsigned char temp;
        int read = fread(&temp, sizeof(char), 1, bf->f);
        if (!read) {
            fprintf(stderr, "Couldn't read from file");
        }
        fseek(bf->f, 0L, SEEK_SET);
        temp &= 0xE0;
        int retVal = temp >> 5;
        return retVal;
    }
    else {
        int retVal = 0;
        for (int i = 0; i<3; i++) {
            retVal <<= 1;
            retVal |= getbit(bf);
        }
        return retVal;
    }
}

int checkSignature(BITFILE* bf) {
    unsigned int temp = 0;
    for (int i = 0; i < 5; i++ ) {
        temp <<= 1;
        temp |= getbit(bf);
    }
    return temp != SIGNATURE;
}

int emptyFile(FILE* file) {
    fseek (file, 0, SEEK_END);
    int size = ftell(file);
    fseek (file, 0, SEEK_SET);
    return size == 0 ? 1 : 0;
}

int regularFile(const char* path) {
    struct stat sb;
    stat(path, &sb);
       return S_ISREG(sb.st_mode);
}