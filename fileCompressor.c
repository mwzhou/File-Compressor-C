#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 
#include <dirent.h>
#include <sys/stat.h>
#include "fileCompressor.h"


//GLOBALS

#define is_DIRnum 35841
#define is_REGnum 12338726
#define is_LNKnum 23700582

char flag ='\0';
bool isRecursive = false;
char* files [2] = {NULL, NULL};



//BUILD_CODEBOOK methods////////////////////////////////////////////////////////////////////////////////////////

void buildcodebook(){ //TODO: add params and return

}


/**
goes through a file, tokenizes it, and gets the frequency of each token
**/
AVLNode* getFrequencies(FILE* FP){ //TODO
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


//COMPRESS METHODS////////////////////////////////////////////////////////////////////////////////////////

void compress(TreeNode* huffman_tree){ //TODO: add params and return
}



//DECOMPRESS methods////////////////////////////////////////////////////////////////////////////////////////

void decompress(){ //TODO: add params and return
}



//RECURSIVE methods////////////////////////////////////////////////////////////////////////////////////////

/**
Runs the flag multiple times in all subdirectories of a given path
**/
void Recursive(char* path){
	DIR* curr_dir = opendir(path);
	struct dirent* dp;
	
	if(curr_dir==NULL){ //if opendir() failed or if not a directory - return
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
		int type = typeDirent(new_path);
		
		if(type ==-1){//error from calling typeDirent
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
returns the type of the dirent given in
@params: dirent*dp and curr_dir to check if valid
@returns:
	DIR - directory
	REG - regular file
	LINK - link
	-1 - error
**/
int typeDirent(char* path){
	if(path==NULL){ //passed in NULL dirent or curr_dir
		PRINT_ERROR("passed in NULL path");
		return -1;
	}
			
	struct stat dpstat;
	if(stat( path , &dpstat) < 0){ //error calling lstat
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
checks if file meets huffman codebook properties
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
		
		//Initializing the directory and dirent structs - also checking if valid
			DIR* curr_dir = opendir(argv[i]);
			if(curr_dir==NULL){
				perror("invalid argument passed, should be an existing file/directory");
				return false;
			}
			struct dirent* dp = readdir(curr_dir);
			if(dp==NULL){
				perror("error calling readdir()");
				closedir(curr_dir); 
				return false;
			}
		
	
		//checking type of file, if it's not a directory, file, or symb_link, return false
			if( typeDirent(argv[i]) == -1){
				closedir(curr_dir); 
				return false;
			}
				
				
		//File is valid - Initializing global files[] array
			if(files[0]==NULL)
				files[0] = argv[i];
			else if(files[1]==NULL)
				files[1] = argv[i];
			else{ //already initialized
				PRINT_ERROR("more than two files/paths passed in");
				closedir(curr_dir);
				return false;
			}
				
			closedir(curr_dir);
		}	
	}
	
	
	//check if all necessary globals have been initialized
	if(flag=='\0'){
		PRINT_ERROR("must specify a flag as an argument"); 
		return false;
	}
	if( files[0] == NULL){
		PRINT_ERROR("must give in a path or a file as an argument"); 
		return false;
	}
	//TODO: 
	//make sure files[0]=path/file and files[1]=huffman_codebook. 
	//Then make sure the flag matches number of arguments passed in
	
	return true;
}


int main(int argc, char** argv){
	//INPUT CHECKS
		if(!inputCheck(argc, argv))
			return 0;
					
	//Running the respective flag operation
		if(isRecursive){ //recursive
			Recursive(files[0]);
		}else{
			runFlag(files[0]);
		}
		
	return 0;
}
