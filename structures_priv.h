/**
these are private methods only meant to be accessed by structures.c
**/

#ifndef priv
#define	priv

#include "structures.h"	
	
//MACROS
	#define heightAVL(node) (((node)==NULL)? 0: (node)->height)
	
//METHOD SIGNATURES
	static AVLNode* insertOrUpdateAVLRec(AVLNode* root, char* tok, Operation* op_ptr);
	static AVLNode* BalanceAVL(AVLNode* root, int balance_factor, char* cmp_str, CMPMode mode);
	static void Case1Balance(AVLNode** root_ptr, bool isLeft);
	static void Case2Balance(AVLNode** root_ptr, bool isLeftRight);
	static int greaterHeight(AVLNode* parent);
	
	
	static CodeNode* createCodeNode(char* tok, char* encoding);
	static CodeNode* insertCodeTreeRec( CodeNode* root, char* tok, char* encoding, CMPMode mode);
	
	static QueueItem* createQueueItem(TreeNode* tree);	

	static int initializeMinHeapArr(AVLNode* tree, Token** heapArr, int i);
	static void heapify(MinHeap* heap);
	static void siftDown(Token** heapArr, int size, int ind);
	static void swap(Token** element1, Token** element2);
	
	static void printHeapRec(MinHeap* heap, int root, int space);
	static void printAVLTreeRec(AVLNode* root, int space);
	static void printTreeRec(TreeNode* root, int space);
	void printHeapArray(Token** arr, int size);

#endif
