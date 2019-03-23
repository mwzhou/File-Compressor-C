#ifndef FILEC_H
#define FILEC_H

#include "structures.h"
#include "fileHelperMethods.h"

	//DEFINE STATEMENTS
	#define WHITESPACE_DELIM " \t\n"


	//METHOD SIGNAURES
	void buildcodebook(char* file_name);
	static void writeEncodings( int codebook, TreeNode* huffman_tree, char* encoding);
	static TreeNode* huffmancoding(AVLNode* frequencies);
	static AVLNode* buildFrequencyAVL(char* file_name);
	static char* getNewEncoding( char* prev_encoding, bool wentLeft);
	static char* getStringRep(char c);
	
	void recurse(char* path);
	
	bool runFlag(char* path_file);

#endif
