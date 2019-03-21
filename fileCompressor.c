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


//DEFINE STATEMENTS
#define is_DIRnum 358410 //is directory
#define is_REGnum 12338726 //is regular file
#define is_LNKnum 23700582 //is symbolic link
#define WHITESPACE_DELIM " \t\n"

//GLOBALS
char flag ='\0';
bool isRecursive = false;
char* orig_pathfile = NULL;
char* codebook = NULL;



//BUILD_CODEBOOK methods////////////////////////////////////////////

void buildcodebook(){ //TODO: add params and return

}


/**
return unique string representation for a char whitespace
**/
char* getStringRep( char c ){
	switch(c){
		case ' ':
			return "SPACE_str";
		case '\t':
			return "TAB_str";
		case '\n':
			return "NEWLINE_str";
		default:
			PRINT_ERROR("not a whitespace");
			return NULL;
	}
	return NULL;
}


/**
goes through a file, tokenizes it, and gets the frequency of each token
@returns AVLTree* based on frquencies of each element
@returns NULL is file wasn't passed in, or there were no tokens
**/
AVLNode* getFrequencies(char* file_name){ //TODO: freeing item not allocated by malloc?
	char* s = readFile(file_name);
	if(s==NULL) return NULL;

	AVLNode* freq_tree = NULL;

/* TODO: sorry, got tired, will update later
	//int orig_len = strlen(s);
	int break_ind;
	int i = 0;
	//finds first space where first whitespace shows up
	while( (s[i] != '\0'){
		//Initializing and Declating tokens
		char* tok = malloc( break_ind + 1 ); //TODO check malloc null EVERYWHERE
			if(tok==NULL){}
			tok[break_ind] = '\0';
			memcpy(tok, s, break_ind); //copy string over
		char* tok2 = getStringRep(s[break_ind]);

		s =  strdup(s+break_ind); //UPDATES String - moves pointer number of bytes away
		printf("%s and %s and %s\n", tok, tok2, s);
		printf("%lu\n",strcspn(s, WHITESPACE_DELIM));
		i++;
	}
*/

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
picks minimum frequency from top of Heap or top of Queue, removes the min from the data structure
@returns: the tree of the minimum frequency
**/
static TreeNode* pickMinTree(MinHeap* heap, Queue* q){
	if((heap==NULL || heap->size==0) && q==NULL){
		PRINT_ERROR("Nothing to compare");
		return NULL;
	}

	if(heap==NULL || heap->size==0){
		return dequeue(q);
	}else if(q==NULL || (q->front==NULL&& q->end==NULL)){
		return createTreeNode( removeMin(heap) );
	}

	return (peekMinHeap(heap) < peekQueue(q))? createTreeNode( removeMin(heap) ) : dequeue(q);
}


//COMPRESS METHODS////////////////////////////////////////////

void compress(){ //TODO: add params and return
}



//DECOMPRESS methods////////////////////////////////////////////

void decompress(){ //TODO: add params and return
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

		//Checks type of dp
		char* new_path = combinedPath(path, dp->d_name);
		int type = typeStat(new_path);

		if(type ==-1){//error from calling typeStat
			if(strcmp(new_path, path)!=0) //if not original string (was malloced)
				free(new_path);
			continue;

		}else if(type == is_DIRnum){ //dp is a directory
			//TODO check for symbolic link as well?
			printf("%s\t%s\n", dp -> d_name, new_path); //TODO: del

			runFlag(new_path);
			Recursive(new_path);
			if(strcmp(new_path, path)!=0) //if not original string (was malloced)
				free(new_path);
		}
	}

	closedir(curr_dir);
}



////////////////////////////////////////////////

/**
Combines a path string with a file string and returns the new path
**/
static char* combinedPath(char* path_name, char* file_name){
	char* ret = (char*)malloc(2 + strlen(path_name) + strlen(file_name));

	//ret copies (path + "/" + file)
	strcpy(ret, path_name);
	strcat(ret, "/");
	strcat(ret, file_name);

	return ret;
}


/**
reads a file given a filename.
@returns: string of contents of file if successful
returns NULL if there's an error
**/
char* readFile(char* file_name){
	//VARIABLES
		int file, file_cpy;
		int file_len;
		int bytes_read;
		char* str_f;

	//INITIALIZING VARIABLES AND OPENING
		//Opening files
		file = open(file_name, O_RDONLY);
		file_cpy = open(file_name, O_RDONLY);
			 if( file < 0 || file_cpy < 0 ){
	 			PRINT_ERROR("error opening file");perror(file_name);return NULL;
	 		 }

		//finding length of filemake
		file_len = (int)lseek( file_cpy, 0, SEEK_END ); //gets file size in bytes by going to end og file_cpy
			if ( file_len < 0){
				PRINT_ERROR("error getting file length with lseek()");perror(file_name);return NULL;
			}
			close(file_cpy);

		//file string : to return
		str_f = (char*)calloc((file_len + 1), 1);


	//READING FILE
	  bytes_read = read(file, str_f, file_len);
			if(bytes_read < 0){
				PRINT_ERROR("error reading file");perror(file_name);return NULL;
			}
		str_f[bytes_read] = '\0'; //mark end of string

	close(file);
	return str_f;
}


