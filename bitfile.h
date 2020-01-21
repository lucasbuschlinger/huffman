#ifndef BITFILE_H
#define BITFILE_H

typedef struct {
    unsigned int buffer;            
    unsigned int counter;           
    FILE* f;                      
} BITFILE;

BITFILE* bitOpen(FILE* f);        
BITFILE* bitOpen_decomp(FILE* f); 
void* bitClose(BITFILE* bf);      
int getbit(BITFILE* bf);     
void putbits(unsigned nbits, unsigned long word, BITFILE* bf);
void flushbuffer(BITFILE* bf);
void flushbuffer_final(BITFILE* bf, int padding);
int fillbuffer(BITFILE* bf);
int getPadding(BITFILE* bf);
int checkSignature(BITFILE* bf);
int emptyFile(FILE* file);
int regularFile(const char* path);

static int BUFFER_SIZE;
static int BUFFER_SIZE_BYTES;
static int MSB_MASK;
static int SIGNATURE;
long int NUMBER_BYTES_FILE;
long int NUMBER_BYTES_READ;
int PADDING;

#endif