#ifndef FILEC_H
#define FILEC_H

#include "structures.h"
#include "fileHelperMethods.h"


//METHOD SIGNAURES
	void buildcodebook( char* file_name );
	static void writeEncodings( int codebook, TreeNode* huffman_tree, char* encoding );
	static TreeNode* huffmancoding( AVLNode* frequencies );
	static AVLNode* buildFrequencyAVL( char* file_name );
	
	void compress( char* file_name );
	
	void decompress( char* file_name );
	
	void recurse( char* path );
	
	bool runFlag( char* path_file );

#endif
