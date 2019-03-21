#ifndef FILE_HELP
#define FILE_HELP
#include"structures.h"

	//DEFINE STATEMENTS
	#define is_DIRnum 358410 //is directory
	#define is_REGnum 12338726 //is regular file
	#define is_LNKnum 23700582 //is symbolic link

	//METHOD SIGNATURES
	char* readFile(char* file_name);
	int typeStat(char* pathfile_name);

#endif
