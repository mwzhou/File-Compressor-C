#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<sys/types.h>

#include "fileCompressor.h"

//GLOBALS: items passed through the terminal

	//flag passed in, will be initialized to flag
	char flag ='\0'; 
	
	//if'-R' flag passed in, will be initialized to true
	bool isRecursive = false; 
	
	//file/path name passed in, will be initialized to file/path name
	char* orig_pathfile = NULL;
	
	//file/path name passed in, will be initialized to file/path name
	char* codebook_name = NULL;
	
	
	
//TODO: (del note) DO NOT REMOVE BRACKETS ARROUND pEXIT_ERROR(txt); or pRETURN_ERROR(txt);
 		//even if it looks ugly, it's a whole do while loop defined in a macro. if you remove the brackets, it won't run correctly!!!
		//feel free to use these macros ( defined in fileHelperMethods.h ) to print out errors/return or exit
		
		//sorry if some lines are multi-commands, error checks take up so much space, so I put it all on one line.
		//if you wanna separate the lines feel free to do so (but with the brackets!)




//BUILD_CODEBOOK methods////////////////////////////////////////////

/**
builds codebook in current directory called HuffmanCodebook
**/
void buildcodebook(char* file_name){
	if(file_name==NULL){ pEXIT_ERROR("passed in NULL file_name into buildcodebook"); }

	//build Huffman Tree		
		TreeNode* huffman_tree = huffmancoding( buildFrequencyAVL(file_name) );	//Note: AVLtree is freed in huffmancoding
			if(huffman_tree==NULL) return; //Note: huffmancoding method already outputs errors	
		
	//Create File:
		//get path of huffmancodebook:
		char* cb_path = getNewFilePath(file_name, "HuffmanCodebook");
		
		//opening file:
		int codebook = open(cb_path, O_WRONLY| O_CREAT, 0644); //creates a new file, overwrites old HuffmanCoding File if exists 
			if(codebook < 0){ pRETURN_ERRORvoid("tried to open HuffmanCodebook"); }
			free(cb_path);
			
	//FIND ENCODINGS AND WRITE TO CODEBOOK
		WRITE_AND_CHECKv( codebook, "\\\n", 2); //writes starting line, and then checks if unsuccesful
		writeEncodings(codebook, huffman_tree, NULL); //finds and writes encodings of each token into file
		WRITE_AND_CHECKv( codebook, "\n", 1); //writes terminating line, and then checks if unsuccesful
		
	//FREEING and CLOSING
		close(codebook);
		freeTreeAndTok(huffman_tree); //frees all TreeNodes AND also frees the encoding and token of each node
}


/**
goes through the Huffman Tree passed in and changes each node's frequency into the encoding for that word.
Adds each NON-NULL encoding for a NON-NULL token into the codebook
**/
static void writeEncodings(int codebook, TreeNode* root, char* encoding){ //TODO: check if codebook has to be certain order
	if( root == NULL )
		return;
	
	//WRITE ROOT'S ENCODING TO THE CODEBOOK AND UPDATES ROOT
		//update root's element and swaps the frequency member for an encoding member so that it can be freed later in freeTreeAndTok()
		(root->element)->encoding = encoding;
		(root->element)->hasFrequency=false;
		
		//WRITE encoding and tok if not NULL into codebook in the format specified by Assignment. (Checks for write() errors after each write)
		if( (root->element)->tok!= NULL && encoding!=NULL){ 
			WRITE_AND_CHECKv( codebook, (root->element)->encoding , strlen((root->element)->encoding) );//write the encoding into codebook
			WRITE_AND_CHECKv( codebook, "\t" , 1 );		
			WRITE_AND_CHECKv( codebook, (root->element)->tok , strlen((root->element)->tok) );//write the token into codebook
			WRITE_AND_CHECKv( codebook, "\n" , 1 );		
		}
	
	//UPDATE new encoding by adding a 0 or a 1 to the current encoding, then RECURSE
	writeEncodings( codebook, root->left, appendCharToString(encoding, '0') ); 
	writeEncodings( codebook, root->right, appendCharToString(encoding, '1') );
}



