#ifndef FILEC_H
#define FILEC_H

#include "structures.h"
#include "fileHelperMethods.h"


//METHOD SIGNAURES
	void buildcodebook( char* file_name, AVLNode* freq_tree );
	static void writeEncodings( int codebook, TreeNode* huffman_tree, char* encoding );
	static TreeNode* huffmancoding( AVLNode* frequencies );
	static void buildFrequencyAVL( char* file_name, AVLNode** freq_tree_ptr );
	
	void compress( char* file_name, CodeNode* codebook_tree );
	
	void decompress( char* file_name, CodeNode* codebook_tree );
	
	void recurse( char* path );
	
	void runFlag( char* path_file);
	
	void inputCheck(int argc, char** argv);

#endif
