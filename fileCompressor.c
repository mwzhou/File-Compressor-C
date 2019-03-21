#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fileCompressor.h"


//GLOBALS

#define is_DIRnum 35841
#define is_REGnum 12338726
#define is_LNKnum 23700582

char flag ='\0';
bool isRecursive = false;
char* orig_pathfile = NULL;
char* codebook = NULL;



//BUILD_CODEBOOK methods////////////////////////////////////////////

void buildcodebook(){ //TODO: add params and return

}


/**
goes through a file, tokenizes it, and gets the frequency of each token
@returns AVLTree* based on frquencies of each element
@returns NULL is file wasn't passed in, or there were no tokens
**/
AVLNode* getFrequencies(){ //TODO
	if(orig_pathfile == NULL || typeStat(orig_pathfile) != is_REGnum){
			PRINT_ERROR("must pass in a file when using '-b' flag");
			return NULL;
	}

	int file;
	if( (file = open(orig_pathfile, O_RDONLY , 0)) < 0 ){
		PRINT_ERROR("error opening file");
		perror(orig_pathfile);
		return NULL;
	}

	return NULL;
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

void compress(TreeNode* huffman_tree){ //TODO: add params and return
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
		PRINT_ERROR("Did not pass in a path");
		perror(path);
		closedir(curr_dir);
		return;
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


/**
Combines a path string with a file string and returns the new path
**/
static char* combinedPath(char* path, char* file){
	char* ret = (char*)malloc(2 + strlen(path) + strlen(file));

	//ret copies (path + "/" + file)
	strcpy(ret, path);
	strcat(ret, "/");
	strcat(ret, file);

	return ret;
}




////////////////////////////////////////////////


/**
returns the type of the string given in
@params: char* name - file_name or path_name
@returns:
	DIR - directory
	REG - regular file
	LINK - link
	-1 - error
**/
int typeStat(char* filepath_name){
	if(filepath_name ==NULL){ //passed in NULL dirent or curr_dir
		PRINT_ERROR("passed in NULL path");
		return -1;
	}

	struct stat dpstat;
	if(stat( filepath_name  , &dpstat) < 0){ //error calling lstat
		perror("lstat failed");
		return -1;
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
bool runFlag(char* path_file){ //TODO
	if(path_file==NULL){
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
		PRINT_ERROR("Must pass inbetween 2 to 4 arguments in addition to the executable");
		return false;
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
					PRINT_ERROR("cannot have multiple flags");
					return false;
				}
				flag = s[1];

			//is a recursive flag
			}else if( s[1]=='R' ){
				if(isRecursive){ //already came across '-R' flag
					PRINT_ERROR("cannot have multiple '-R' flags");
					return false;
				}
				isRecursive = true;
			}


		//CHECK IF IS PATH/FILE and that it exists
		}else{
				if( typeStat(s) == -1){ //if file/path does not exist
					return false;
				}

				//isHuffmanCodebook
				if(isHuffmanCodebook(s)){
					if(codebook!=NULL){ //codebook already initialized
						PRINT_ERROR("can only have one codebook");
						return false;
					}
					codebook = s;

				//regular path/file
				}else{
					if(orig_pathfile!=NULL){ //orig_pathfile already initialized
						PRINT_ERROR("can only have at most one file/path");
						return false;
					}
					orig_pathfile = s;
				}
		}
	}


	//CHECK if all necessary globals have been initialized
	if(flag=='\0'){
		PRINT_ERROR("must specify a flag as an argument");
		return false;
	}
	if( orig_pathfile == NULL){
		PRINT_ERROR("must give in a path or a file as an argument");
		return false;
	}

	//make sure the number of arguments passed in matches the flag initialized
	if( (flag=='d'||flag=='c') && codebook==NULL ){
		PRINT_ERROR("must pass in huffman codebook for flag '-c' and '-b'");
		return false;
	}else if( flag=='b' && codebook!=NULL ){
		PRINT_ERROR("passed in too many arguments for '-b', codebook unnecessary");
		return false;
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
