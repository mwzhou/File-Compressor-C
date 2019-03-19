#ifndef DS_H
#define DS_H

	#define PRINT_ERROR(txt) (printf("ERROR: %s in: %s on line:%d\n",txt,__FILE__, __LINE__))
	#define createAVLNode(s) (createTreeNode(createWordFreq(s,1)))
	
	/**
	structure to associate a word with a frequency.
	**/
	typedef struct WordFreq{
		char* word;
		int frequency;
	}WordFreq;
	
	
	/**
	structure to combine frequencies of each WordFreq element for huffman coding
	**/
	typedef struct TreeNode{
		WordFreq* element;
		//Note: If element->word==NULL, it represents an combined frequency of the left and right subtrees
		
		struct TreeNode* left;
		struct TreeNode* right;
	}TreeNode;	


	//AVLNode is same type as TreeNode, just made a distinction for more clarity
	typedef TreeNode AVLNode;


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
	Queue that keeps track of head and tail
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
		WordFreq** heapArr; //array of WordFreq pointers
		int size;
	}MinHeap; 

	
	
	//Method Signatures
	WordFreq* createWordFreq(char* word, int frequency);
	void freeWordFreq(WordFreq* element);
	
	void insertOrUpdateAVL(AVLNode** root_ptr, char* word);
	int sizeOfTree(AVLNode* root);
	
	TreeNode* createTreeNode(WordFreq* element);
	TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2);
	void freeTreeOnly(TreeNode* root);
	void freeTreeAndWF(TreeNode* root); //frees the word frequency and its string
	
	TreeNode* dequeue(Queue* q);
	void enqueue(Queue* q, TreeNode* tree);
	void freeQueue(Queue q);
	
	MinHeap createMinHeap(AVLNode* root);
	WordFreq* removeMin(MinHeap* heap);
	
	void printWordFreq(WordFreq* element, char* s);
	void printHeap(MinHeap heap);
	void printTree(TreeNode* root);
	void printQueue(Queue q);

#endif