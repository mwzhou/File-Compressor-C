#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 
#include "fileCompressor.h"


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


void buildcodebook(){ //TODO: add params and return

}


void compress(){ //TODO: add params and return
}


void decompress(){ //TODO: add params and return
}


/**
Runs a single flag operation.
Returns true if succesful, returns false if not.
**/
bool runFlag(char flag, char* path_file, char* codebook){ //TODO
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


int main(int argc, char** argv){
	//INPUT CHECKS
		if(argc!=3 && argc!=4){
			PRINT_ERROR("Not the correct number of arguments");
			return 0; 
		} 
		
	//DECLARING VARIABLES
		char* str_flag = argv[1]; 
		int length_flag = strlen(str_flag);
		if(length_flag>2){
			PRINT_ERROR("Flag must be one to two characters");
			return 0; 		
		}
			
		char flag = str_flag[0];
		bool isRecursive = (length_flag==2 && str_flag[1]=='R')? true : false;	
		char* path_file = argv[2];
		char* codebook = (argc==4)? argv[3]:NULL; //if codebook was passed in as argument	
			
	//Running the respective flag operation

		if(isRecursive){ //recursive
			//TODO
		}else{
			runFlag(flag, path_file, codebook);
		}
		
	
	return 0;
}