/**
goes through a file, tokenizes it, and gets the frequency of each token
@returns AVLTree* based on frquencies of each element
 returns NULL is file wasn't passed in, or there were no tokens
**/
static AVLNode* buildFrequencyAVL(char* file_name){
	if(file_name == NULL ){ pRETURN_ERROR("cannot pass in NULL file_name into buildFrequencyAVL()", NULL); }
	
	//DECLARE VARIABLES
		char* fstr = readFile(file_name); //file read into a string
		char* fstr_ptr = fstr; //pointer to front of string for manipulation later
			if(fstr == NULL) return NULL; //Note: readFile() already prints out errors

		int indOfDELIM = 0; //index to keep track of the first instance of a delimiter in fstr_ptr
		
		AVLNode* freq_tree = NULL;
		
	//TOKENIZES fstr INTO A TOKEN AND A DELIM TOKEN - loops through fstr until there are no more delimiters	
		while( (indOfDELIM = strcspn(fstr_ptr , DELIMS) ) < strlen(fstr_ptr) ){ //updates index of delimiter and stops once there are no more delimiters 
		
			//TOKEN BEFORE DELIM:
				if(indOfDELIM!=0){ //if there is a token b4 the delimiter
					//finding the token
					char* tokB4 = (char*)malloc( indOfDELIM + 1 );
						if(tokB4 ==NULL){ pEXIT_ERROR("malloc"); }
						tokB4[indOfDELIM] = '\0'; //string terminator
						memcpy(tokB4, fstr_ptr , indOfDELIM);//copy (indOfDELIM) number of characters from fstr into tok
					
					//insert tokB4 into AVL, frees tokB4 if insertOrUpdateAVL() did an Update
					if( insertOrUpdateAVL(&freq_tree, tokB4) == UPDATED ) 
						free(tokB4);
				}
				
				
			//TOKEN OF DELIM:
				//finding the token
				char* tokDELIM = getStringRepOfDELIM(fstr_ptr[indOfDELIM]);
					if(tokDELIM==NULL){ free(fstr); free(freq_tree); pRETURN_ERROR("did not enter in delimiter", NULL); }
			
				//inserting
				insertOrUpdateAVL(&freq_tree, tokDELIM); //insert string (is constant string so don't need to free)
					
			
			//UPDATE fstr_ptr
				fstr_ptr  += ( indOfDELIM + 1 ); //update pointer of fstr_ptr to after current tokens
		}

	free(fstr);
	return freq_tree;
}



/**
Takes in a AVLTree of tokss and their frequencies in a file and performs huffman coding
returns a huffman tree to be used in encoding
FREES AVL TREE AFTER USE
**/
static TreeNode* huffmancoding(AVLNode* frequency_tree){
	if(frequency_tree==NULL){ pRETURN_ERROR("passed in NULL frequency tree", NULL);}
	
	if( frequency_tree->left==NULL && frequency_tree->right==NULL ){ //if there is only one token: create a tree and return it
		TreeNode* ret = createTreeNode(  createTokenInt(NULL, frequency_tree->element->frequency) );
		ret->left = createTreeNode(  frequency_tree->element );
		free(frequency_tree);
		return ret;
	}
	
	//Variables
	MinHeap tokens = buildMinHeap(frequency_tree);
		freeAVLTree(frequency_tree); //do not need frequencies anymore, free AVLNodes (not the element though)
	Queue trees = {NULL,NULL};

	
	//continuously merge the two min elements from the heap and queue until we get one merged huffman tree
	//takes the smaller item in the heap/queue and merges the two minimums, then enqueues the tree onto the queue
	while( !(tokens.size==0 && hasSizeOne(&trees)) ){ //stops when heap is empty and only one element left in tree
		TreeNode* t1 = pickMinTreeHuffman(&tokens, &trees);
		TreeNode* t2 = pickMinTreeHuffman(&tokens, &trees);
		enqueue(&trees, mergeTrees(t1,t2));
	}

	free(tokens.heapArr);
	return dequeue(&trees);
}




//COMPRESS METHODS////////////////////////////////////////////

