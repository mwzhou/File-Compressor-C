#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 
#include <dirent.h>
#include <sys/stat.h>
#include "fileCompressor.h"


//GLOBALS

#define is_DIRnum 35841
#define is_REGnum 1238726
#define is_LNKnum 237582

char flag ='\0';
bool isRecursive = false;
char* orig_pathfile = NULL;
char* codebook = NULL;



//BUILD_CODEBOOK methods////////////////////////////////////////////

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
void Recursive(char* path_file){
	DIR* curr_dir = opendir(path_file);
	struct dirent* dp;
		
	if(curr_dir==NULL){ //if invalid file, or opendir() failes
		perror("call to opendir() failed");
		closedir(curr_dir);
		return;
	}
		
	//goes through all subdirectories in current path	
	while((dp = readdir(curr_dir))!=NULL){
		if(strcmp(dp->d_name, ".")==0 || strcmp(dp->d_name, "..")==0) //if current/parent directory(ignore)
			continue;	


		int type = typePath(dp,curr_dir); //checks what type the current item is
		if(type ==-1){//error from typePath
			continue;
			
		}else if(type == is_DIRnum){ //dp is a directory
			//TODO check for symbollic link as well?
			char* new_pathfile = combinedPath(path_file, dp->d_name);
			printf("%s\t%s\n", dp -> d_name, new_pathfile); //TODO: del
			
			runFlag(new_pathfile);
			Recursive(new_pathfile);
		}
	}
							
	closedir(curr_dir);
}


/**
Combines a path with a file and returns the new path
**/
static char* combinedPath(char* path, char* file){
	char* ret = (char*)malloc(2 + strlen(path) + strlen(file));
	
	//ret copies path + "/" + file
	strcpy(ret, path);
	if(strcmp(path, orig_pathfile)!=0) //if not original path
		free(path);
	strcat(ret, "/");
	strcat(ret, file);
	
	return ret;
}




////////////////////////////////////////////////


/**
returns the type of the file 
@params: TODO
@returns:
	DIR - directory
	REG - regular file
	LINK - link
	-1 - error
**/
int typePath(struct dirent* dp, DIR* curr_dir){
	if(dp==NULL||curr_dir==NULL){ //passed in NULL dirent or curr_dir
		PRINT_ERROR("passed in null dirent or DIR*");
		return -1;
	}
		
		
	struct stat dpstat;
	if(fstatat(dirfd(curr_dir), dp->d_name, &dpstat, 0) < 0){
		perror(dp->d_name);
		return -1;
	}
	
	//check if DIR, REG, or LINK, and returns the respective number (defined in macro)
	if(S_ISDIR(dpstat.st_mode)) //directory or file
		return is_DIRnum;
	else if(S_ISREG(dpstat.st_mode))
		return is_REGnum;
	else if(S_ISLNK(dpstat.st_mode))
		return is_LNKnum;
	
	return -1;
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


bool initializeGlobals(int argc, char** argv){
	if(argc!=3 && argc!=4){
		PRINT_ERROR("Not the correct number of arguments");
		return 0; 
	} 
	
	//LOOPING THROUGH THE ARGUMENTS
	int i;
	for(i=0; i<argc; i++){
		char* s = argv[i];
		int len_s = strlen(s);
		
		if(len_s == 2 && s[0]=='-'){ //check if is a flag
			if(s[1]=='b'||s[1]=='c'||s[1]=='d'){ //regular flag
				if(flag!='\0'){ //already initialized
					PRINT_ERROR("cannot have multiple flags");
					return false;
				}
				
				flag = s[1];
				continue;
				
			}else if(s[1]=='R'){ //recursive flag
				if(isRecursive){
					PRINT_ERROR("cannot have multiple '-R' flags");
					return false;
				}	
				
				isRecursive = true;			
				continue;
			}
		}else{ //check if path/file exists
			//TODO
			//typePath(struct dirent* dp, DIR* curr_dir)
		}
			
	}
	
	return true;
}


int main(int argc, char** argv){
	//INPUT CHECKS

		
	//INITIALIZE GLOBALS
		orig_pathfile = "/ilab/users/mwz7/Desktop";
	//Running the respective flag operation
		if(isRecursive){ //recursive
			Recursive(orig_pathfile);
		}else{
			runFlag(orig_pathfile);
		}
		
	return 0;
}
