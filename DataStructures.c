/**
DataStructures.c holds all the structures necessary to be used in FileCompression.c
**/
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h> 
#include "DataStructures.h"


//WORDFREQ methods/////////////////////////////////////////////////////////////////

/**
Initializes a node structure
@params - char* word, int frequency
@ret - WordFreq
**/
WordFreq createWordFreq(char* word, int frequency){
	WordFreq ret = (WordFreq)malloc(sizeof(struct WordFreq));
	ret->word = word;
	ret->frequency = frequency;
	return ret;
}



//AVLNode methods/////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes an AVLNode structure
@params - the word (the frequency is always 1 at first)
@ret - initialized struct AVLNode*.
**/
struct AVLNode* createAVLNode(char* word){
	struct AVLNode* ret = (struct AVLNode*)malloc(sizeof(struct AVLNode));
	ret->element = createWordFreq(word, 1);
	ret->left = NULL;
	ret->right = NULL;
	return ret;
}


/**
Searches through AVL tree. 
If found: Updates word's frquency
If not found: Creates a WordFreq element and inserts it into the tree.
MAINTAINS AVL properties of the tree (balances if necessary)
**/
void insertAndUpdate(struct AVLNode* root, char* word){ //TODO

}


/**
gets number of nodes in an AVL Frequency Tree
**/
int sizeOfAVL(struct AVLNode* root){
	if(root==NULL)
		return 0;
		
	return 1+sizeOfAVL(root->left)+sizeOfAVL(root->right);
}



//TREENODE methods/////////////////////////////////////////////////////////////////

/**
Initializes a TreeNode structure and returns a pointer
**/
struct TreeNode* createTreeNode(WordFreq element){
	struct TreeNode* ret = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	ret->element = element;
	ret->left = NULL;
	ret->right = NULL;
	return ret;
}


/**
merges two trees into one and returns the root (the combined frequency)
Note: the root->element->word=NULL (because if only represents a frequency!)
**/
struct TreeNode* mergeTrees(struct TreeNode* t1, struct TreeNode* t2){ //TODO
	return NULL;
}



//TREEQUEUE methods////////////////////////////////////////////////////////////////

/**
**/
struct TreeNode* dequeue(struct TreeQueue* head){ //TODO
	return NULL;
}


/**
**/
void enqueue(struct TreeQueue* head, struct TreeNode* node){ //TODO

}



//MINHEAP methods//////////////////////

/**
creates a MinHeap from an AVL Frequency Tree 
@params: tree - AVL tree root that keeps track of frequencies of each word 
@ret : a MinHeap pointer to a MinHeap w/ an initialized heap array
**/
struct MinHeap* createMinHeap(struct AVLNode* tree){ 
	struct MinHeap* ret = (struct MinHeap*)malloc(sizeof(struct MinHeap));
	
	ret->length = sizeOfAVL(tree);
	
	ret->heapArr = (WordFreq*)malloc( (ret->length) * sizeof(WordFreq) ); //initializes heapArr to number of nodes in given AVL tree
	initializeMinHeapArr(tree, ret->heapArr, 0); //inserts each node of AVLtree into heapArr	
	heapify(ret);
		
	return ret;
}


/**
Serializes an AVL tree into an array.
Traverses through an AVL tree and inserts each WordFreq element into the heapArr
i is the current index of heapArr
**/
int initializeMinHeapArr(struct AVLNode* node, WordFreq* heapArr, int i){ 
	if(node==NULL || heapArr==NULL)
		return i;
	
	if(node->left != NULL)
		i= initializeMinHeapArr(node->left, heapArr, i);
	heapArr[i++]= node->element;
	if(node->right != NULL)
		i = initializeMinHeapArr(node->right, heapArr, i);
	
	return i;
}


/**
takes initialized MinHeap array and heapifies it with the O(n) time algorithm
**/
void heapify(struct MinHeap* heap){ 
	int currind = (heap->length)/2 - 1;
	while(currind>=0){
		siftDown(heap->heapArr, heap->length, currind);
		currind--;
	}
}


/**
sifts down given index in a heapArray to mantain minheap structure
**/
void siftDown(WordFreq* heapArr, int length, int ind){ 
	if(heapArr==NULL||length<=ind){
		PRINT_ERROR("either heapArr uninitialized or faulty index/length passed");
		return;
	}
	
	while(ind < (length/2)){ //until ind is a leafnode
		int l = 2*ind + 1; //left node index
		int r = 2*ind + 2; //right node index
		
		if(r>=length){ //curr node has no right child
			swap(&heapArr[ind], &heapArr[l]);
			ind = l;
		}else{ //curr node has two children
			int minChildInd = (heapArr[l]->frequency <= heapArr[r]->frequency)? l:r; //index of smallest child
			
			if((heapArr[ind]->frequency)>(heapArr[minChildInd]->frequency)){
				swap(&heapArr[ind], &heapArr[minChildInd]);
				ind = minChildInd;
			}else{ //equal to both children
				return;
			}
		}
	}
}


/**
Swaps two WordFreq items given a pointer to each element
**/
void swap(WordFreq* element1, WordFreq* element2){
	WordFreq temp = *element1;
	*element1 = *element2;
	*element2 = temp;
}


/**
returns from top of the heap and then updates the heap (sifts up)
**/
WordFreq removeMin(struct MinHeap* heap){ //TODO
	WordFreq min= heap->heapArr[0];
	return min;
}



//PRINT methods/////////////////////////////////////////////////////////////


void printHeap(struct MinHeap* heap){
	printf("\nHEAP:\n");	
	int i;
	for(i=0; i<(heap->length); i++){
		int level = 1<<i;//current level
		
		int j = 0;
		for(j=0; j<level && (j+level)<=(heap->length); j++){
			int j_ind = j+level-1;
			PRINT_WORDFREQ(heap->heapArr[j_ind], "	");
		}
		printf("\n");
	}
}


void printHeapArray(WordFreq* arr, int length){
	int i;
	for(i=0; i<length; i++){
		PRINT_WORDFREQ(arr[i],"  ");
	}
	printf("\n");
}

///////////////////////////////////////////////////////////////


int main(){
	struct AVLNode* root = createAVLNode("hello");
	root->left = createAVLNode("hellol");
	root->right = createAVLNode("hellor");
	root->left->left = createAVLNode("nahh");
	//root->left->left->left = createAVLNode("wog");
	root->left->element->frequency = 0; //ERROR
	root->right->element->frequency = 3;
	root->left->left->element->frequency = 10;
	//root->left->left->left->element->frequency = 7;
	
	WordFreq* arr = (WordFreq*)malloc(sizeOfAVL(root)*sizeof(WordFreq));
	initializeMinHeapArr(root, arr, 0);
	//printHeapArray(arr,sizeOfAVL(root));
	
	
	struct MinHeap* heap = createMinHeap(root);
	printHeapArray(heap->heapArr,sizeOfAVL(root));
	printHeap(heap);
	
	return 0;
}


