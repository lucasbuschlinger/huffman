huffmann: huffmann.o bitfile.o compress.o decompress.o tree.o
	gcc -o huffmann  huffmann.o bitfile.o compress.o decompress.o tree.o
 
huff.o: huffmann.c bitfile.o compress.o decompress.o tree.o
	gcc -c huffmann.c

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
	rm huffmann