/**
compresses file_name given and writes <file_name>.hcz into the same directory
Note: checks on file_name have already been done in inputCheck() and runFlag()
**/
void compress( char* file_name ){ //TODO
	//tree to search up tokens quickly and get an encoding
		CodeNode* codebook_tree = buildCodebookTree( codebook_name, cmpByTokens);
	
	//file contents of fname read into fstr (readFile is a fileHelperMethods.c method) 
		char* fstr = readFile(file_name);
		
		char* fcompr_name = "TODO: replace with actual name"; //TODO: name of compressed file WITHOUT altering the original string
		char* fcompr_path_name = getNewFilePath(file_name, fcompr_name); //full path of where fcompr file would open.

	//compressed file to write to:
		int fcompr = open(fcompr_path_name , O_WRONLY| O_CREAT, 0644); //creates a new file called "fcomp_name" in directory, overwrites old file if exists
			if(fcompr < 0){ pRETURN_ERRORvoid("tried to open()"); }
			free(fcompr_path_name);
	
	
	
	//TOKENIZE AND WRITE:
		//TODO
		
		
	/*TODO: compress file passed in 
		-find a way to get fcomp_name
		-use char* getCodeItem( CodeNode* root, char* key, cmpByTokens); (in structures.c) to find encoding associated with token
		-REMEMBER when you READ in a delimiter token like, you need to search up it's string representation i.e '\t' becomes '\\t' in the codebook
		use char* getStringRepOfDELIM( char c ); in [fileHelperMethods.h] to find the string representation of a delimiter token
		-can look at the tokenizer I made in buildFrequencyAVL() (in the same c file) 
		 if you want to separate a string into a token and a delimiter (with a few modifications ofcourse)	
		-NOTE: in my tokenizer all tokens are malloced, so free them after each use when you don't need them anymore!
		-can use my write macros in fileHelperMethods.h
	*/
	
	
	free(fstr);
	close(fcompr);
	freeCodeTreeAndTok( codebook_tree );
}




//DECOMPRESS methods////////////////////////////////////////////

/**
decompresses given file_name.hcz given and writes decompressed file_name into the same directory
Note: checks on file_name have already been done in inputCheck() and runFlag()
**/
void decompress( char* file_name ){ //TODO
	//tree to search up encodings quickly and get a token
	CodeNode* codebook_tree = buildCodebookTree( codebook_name, cmpByEncodings);
	
	//file contents of file_name read into fstr_hcz (readFile is a fileHelperMethods.c method) TODO: free
	char* fstr_hcz = readFile(file_name); 
	
	//decompressed file name:
	char* fdec_name = "<file_name> - .hcz"; //TODO: find a way to get rid of the .hcz extension WITHOUT altering the original string
	char* fdec_path_name = getNewFilePath(file_name, fdec_name ); //full path of where fdecompr file would open.
		
	//decompressed file to write to:
	int fdecompr = open(fdec_path_name , O_WRONLY| O_CREAT, 0644); //creates a new file in directory, overwrites old file if exists
		if(fdecompr < 0){ pRETURN_ERRORvoid("tried to open()"); }
		free(fdec_path_name);
		
	
	/*TODO: decompress file passed in:
		-find a way to get fdec_name
		-go through fstr_hcz and try to extract each encoding
		-USE char* getCodeItem( CodeNode* root, char* key, cmpByEncodings); to find token associated with encoding
		-might be able to use [char* appendCharToString( char* prev_str , char add_c)] (in fileHelperMethods.c) but 
		 NOTE that I don't free the previous string in order to use it recusively in buildcodebook(), you'll have to free the string
		-REMEMBER TO REPLACE THE STRING REPRESENTATION OF A DELIM WITH THE ACTUAL CHAR i.e. "\\t" with '\t'
			can use the mehod: bool isDELIMStr(char* s); in [fileHelperMethods.h]
	*/
	
	
	//Frees
	free(fstr_hcz);
	close(fdecompr);
	freeCodeTreeAndTok( codebook_tree );
}




//RECURSE methods////////////////////////////////////////////

/**
Runs the flag multiple times in all subdirectories of a given path. Checks on input parameter path have already been done in inputCheck;
**/
void recurse(char* path){
	//DIRECTORY VARIABLES
	DIR* curr_dir = opendir(path);
		if(curr_dir==NULL){ pEXIT_ERROR("opendir()"); } 
		
	struct dirent* dp;

	//TRAVERSES THROUGH CURRENT DIRECTORY TO FIND ALL SUBDIRECTORIES //TODO check for symbolic link as well?
	while((dp = readdir(curr_dir))!=NULL){
		if(strcmp(dp->d_name, ".")==0 || strcmp(dp->d_name, "..")==0) //if current/parent directory(ignore) TODO
			continue;
		
		//Checks type of dp and combines filepath (frees after entering the directory)
		char* new_path = combinedPath(path, dp->d_name);
		FileType type = typeOfFile(new_path); //file type of new_path
		
		//RECURSE if Sub-Directory, RUN-FLAG, if not
		if( type == isDIR ){ //new_path is a directory
			recurse(new_path);
		}else if ( type == isREG ){ //new_path is a regular file 
			runFlag(new_path); //Note: flag only runs if the file meets the requirment for each respective flag in runFlag()
		}
		
		free(new_path);
	}

	closedir(curr_dir);
}




