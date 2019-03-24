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
builds codebook in current directory called HuffmanCodebook (TODO: check exactly where the codebook should end up)
**/
void buildcodebook(char* file_name){
	if(file_name==NULL){ pEXIT_ERROR("passed in NULL file_name into buildcodebook"); }

	//build Huffman Tree
		TreeNode* huffman_tree = huffmancoding( buildFrequencyAVL(file_name) );	//Note: AVLtree is freed in huffmancoding
			if(huffman_tree==NULL) return; //Note: huffmancoding method already outputs errors

	//Create File
		int codebook = open("HuffmanCodebook", O_WRONLY| O_CREAT, 0644); //creates a new file, overwrites old HuffmanCoding File if exists TODO: check where to add
			if(codebook < 0){ pRETURN_ERRORvoid("tried to open HuffmanCodebook"); }
	
	//FIND ENCODINGS AND WRITE TO CODEBOOK
		if ( write( codebook, "\\\n", 2) < 0 ) { pRETURN_ERRORvoid("write()"); } //writes starting line, and then checks if unsuccesful
		writeEncodings(codebook, huffman_tree, NULL); //finds and writes encodings of each token into file
		if ( write( codebook, "\n", 1) < 0 ) { pRETURN_ERRORvoid("write()"); } //writes terminating line, and then checks if unsuccesful
		
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
			if( write( codebook, (root->element)->encoding , strlen((root->element)->encoding) ) < 0 ) { pRETURN_ERRORvoid("write()"); } //write the encoding into codebook
			if( write( codebook, "\t" , 1) < 0 ) { pRETURN_ERRORvoid("write()"); }
			if( write( codebook, (root->element)->tok , strlen((root->element)->tok) ) < 0 ) { pRETURN_ERRORvoid("write()"); } //write the token into codebook
			if( write( codebook, "\n" , 1) < 0) { pRETURN_ERRORvoid("write()"); }
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

		AVLNode* freq_tree = NULL;
		int indOfWS; //index of whitespace

	//TOKENIZES fstr INTO A TOKEN AND A WHITESPACE TOKEN
		while( (indOfWS = strcspn(fstr_ptr , WHITESPACE_DELIM))!= 0 ){ //finds index of first instance of whitespace delim

			//Finding the token before the white_space
				char* tokB4 = (char*)malloc( indOfWS + 1 );
					if(tokB4 ==NULL){ pEXIT_ERROR("malloc"); }
					tokB4[indOfWS] = '\0'; //string terminator
					memcpy(tokB4, fstr_ptr , indOfWS);//copy (indOfWS) number of characters from fstr into tok

			//Finding the whitespace token
				char* tokWS = getStringRepOfWS(fstr_ptr[indOfWS]);
					if(tokWS==NULL){ free(fstr); free(fstr_ptr); free(tokB4); free(freq_tree); pRETURN_ERROR("did not enter in whitespace", NULL);}
			
			//Inserting tokens into Tree, frees token if duplicate
				if( insertOrUpdateAVL(&freq_tree, tokB4) == UPDATED ) //insert tokB4 into AVL, frees tokB4 if insertOrUpdateAVL() did an Update
					free(tokB4);
				if( insertOrUpdateAVL(&freq_tree, tokWS) == UPDATED ) //insert tokWS into AVL, frees tokWS if insertOrUpdateAVL() did an Update
					free(tokWS);

			//Update string pointer fstr
				if(strlen(fstr_ptr) <= indOfWS + 1 )
					break;
				fstr_ptr  += ( indOfWS + 1 ); //update pointer of fstr_ptr to after current tokens
		}

	free(fstr);
	return freq_tree;
}


/**
returns a unique string representation (dynamically allocated) for a char whitespace passed in
**/
static char* getStringRepOfWS( char c ){
	//String must be dynamically allocated to free indisciminantly later in BuildFrequencyAVL() without having to do strcmps
	char* ret = (char*)malloc(3);
	if(ret==NULL){ pEXIT_ERROR("malloc"); }
	ret[2]='\0';

	switch(c){
		case ' ': //space
			ret[0]='S';
			ret[1]= 'P';
			return ret;
		case '\t': //tab
			ret[0]='T';
			ret[1]= 'B';
			return ret;
		case '\n': //new line
			ret[0]='N';
			ret[1]= 'L';
			return ret;
		default:
			free(ret);
			pRETURN_ERROR("not a whitespace", NULL);
	}

	return NULL;
}


/**
Takes in a AVLTree of tokss and their frequencies in a file and performs huffman coding
returns a huffman tree to be used in encoding
FREES AVL TREE AFTER USE
**/
static TreeNode* huffmancoding(AVLNode* frequency_tree){
	if(frequency_tree==NULL){ pRETURN_ERROR("passed in NULL frequency tree", NULL);}

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

void compress( char* file_name ){ //TODO
	//tree to search up tokens quickly and get an encoding
	CodeNode* codebook_tree = buildCodebookTree( codebook_name, cmpByTokens);
	
	//TODO: compress file passed in 
		// use char* getCodeItem( CodeNode* root, char* key, cmpByTokens); to find encoding associated with token
		// can look at the tokenizer I made in buildFrequencyAVL() if you want to separate a string into a token and a whitespace (with a few modifications ofcourse)
	
	freeCodeTree( codebook_tree );
}




//DECOMPRESS methods////////////////////////////////////////////

void decompress( char* file_name ){ //TODO
	//tree to search up encodings quickly and get a token
	CodeNode* codebook_tree = buildCodebookTree( codebook_name, cmpByEncodings);
	
	
	//TODO: decompress file passed in:
		//will have to figure out your own tokenizer, maybe do a bit by bit comparison?
		//char* getCodeItem( CodeNode* root, char* key, cmpByEncodings); to find token associated with encoding
	
	freeCodeTree( codebook_tree );
}




//RECURSE methods////////////////////////////////////////////

/**
Runs the flag multiple times in all subdirectories of a given path
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
		if( type == isDIR ) //new_path is a directory
			recurse(new_path);
		else if ( type == isREG ) //new_path is a file
			runFlag(new_path);

		free(new_path);
	}

	closedir(curr_dir);
}




//MISC methods//////////////////////////////////////////////


/**
Runs a single flag operation on file_name given. (note: must be a regular file!)
Returns true if succesful, returns false if not.
**/
bool runFlag(char* file_name){ //TODO test once complete
	if(file_name==NULL){ pEXIT_ERROR("path_file NULL");}
		
	switch(flag){
		case 'b':
			buildcodebook( file_name );
			break;
		case 'c':
			compress ( file_name );
			break;
		case 'd':
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
			
			if( typeOfFile(s) == isUNDEF) { pEXIT_ERROR(""); } //checking if PATH/FILE exists , if not, exit
			
			//if arg is a HuffmanCodebook
			if( isHuffmanCodebook(s) ){
				if( codebook_name != NULL ){ pEXIT_ERROR("can only have one codebook"); } //codebook already initialized
				codebook_name = s;

			//if arg is a regular path/file
			}else{
				if(orig_pathfile!=NULL){ pEXIT_ERROR("can only have at most one file/path"); }  //orig_pathfile already initialized
				orig_pathfile = s;
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
	//INPUT CHECKS
		if(!inputCheck(argc, argv))
			return 0;
	
	//Running the respective flag operation
		if(isRecursive) //recursive
			recurse(orig_pathfile);
		else
			runFlag(orig_pathfile);
	
	return 0;
}