/**
returns the type of the string given in
@params: char* name - file_name or path_name
@returns:
	DIR - directory
	REG - regular file
	LINK - link
	-1 - error
**/
int typeStat(char* pathfile_name){
	if(pathfile_name ==NULL){ //passed in NULL dirent or curr_dir
		PRINT_ERROR("passed in NULL path");return -1;
	}

	struct stat dpstat;
	if(stat( pathfile_name  , &dpstat) < 0){ //error calling lstat
		perror("lstat failed"); return -1;
	}

	//check if DIR, REG, or LINK, and returns the respective number (defined in macro)
	if(S_ISREG(dpstat.st_mode)) //directory or file
		return is_REGnum;
	else if(S_ISLNK(dpstat.st_mode))
		return is_LNKnum;
	else if(S_ISDIR(dpstat.st_mode))
		return is_DIRnum;
	else
		return -1;
}


/**
builds AVL Tree based on the codebook given
if:
	flag == 'd': builds AVL Tree based on numerical ordering (bits)
	flag == 'c': builds an AVL Tree based on lexiographic ordering
@returns search tree if successful
@returns NULL if flag uninitialized or file is not a huffman_codebook
**/
AVLNode* buildHuffmanSearchTree(char* file_name){ //TODO
	if(flag!= 'c' || flag!= 'd'){ //if flag is not 'c' or 'd' or hasn't been initialized yet
		return NULL;
	}

	AVLNode* ret = NULL;
	return ret;
}


/**
checks if file matches huffman codebook
**/
bool isHuffmanCodebook(char* file_name){ //TODO
	return false;

}


/**
Runs a single flag operation.
Returns true if succesful, returns false if not.
**/
bool runFlag(char* pathfile_name){ //TODO
	if(pathfile_name==NULL){
		PRINT_ERROR("path_file NULL");
		return false;
	}

	switch(flag){
		case 'b': //TODO
			break;
		case 'c': //TODO
			break;
		case 'd': //TODO
			break;
		default:
			PRINT_ERROR("first letter of flag must be 'b', 'c', or 'd'");
			return false;
	}
	return true;
}


/**
checks inputs of items passed through terminal and initializes globals
**/
bool inputCheck(int argc, char** argv){
	if(argc<3 || argc>5){
		PRINT_ERROR("Must pass inbetween 2 to 4 arguments in addition to the executable");return false;
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
					PRINT_ERROR("cannot have multiple flags");return false;
				}
				flag = s[1];

			//is a recursive flag
			}else if( s[1]=='R' ){
				if(isRecursive){ //already came across '-R' flag
					PRINT_ERROR("cannot have multiple '-R' flags");return false;
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
						PRINT_ERROR("can only have one codebook");return false;
					}
					codebook = s;

				//regular path/file
				}else{
					if(orig_pathfile!=NULL){ //orig_pathfile already initialized
						PRINT_ERROR("can only have at most one file/path");return false;
					}
					orig_pathfile = s;
				}
		}
	}


	//CHECK if all necessary globals have been initialized
	if(flag=='\0'){
		PRINT_ERROR("must specify a flag as an argument");return false;
	}
	if( orig_pathfile == NULL){
		PRINT_ERROR("must give in a path or a file as an argument");return false;
	}

	//make sure the number of arguments passed in matches the flag initialized
	if( (flag=='d'||flag=='c') && codebook==NULL ){
		PRINT_ERROR("must pass in huffman codebook for flag '-c' and '-b'");return false;
	}else if( flag=='b' && (codebook!=NULL|| typeStat(orig_pathfile) != is_REGnum) ){
		PRINT_ERROR("flag '-b' requires a FILE as input and no codebook");return false;
	}

	if(isRecursive && typeStat(orig_pathfile) != is_DIRnum){
		//TODO Symbolic link?
		PRINT_ERROR("flag '-R' requires a PATH to be passed in");return false;
	}

	return true;
}



int main(int argc, char** argv){
	//INPUT CHECKS
		if(!inputCheck(argc, argv))
			return 0;

///////////////////////////////////////////////////////
	 getFrequencies(orig_pathfile);

//////////////////////////////////////////////////////////

	//Running the respective flag operation
		if(isRecursive){ //recursive
			Recursive(orig_pathfile);
		}else{
			runFlag(orig_pathfile);
		}

	return 0;
}
