all: fileCompressor

fileCompressor: fileCompressor.o structures.o
	gcc -lm -Wall -Werror -fsanitize=address fileCompressor.o structures.o -o fileCompressor
	
fileCompressor.o: fileCompressor.c fileCompressor.h structures.h
	gcc -lm -Wall -Werror -fsanitize=address -g -c fileCompressor.c

structures.o: structures.c structures.h structures_priv.h
	gcc -lm -Wall -Werror -fsanitize=address -g -c structures.c
		
clean:
	rm -f  *.o fileCompressor
