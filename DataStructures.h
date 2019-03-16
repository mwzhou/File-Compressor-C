#ifndef DS_H
#define DS_H

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
	structure to associate a word with a frequency.
	**/
	struct WordFreq{
		char* word;
		int frequency;
	};


	/**
	structure to combine frequencies of each WordFreq element for huffman coding
	**/
	struct TreeNode{
		struct WordFreq* element;
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
	minheap to keep track of Words and their Frequencies.
	Ordered based on the Frequencies of elements.
	**/
	struct minheap{
		struct WordFreq** heapArray; //array of WordFreq pointers
	}; 

	
	//Method Signatures
	struct AVLFreqNode* createAVLNode(char* word);
	void insertAndUpdate(struct AVLFreqNode* root, char* word);
	struct WordFreq* createWordFreq(char* word, int frequency);
	struct TreeNode* createTreeNode(struct WordFreq* element);
	struct TreeNode* mergeTrees(struct TreeNode* t1, struct TreeNode* t2);
	struct TreeNode* dequeue(struct TreeQueue* head);
	void enqueue(struct TreeQueue* head, struct TreeNode* node);
	struct minheap* createMinHeap(struct AVLFreqNode* root, int size);
	void heapify(struct minheap* heap);
	struct WordFreq* getMin(struct minheap* heap);



#endif
