all: fileCompressor

fileCompressor: fileCompressor.o fileHelperMethods.o structures.o
	gcc -lm -Wall -Werror -fsanitize=address fileCompressor.o fileHelperMethods.o structures.o -o fileCompressor
	
fileCompressor.o: fileCompressor.c fileCompressor.h structures.h structures_priv.h fileHelperMethods.h
	gcc -lm -Wall -Werror -fsanitize=address -g -c fileCompressor.c

fileHelperMethods.o: fileHelperMethods.c fileHelperMethods.h
	gcc -lm -Wall -Werror -fsanitize=address -g -c fileHelperMethods.c

structures.o: structures.c structures.h structures_priv.h fileHelperMethods.h
	gcc -lm -Wall -Werror -fsanitize=address -g -c structures.c
		
clean:
	rm -f  *.o fileCompressor