//MISC methods//////////////////////////////////////////////


/**
Runs a single flag operation on file_name given. (note: must be a regular file!)
Returns true if succesful, returns false if not.
**/
bool runFlag(char* file_name){ 
	//Error Checks
	if(file_name==NULL){ pEXIT_ERROR("path_file NULL");}
	if( typeOfFile(file_name)!= isREG ){ pRETURN_ERROR("must input a REG FILE into runFlag()", false); }
	if ( sizeOfFile(file_name)== 0){ printf("Empty File: %s\n", file_name); return false; }
	
	switch(flag){
		case 'b':
			if( strstr(file_name, "HuffmanCodebook") == NULL ) //if file sent in is not a HuffmanCodebook creates by b
				buildcodebook( file_name );
			break;
		case 'c':
			if( !endsWithHCZ(file_name) ) //if the file does not end with hcz
				compress ( file_name );
			break;
		case 'd':
			if( endsWithHCZ(file_name) ) //if the file ends with hcz
				decompress ( file_name );
			break;
		default:
			pEXIT_ERROR("flag must be 'b', 'c', or 'd'");
	}
	
	return true;
}


/**
checks inputs of items passed through terminal and initializes globals
exits program if arguments do not match the requirements
**/
bool inputCheck(int argc, char** argv){
	if(argc<3 || argc>5){ pEXIT_ERROR("Must pass inbetween 2 to 4 arguments in addition to the executable"); }

	//LOOPING THROUGH EACH ARGUMENT (excluding the first argument: the executable)
	int i;
	for(i=1; i<argc; i++){
		char* s = argv[i];
		
		//CHECK IF FLAG
		if(strlen(s) == 2 && s[0]=='-'){
			
			//if arg is a regular flag
			if( s[1]=='b'||s[1]=='c'||s[1]=='d' ){
				if(flag!='\0'){ pEXIT_ERROR("cannot have multiple flags"); } //already came across a flag
				flag = s[1];
			//if ard is a recursive flag
			}else if( s[1]=='R' ){
				if(isRecursive){ pEXIT_ERROR("cannot have multiple '-R' flags"); } //already came across '-R' flag
				isRecursive = true;
			}

		//CHECK IF IS PATH/FILE and that it exists
		}else{
			
			if( typeOfFile(s) == isUNDEF ) { pEXIT_ERROR(""); } //checking if PATH/FILE exists , if not, exit
			
			//if arg is a HuffmanCodebook
			if( isHuffmanCodebook(s) ){
				if( codebook_name != NULL ){ pEXIT_ERROR("can only have one codebook"); } //codebook already initialized
				codebook_name = s;

			//if arg is a regular path/file
			}else{
				if(orig_pathfile!=NULL){ pEXIT_ERROR("can only have at most one file/path"); }  //orig_pathfile already initialized
				orig_pathfile = realpath(s, NULL); //gets real path
			}
			
		}
		
	}
	
	
	//CHECK if all necessary globals have been initialized
		if(flag =='\0'){ pEXIT_ERROR("must specify a flag as an argument"); }
		if( orig_pathfile == NULL ){ pEXIT_ERROR("must give in a path or a file as an argument"); }
	
	//CHECK IF ARGUMENTS MATCH FLAG
		if( (flag=='d'||flag=='c') && codebook_name==NULL ){ pEXIT_ERROR("must pass in huffman codebook for flags '-c' and '-b'"); } //-d and -c flags, but codebook not initialized
	
		if(isRecursive && typeOfFile(orig_pathfile) != isDIR ){  pEXIT_ERROR("flag '-R' requires a PATH to be passed in"); } //'-R' flag called but path not handed in
		else if( !isRecursive && typeOfFile(orig_pathfile) != isREG ){ pEXIT_ERROR ( "must pass in a REGULAR FILE if not calling flag '-R'"); }//'-R' flag not called, but file is not a regular file
		
	return true;
}


int main(int argc, char** argv){
	//CHECKS ARGUMENTS PASSED IN THROUGH TERMINAL
		if(!inputCheck(argc, argv))
			return 0;
	
	//Running the respective flag operation
		if(isRecursive) //recursive
			recurse( orig_pathfile);
		else
			runFlag(orig_pathfile);
	
	return 0;
}
