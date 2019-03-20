#ifndef DS_H
#define DS_H

	#define PRINT_ERROR(txt) (printf("ERROR: %s in: %s on line:%d\n",txt,__FILE__, __LINE__))
	
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


	/**
	structure to order tokens in a file lexiographically in a balanced tree and keep track of its frequencies
	**/
	typedef struct AVLNode{
		WordFreq* element;
		int height;
		struct AVLNode* left;
		struct AVLNode* right;
	}AVLNode;


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
	
	AVLNode* createAVLNode(char* word);	
	AVLNode* insertOrUpdateAVL(AVLNode* root, char* word);
	int sizeOfAVLTree(AVLNode* root);
	
	TreeNode* createTreeNode(WordFreq* element);
	TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2);
	void freeTreeOnly(TreeNode* root);
	void freeTreeAndWF(TreeNode* root); //frees the word frequency and its string
	
	TreeNode* dequeue(Queue* q);
	void enqueue(Queue* q, TreeNode* tree);
	int peekQueue(Queue* q);
	bool hasSizeOne(Queue* q);
	void freeQueue(Queue* q);
	
	MinHeap createMinHeap(AVLNode* root);
	WordFreq* removeMin(MinHeap* heap);
	int peekMinHeap(MinHeap* heap);
	
	
	void printWordFreq(WordFreq* element, char* s);
	void printHeap(MinHeap* heap_ptr);
	void printTree(TreeNode* root);
	void printAVLTree(AVLNode* root);
	void printQueue(Queue* q_ptr);

#endif
