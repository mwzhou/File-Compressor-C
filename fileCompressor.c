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

//GLOBALS
char flag ='\0';
bool isRecursive = false;
char* orig_pathfile = NULL;
char* codebook = NULL;

//TODO: (del note) DO NOT REMOVE BRACKETS ARROUND pEXIT_ERROR(txt); even if it looks ugly, it's actually two statements.
		//if you remove the brackets, it won't run correctly
		
		

//BUILD_CODEBOOK methods////////////////////////////////////////////

void buildcodebook(char* file_name){ //TODO
	if(file_name==NULL){
		PRINT_ERROR("passed in NULL file_name into buildcodebook"); return;
	}
	
	TreeNode* huffman_tree = huffmancoding( buildFrequencyAVL(file_name) );
	if(huffman_tree==NULL) return; //method already outputs errors
	
	//TODO: build codebook from tree
	
}



/**
goes through a file, tokenizes it, and gets the frequency of each token
@returns AVLTree* based on frquencies of each element
 returns NULL is file wasn't passed in, or there were no tokens
**/
AVLNode* buildFrequencyAVL(char* file_name){ 
	if(file_name == NULL ){
		PRINT_ERROR("cannot pass in NULL file_name into buildFrequencyAVL()"); return NULL;
	}
	
	//DECLARE VARIABLES
		char* s = readFile(file_name); //file read into a string
		char* s_ptr = s; //pointer to string for manipulation later
		if(s == NULL) //Note: readFile() already prints out errors
			return NULL; 
		
		AVLNode* freq_tree = NULL;
		int break_ind; //index of first WHITESPACE_DELIM
	
	//SPLITS string INTO TOKEN AND WHITESPACE
		while( (break_ind = strcspn(s_ptr , WHITESPACE_DELIM))!= 0 ){ //finds index of first instance of whitespace delim
		
			//Finding token before the white_space
				char* tokB4 = (char*)malloc( break_ind + 1 ); 
					if(tokB4 ==NULL){ pEXIT_ERROR("malloc"); }
					tokB4[break_ind] = '\0'; //string terminator
					memcpy(tokB4, s_ptr , break_ind);//copy (break_ind) number of characters from s into tok
			
			//Creating the whitespace token
				char* tokWS = getStringRep(s_ptr[break_ind]);
			
			//Inserting tokens into Tree, frees token if duplicate
				if( insertOrUpdateAVL(&freq_tree, tokB4) ) //insert tokB4, frees if duplicate
					free(tokB4);
				if( insertOrUpdateAVL(&freq_tree, tokWS) ) //insert tokWS, frees if duplicate
					free(tokWS);
			
			//Update string pointer s
				if(strlen(s_ptr) <= break_ind + 1 ) 
					break;
				s_ptr  = s_ptr + ( break_ind + 1 );
		}
		
	return freq_tree;
}


/**
Takes in a AVLTree of words and their frequencies in a file and performs huffman coding
returns a huffman tree to be used in encoding
**/
TreeNode* huffmancoding(AVLNode* frequencies){
	if(frequencies==NULL){
		return NULL;
	}

	MinHeap tokens = createMinHeap(frequencies);
	freeAVLTree(frequencies);
	Queue trees = {NULL,NULL};

	while( !(tokens.size==0 && hasSizeOne(&trees)) ){ //stops when heap is empty and only one element left in tree
		TreeNode* t1 = pickMinTree(&tokens, &trees);
		TreeNode* t2 = pickMinTree(&tokens, &trees);
		enqueue(&trees, mergeTrees(t1,t2));
	}

	free(tokens.heapArr);
	return dequeue(&trees);
}


/**
returns a unique string representation (dynamically allocated) for a char whitespace passed in
**/
char* getStringRep( char c ){
	//String must be dynamically allocated to free indisciminantly later (avoid strcmp)
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
			PRINT_ERROR("not a whitespace");
			return NULL;
	}
	
	return NULL;
}



//COMPRESS METHODS////////////////////////////////////////////

void compress( char* pathfile_name ){ //TODO: add params and return
}



//DECOMPRESS methods////////////////////////////////////////////

void decompress( char* pathfile_name ){ //TODO: add params and return
}



//RECURSIVE methods////////////////////////////////////////////

/**
Runs the flag multiple times in all subdirectories of a given path
**/
void Recursive(char* path){
	DIR* curr_dir = opendir(path);
	struct dirent* dp;

	if(curr_dir==NULL){ //if opendir() failed or if not a directory - return
		PRINT_ERROR("Did not pass in a path");perror(path);closedir(curr_dir);return;
	}


	//TRAVERSES THROUGH CURRENT DIRECTORY TO FIND ALL SUBDIRECTORIES
	while((dp = readdir(curr_dir))!=NULL){
		if(strcmp(dp->d_name, ".")==0 || strcmp(dp->d_name, "..")==0) //if current/parent directory(ignore)
			continue;

		//Checks type of dp and combines filepath (frees after entering the directory)
		char* new_path = combinedPath(path, dp->d_name);
		int type = typeStat(new_path);
	

		//new_path is a directory
		if(type == is_DIRnum){ 
			//TODO check for symbolic link as well?
			//printf("%s\t%s\n", dp -> d_name, new_path); TODO: del
			
			runFlag(new_path);
			Recursive(new_path);
			free(new_path);
		
		//new_path is not a directory
		}else{
			free(new_path);
		}
	}

	closedir(curr_dir);
}


