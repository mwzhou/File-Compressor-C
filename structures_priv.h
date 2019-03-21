/**
private methods only meant to be accessed by DataStructures.c
**/
#ifndef priv
#define	priv

#include "structures.h"	
	
	#define heightAVL(node) (((node)==NULL)? 0: (node)->height)
	
	static AVLNode* insertOrUpdateAVLRec(AVLNode* root, char* word, bool* updatedFreq);
	static AVLNode* BalanceAVL(AVLNode* root, int balance_factor, char* word);
	static void Case1Balance(AVLNode** root_ptr, bool isLeft);
	static void Case2Balance(AVLNode** root_ptr, bool isLeftRight);
	static int greaterHeight(AVLNode* parent);
	
	static QueueItem* createQueueItem(TreeNode* tree);	

	static int initializeMinHeapArr(AVLNode* tree, WordFreq** heapArr, int i);
	static void heapify(MinHeap* heap);
	static void siftDown(WordFreq** heapArr, int size, int ind);
	static void swap(WordFreq** element1, WordFreq** element2);
	
	static void printHeapRec(MinHeap* heap, int root, int space);
	static void printAVLTreeRec(AVLNode* root, int space);
	static void printTreeRec(TreeNode* root, int space);
	void printHeapArray(WordFreq** arr, int size);

#endif
