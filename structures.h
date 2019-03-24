#ifndef DS_H
#define DS_H
#include "fileHelperMethods.h"

//DEFINE STATEMENTS
	#define printCodeTree(root) printAVLTree(root)
	#define freeCodeTree(root) freeAVLTree(root)
	
//ENUMS	
	//Comparison Mode to be used in AVLNode and CodeNode methods
	typedef enum{ cmpByTokens, cmpByEncodings }CMPMode;
	
	//Operation returned by insertOrUpdateAVL()
	typedef enum{ UPDATED, INSERTED }Operation;

//STRUCTS
	
	/**
	structure to associate a word with a frequency OR an encoding
	**/
	typedef struct Token{
		char* tok;
		bool hasFrequency; //boolean to distinguish whether it has a frequency or encoding
		union{ //Token can either have a frequency associated with the tok or a byte_encoding associated with the token
			int frequency;
			char* encoding;
		};
	}Token;


	/**
	structure to combine frequencies of each Token element for huffman coding
	**/
	typedef struct TreeNode{
		Token* element;
		//Note: If element->word==NULL, it represents an combined frequency of the left and right subtrees

		struct TreeNode* left;
		struct TreeNode* right;
	}TreeNode;


	/**
	structure to order tokens in a file lexiographically in a balanced tree and keep track of its frequencies
	**/
	typedef struct AVLNode{
		Token* element;
		int height;
		struct AVLNode* left;
		struct AVLNode* right;
	}AVLNode;


	/**
	Note: CodeNode is the same type as an AVL Node (different name for readability)
	It's  a Codebook AVL Node used for searching toks and their encodings efficiently.
	**/
	typedef struct AVLNode CodeNode;


	/**
	QueueItems of Trees. Implemented with a doubly-linkedlist to save on time efficiency
	Only meant to be accessed within DataStructures.c
	**/
	typedef struct QueueItem{
		struct TreeNode* tree;
		struct QueueItem* prev;
		struct QueueItem* next;
	}QueueItem;


	/**
	Queue that keeps track of head and tail QueueItems
	**/
	typedef struct Queue{
		QueueItem* front;
		QueueItem* end;
	}Queue;


	/**
	MinHeap to keep track of Words and their Frequencies.
	Ordered based on the Frequencies of elements.
	**/
	typedef struct MinHeap{
		Token** heapArr; //array of Token pointers
		int size;
	}MinHeap;



//METHOD SIGNATURES
	Token* createTokenInt(char* word, int frequency);
	Token* createTokenStr( char* tok, char* encoding);
	void freeToken(Token* element);

	AVLNode* createAVLNode(char* word);
	Operation insertOrUpdateAVL(AVLNode**root_ptr, char* tok);
	int sizeOfAVLTree(AVLNode* root);
	void freeAVLTree(AVLNode* root);

	CodeNode* buildCodebookTree(char* codebook_name, CMPMode mode);
	char* getCodeItem( CodeNode* root, char* key, CMPMode mode);
	//Note:freeCodeTree(root) is the same as freeAVLTree(AVLNode* root) as defined in the macr
	
	TreeNode* createTreeNode(Token* element);
	TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2);
	void freeTreeOnly(TreeNode* root);
	void freeTreeAndTok(TreeNode* root); //frees the Token and its elements

	TreeNode* dequeue(Queue* q);
	void enqueue(Queue* q, TreeNode* tree);
	int peekQueue(Queue* q);
	bool hasSizeOne(Queue* q);
	void freeQueue(Queue* q);

	MinHeap buildMinHeap(AVLNode* root);
	Token* removeMin(MinHeap* heap);
	int peekMinHeap(MinHeap* heap);

	TreeNode* pickMinTreeHuffman(MinHeap* heap, Queue* q);

	void printToken(Token* element, char* formatting);
	void printHeap(MinHeap* heap_ptr);
	void printTree(TreeNode* root);
	void printAVLTree(AVLNode* root);
	//Note: printCodeTree(root) is same as printAVLTree(root) as defined in Macro
	void printQueue(Queue* q_ptr);

#endif
