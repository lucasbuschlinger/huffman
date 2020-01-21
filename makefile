huffman: huffman.o bitfile.o compress.o decompress.o tree.o
	gcc -o huffman huffman.o bitfile.o compress.o decompress.o tree.o
 
huffman.o: huffman.c bitfile.o compress.o decompress.o tree.o
	gcc -c huffman.c

bitfile.o: bitfile.c bitfile.h
	gcc -c bitfile.c

compress.o: compress.c compress.h
	gcc -c compress.c

decompress.o: decompress.c decompress.h
	gcc -c decompress.c

tree.o: tree.c tree.h
	gcc -c tree.c

clean:
	rm *.o
	rm huffman
