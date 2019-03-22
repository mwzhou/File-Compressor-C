#ifndef FILE_HELP
#define FILE_HELP
#include"structures.h"

	//enum to differentiate between FileTypes
	typedef enum{ isDIR, isREG, isLNK, isUNDEF }FileType;
	
	//METHOD SIGNATURES
	char* readFile(char* file_name);
	FileType typeOfFile(char* file_name);

#endif
