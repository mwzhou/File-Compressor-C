/**
private methods only meant to be accessed by DataStructures.c
**/
#ifndef priv
#define	priv

#include "structures.h"	

	static int searchAVL(AVLNode* root, char* word);
	static AVLNode* insert(AVLNode* root, char* word)
	static AVLNode* balanceAVL(AVLNode* toBalance, int balanceFactor, char*);
	static int greater(AVLNode* parent);
	static int height(AVLNode* findFor);
	
	static QueueItem* createQueueItem(TreeNode* tree);	

	static int initializeMinHeapArr(AVLNode* tree, WordFreq** heapArr, int i);
	static void heapify(MinHeap* heap);
	static void siftDown(WordFreq** heapArr, int size, int ind);
	static void swap(WordFreq** element1, WordFreq** element2);
	
	static void printHeapRec(MinHeap heap, int root, int space);
	static void printTreeRec(TreeNode* root, int space);
	void printHeapArray(WordFreq** arr, int size);

#endif
