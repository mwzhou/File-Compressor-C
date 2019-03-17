#ifndef DS_H
#define DS_H

	#define PRINT_ERROR(txt) (printf("ERROR: %s in: %s on line:%d\n",txt,__FILE__, __LINE__))
	#define PRINT_WORDFREQ(wf,txt) (printf("%s:%d %s",wf->word,wf->frequency,txt))
	
	
	/**
	structure to associate a word with a frequency.
	**/
	typedef struct WordFreq{
		char* word;
		int frequency;
	}WordFreq;
	
	
	/**
	A search structure to keep track of frequencies of a word in a file. 
	Ordered lexiographically by each word
	**/
	typedef struct AVLNode{
		WordFreq* element;

		struct AVLNode* left;
		struct AVLNode* right;
	}AVLNode;


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
	QueueItems of Trees. Implemented with a doubly-linkedlist to save on time efficiency
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
	void insertOrUpdateAVL(AVLNode** root_ptr, char* word);
	int sizeOfAVL(AVLNode* root);
	void freeAVLNode(AVLNode* root);
	
	TreeNode* createTreeNode(WordFreq* element);
	TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2);
	void freeTreeNodeWithWF(TreeNode* root); //frees the word frequency and its string
	
	QueueItem* createQueueItem(TreeNode* tree);	
	TreeNode* dequeue(Queue* q);
	void enqueue(Queue* q, TreeNode* tree);
	void freeQueue(Queue* q);
	
	MinHeap createMinHeap(AVLNode* root);
	int initializeMinHeapArr(AVLNode* tree, WordFreq** heapArr, int i);
	void heapify(MinHeap* heap);
	void siftDown(WordFreq** heapArr, int size, int ind);
	void swap(WordFreq** element1, WordFreq** element2);
	WordFreq* removeMin(MinHeap* heap);
	
	void printHeap(MinHeap heap);
	void printHeapArray(WordFreq** arr, int size);
	void printTree(TreeNode* root, int space);
	void printQueue(Queue q);



#endif
