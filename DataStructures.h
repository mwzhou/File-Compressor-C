#ifndef DS_H
#define DS_H

	#define PRINT_ERROR(txt) (printf("%s in file:%s on line:%d\n",txt,__FILE__, __LINE__))
	#define PRINT_WORDFREQ(wf,txt) (printf("%s:%d %s",wf->word,wf->frequency,txt))
	
	
	/**
	structure to associate a word with a frequency.
	**/
	struct WordFreq{
		char* word;
		int frequency;
	};
	typedef struct WordFreq* WordFreq;
	
	/**
	A search structure to keep track of frequencies of a word in a file. 
	Ordered lexiographically by each word
	**/
	struct AVLNode{
		WordFreq element;

		struct AVLNode* left;
		struct AVLNode* right;
	};


	/**
	structure to combine frequencies of each WordFreq element for huffman coding
	**/
	struct TreeNode{
		WordFreq element;
		//Note: If element->word==NULL, it represents an combined frequency of the left and right subtrees
		
		struct TreeNode* left;
		struct TreeNode* right;
	};

	/**
	Queue of TreeNodes
	**/
	struct TreeQueue{
		struct TreeNode* root;
		struct TreeQueue* next;
	};


	/**
	MinHeap to keep track of Words and their Frequencies.
	Ordered based on the Frequencies of elements.
	**/
	struct MinHeap{
		WordFreq* heapArr; //array of WordFreq pointers
		int size;
	}; 

	
	//Method Signatures
	WordFreq createWordFreq(char* word, int frequency);
	
	struct AVLNode* createAVLNode(char* word);
	void insertAndUpdate(struct AVLNode* root, char* word);
	int sizeOfAVL(struct AVLNode* root);
	
	struct TreeNode* createTreeNode(WordFreq element);
	struct TreeNode* mergeTrees(struct TreeNode* t1, struct TreeNode* t2);
	
	struct TreeNode* dequeue(struct TreeQueue* head);
	void enqueue(struct TreeQueue* head, struct TreeNode* node);
	
	struct MinHeap* createMinHeap(struct AVLNode* root);
	int initializeMinHeapArr(struct AVLNode* tree, WordFreq* heapArr, int i);
	void heapify(struct MinHeap* heap);
	void siftDown(WordFreq* heapArr, int size, int ind);
	void swap(WordFreq* element1, WordFreq* element2);
	WordFreq removeMin(struct MinHeap* heap);
	
	void printHeap(struct MinHeap* heap);
	void printHeapArray(WordFreq* arr, int size);



#endif
