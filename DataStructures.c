/**
DataStructures.c holds all the structures necessary to be used in FileCompression.c
**/
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h> 
#include "DataStructures.h"



///////////////////////////////////////////////////////////////////////////////////////////////

/**
A search structure to keep track of frequencies of a word in a file. 
Ordered lexiographically by each word
**/
struct AVLFreqNode{
	char* word;
	int frequency;

	struct AVLFreqNode* left;
	struct AVLFreqNode* right;
};


/**
Initializes an AVLFreqNode structure
@params - the word (the frequency is always 1 at first)
@ret - initialized struct AVLFreqNode*.
**/
struct AVLFreqNode* createAVLNode(char* word){
	struct AVLFreqNode* ret = (struct AVLFreqNode*)malloc(sizeof(struct AVLFreqNode));
	ret->word = word;
	ret->frequency=1;
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
void insertAndUpdate(struct AVLFreqNode* root, char* word){ //TODO

}


///////////////////////////////////////////////////////////////////

/**
structure to associate a word with a frequency.
**/
struct WordFreq{
	char* word;
	int frequency;
};


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

///////////////////////////////////////////////////////////////////

struct TreeNode{
	struct WordFreq* element;
	//Note: If element->word==NULL, it represents an combined frequency of the left and right subtrees
	
	struct TreeNode* left;
	struct TreeNode* right;
};


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


///////////////////////////////////////////////////////////////////

struct TreeQueue{
	struct TreeNode* root;
	struct TreeQueue* next;
};


/**
**/
struct TreeNode* dequeue(struct TreeQueue* head){ //TODO
	return NULL;
}


/**
**/
void enqueue(struct TreeQueue* head, struct TreeNode* node){ //TODO

}


//////////////////////////////////////////////////////////////////////////////////////////////

/**
minheap to keep track of Words and their Frequencies.
Ordered based on the Frequencies of elements.
**/
struct minheap{
	struct WordFreq** heapArray; //array of WordFreq pointers
}; 


/**
creates a minheap from an AVL Frequency Tree 
@params: root - AVL tree with frequencies of each word. size - number of distinct words
@ret : a minheap pointer to a minheap w/ an initialized heapArray
**/
struct minheap* createMinHeap(struct AVLFreqNode* root, int size){ //TODO
	//do an inorder traversal of root, then call heapify
	return NULL;
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


