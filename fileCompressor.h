#ifndef FILEC_H
#define FILEC_H

#include "structures.h"
#include "fileHelperMethods.h"

	//DEFINE STATEMENTS
	#define WHITESPACE_DELIM " \t\n"


	//METHOD SIGNAURES
	void buildcodebook(char* file_name);
	TreeNode* huffmancoding(AVLNode* frequencies);
	AVLNode* buildFrequencyAVL(char* file_name);
	char* getStringRep(char c);

	static char* combinedPath(char* path_name, char* file_name);
	
	bool runFlag(char* path_file);
	void Recursive(char* path_file);

#endif