/**
Combines a path string with a file string and returns the new path
returns a copy of the new path
**/
static char* combinedPath(char* path_name, char* file_name){
	if(path_name==NULL || file_name==NULL){
		PRINT_ERROR("cannot pass in NULL string into combinedPath()"); exit(EXIT_FAILURE);
	}
	
	//reallocate enough space
	char* ret = (char*)malloc( 2 + strlen(path_name) + strlen(file_name) );
	if(path_name==NULL){ pEXIT_ERROR("malloc"); }
	
	//copies and concatenates string
	strcpy(ret, path_name);
	strcat(ret, "/");
	strcat(ret, file_name);

	return ret;
}


////////////////////////////////////////////////


/**
builds AVL Tree based on the codebook given
if:
	flag == 'd': builds AVL Tree based on numerical ordering (bits)
	flag == 'c': builds an AVL Tree based on lexiographic ordering
@returns search tree if successful
@returns NULL if flag uninitialized or file is not a huffman_codebook
**/
AVLNode* buildHuffmanSearchTree(char* file_name){ //TODO: for compress and decompress
	if(flag!= 'c' || flag!= 'd'){ //if flag is not 'c' or 'd' 
		PRINT_ERROR("Can only build Huffman Search Tree for '-c' and '-d' flags");
		return NULL;
	}

	AVLNode* ret = NULL;
	return ret;
}


/**
checks if file matches huffman codebook format
**/
bool isHuffmanCodebook(char* file_name){ //TODO
	return false;

}


/**
Runs a single flag operation.
Returns true if succesful, returns false if not.
**/
bool runFlag(char* pathfile_name){ //TODO test once complete
	if(pathfile_name==NULL){
		PRINT_ERROR("path_file NULL");
		return false;
	}

	switch(flag){ 
		case 'b': 
			buildcodebook( pathfile_name );
			break;
		case 'c': 
			compress ( pathfile_name );
			break;
		case 'd': 
			decompress ( pathfile_name );
			break;
		default:
			PRINT_ERROR("flag must be 'b', 'c', or 'd'");
			exit(EXIT_FAILURE);
	}
	return true;
}


/**
checks inputs of items passed through terminal and initializes globals
**/
bool inputCheck(int argc, char** argv){
	if(argc<3 || argc>5){
		PRINT_ERROR("Must pass inbetween 2 to 4 arguments in addition to the executable"); exit(EXIT_FAILURE);
	}

	//LOOPING THROUGH EACH ARGUMENT (excluding executable)
	int i;
	for(i=1; i<argc; i++){
		char* s = argv[i];

		//CHECK IF FLAG
		if(strlen(s) == 2 && s[0]=='-'){

			//is a regular flag
			if( s[1]=='b'||s[1]=='c'||s[1]=='d' ){
				if(flag!='\0'){ //already came across a flag
					PRINT_ERROR("cannot have multiple flags"); exit(EXIT_FAILURE);
				}
				flag = s[1];

			//is a recursive flag
			}else if( s[1]=='R' ){
				if(isRecursive){ //already came across '-R' flag
					PRINT_ERROR("cannot have multiple '-R' flags"); exit(EXIT_FAILURE);
				}
				isRecursive = true;
			}


		//CHECK IF IS PATH/FILE and that it exists
		}else{
				if( typeStat(s) == -1){ //if file/path does not exist
					return false; //typeStat already prints out error messages
				}

				//isHuffmanCodebook
				if(isHuffmanCodebook(s)){
					if(codebook!=NULL){ //codebook already initialized
						PRINT_ERROR("can only have one codebook"); exit(EXIT_FAILURE);
					}
					codebook = s;

				//regular path/file
				}else{
					if(orig_pathfile!=NULL){ //orig_pathfile already initialized
						PRINT_ERROR("can only have at most one file/path"); exit(EXIT_FAILURE);
					}
					orig_pathfile = s;
				}
		}
	}

	
	//CHECK if all necessary globals have been initialized
	if(flag=='\0'){
		PRINT_ERROR("must specify a flag as an argument"); exit(EXIT_FAILURE);
	}
	if( orig_pathfile == NULL){
		PRINT_ERROR("must give in a path or a file as an argument"); exit(EXIT_FAILURE);
	}
	
	//CHECK IF ARGUMENTS MATCH FLAG
	if( (flag=='d'||flag=='c') && codebook==NULL ){ //-d and -c require a codebook to run
		PRINT_ERROR("must pass in huffman codebook for flag '-c' and '-b'"); exit(EXIT_FAILURE);
	}
	if(isRecursive && typeStat(orig_pathfile) != is_DIRnum){ //must be path if -R is called
		//TODO Symbolic link?
		PRINT_ERROR("flag '-R' requires a PATH to be passed in"); exit(EXIT_FAILURE);
	}

	return true;
}



int main(int argc, char** argv){
	//INPUT CHECKS
		if(!inputCheck(argc, argv))
			return 0;

	//Running the respective flag operation
		if(isRecursive){ //recursive
			Recursive(orig_pathfile);
		}else{
			runFlag(orig_pathfile);
		}

	return 0;
}
