#ifndef FILEC_H
#define FILEC_H

#include "structures.h"
#include "fileHelperMethods.h"

	//DEFINE STATEMENTS
	#define WHITESPACE_DELIM " \t\n"


	//METHOD SIGNAURES
	void buildcodebook(char* file_name);
	void buildcodebookRec(char* file_name, TreeNode* huffman_tree, char* encoding);
	static TreeNode* huffmancoding(AVLNode* frequencies);
	static AVLNode* buildFrequencyAVL(char* file_name);
	static char* getStringRep(char c);

	static char* combinedPath(char* path_name, char* file_name);
	
	bool runFlag(char* path_file);
	void recurse(char* path);

#endif
