#ifndef FILEC_H
#define FILEC_H

#include "structures.h"
#include "fileHelperMethods.h"

//DEFINE MACROS
	#define WHITESPACE_DELIM " \t\n\\"
	#define WRITE_AND_CHECKv(file, buf, nbytes) do{  if( write(file, buf , nbytes) < 0 ) { pRETURN_ERRORvoid("write()"); } }while(0) //writes to file, if failed, prints out error and returns void

//METHOD SIGNAURES
	void buildcodebook( char* file_name );
	static void writeEncodings( int codebook, TreeNode* huffman_tree, char* encoding );
	static TreeNode* huffmancoding( AVLNode* frequencies );
	static AVLNode* buildFrequencyAVL( char* file_name );
	static char* getStringRepOfWS( char c );
	
	void compress( char* file_name );
	
	void decompress( char* file_name );
	
	void recurse( char* path );
	
	bool runFlag( char* path_file );

#endif
