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
@ret - struct WordFreq*
**/
struct WordFreq* createWordFreq(char* word, int frequency){
	struct WordFreq* ret = (struct WordFreq*)malloc(sizeof(struct WordFreq));
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
struct TreeNode* createTreeNode(struct WordFreq* element){
	struct TreeNode* ret = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	ret->element = element;
	ret->left = NULL;
	ret->right = NULL;
	return ret;
}


/**
merges two trees into one and returns the root (the combined frequency)
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
creates a minheap from an AVL Frequency Tree 
@params: node - AVL tree root with frequencies of each word. 
@ret : a minheap pointer to a minheap w/ an initialized heap array
**/
struct minheap* createMinheap(struct AVLNode* node){ 
	struct minheap* ret = (struct minheap*)malloc(sizeof(struct minheap));
	
	ret->length = sizeOfAVL(node);
	
	ret->heapArr = (struct WordFreq**)malloc( (ret->length) * sizeof(struct WordFreq*) ); //initializes heapArr to number of nodes in given AVL tree
	initializeMinheapArr(node, &(ret->heapArr), 0); //inserts each node of AVLtree into heapArr	
	heapify(ret);
	
	printf("%s\n",ret->heapArr[0]->word);
	return ret;
}


/**
Traverses through an AVL tree and inserts each WordFreq element into the heapArr
**/
int initializeMinheapArr(struct AVLNode* node, struct WordFreq***heapArr, int i){ //TODO:fix
	if(node==NULL || heapArr==NULL){
		return i;
	}
	
	*(heapArr[i++]) = node->element;
	
	if(node->left != NULL){
		i= initializeMinheapArr(node->left, heapArr, i);
	}else if(node->right != NULL){
		i = initializeMinheapArr(node->right, heapArr, i);
	}
	
	return i;
	
}


/**
takes initialized minheap array and heapifies it with the O(n) time algorithm
**/
void heapify(struct minheap* heap){ //TODO (I'll do this one - Mzhou)
}


/**
returns from top of the heap and then updates the heap (sifts up)
**/
struct WordFreq* getMin(struct minheap* heap){ //TODO
	return NULL;
}


int main(){
	struct AVLNode* root = createAVLNode("hello");
	root->left = createAVLNode("hellol");
	root->right = createAVLNode("hellor");
	
	struct minheap* heap = createMinheap(root);
	
	printf("sizeAVL: %d\n", sizeOfAVL(root));
	printf("sizeheaparr: %d\n", heap->length);
	
	
	return 0;
}


