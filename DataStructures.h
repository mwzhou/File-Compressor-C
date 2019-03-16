#ifndef DS_H
#define DS_H

	/**
	structure to associate a word with a frequency.
	**/
	struct WordFreq{
		char* word;
		int frequency;
	};
	
	
	/**
	A search structure to keep track of frequencies of a word in a file. 
	Ordered lexiographically by each word
	**/
	struct AVLNode{
		struct WordFreq* element;

		struct AVLNode* left;
		struct AVLNode* right;
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
		struct WordFreq** heapArr; //array of WordFreq pointers
		int length;
	}; 

	
	//Method Signatures
	struct WordFreq* createWordFreq(char* word, int frequency);
	
	struct AVLNode* createAVLNode(char* word);
	void insertAndUpdate(struct AVLNode* root, char* word);
	int sizeOfAVL(struct AVLNode* root);
	
	struct TreeNode* createTreeNode(struct WordFreq* element);
	struct TreeNode* mergeTrees(struct TreeNode* t1, struct TreeNode* t2);
	
	struct TreeNode* dequeue(struct TreeQueue* head);
	void enqueue(struct TreeQueue* head, struct TreeNode* node);
	
	struct minheap* createMinheap(struct AVLNode* root);
	int initializeMinheapArr(struct AVLNode* tree, struct WordFreq** heapArr, int i);
	void heapify(struct minheap* heap);
	struct WordFreq* getMin(struct minheap* heap);



#endif
