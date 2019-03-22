#ifndef DS_H
#define DS_H

	#define PRINT_ERROR(txt) (printf("ERROR: %s in: %s on line:%d\n",txt,__FILE__, __LINE__))
	#define pEXIT_ERROR(txt) do{perror(txt); exit(EXIT_FAILURE); }while(0)
	
	
	typedef enum{ INT, CHAR }type;
	
	
	/**
	structure to associate a word with a frequency OR an encoding
	**/
	typedef struct Token{
		char* tok;
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
	Note: CodebookSearchNode is the same type as an AVL Node (different name for readability)
	Is used for searching toks and their encodings.
	**/
	typedef struct AVLNode CodebookSearchNode;
	
	
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



	//Method Signatures
	Token* createToken(char* word, int frequency);
	Token* createTokenStr( char* tok, char* encoding);
	void freeToken(Token* element);

	AVLNode* createAVLNode(char* word);
	bool insertOrUpdateAVL(AVLNode**root_ptr, char* word);
	int sizeOfAVLTree(AVLNode* root);
	void freeAVLTree(AVLNode* root);

	TreeNode* createTreeNode(Token* element);
	TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2);
	void freeTreeOnly(TreeNode* root);
	void freeTreeAndWF(TreeNode* root); //frees the word frequency and its string

	TreeNode* dequeue(Queue* q);
	void enqueue(Queue* q, TreeNode* tree);
	int peekQueue(Queue* q);
	bool hasSizeOne(Queue* q);
	void freeQueue(Queue* q);

	MinHeap createMinHeap(AVLNode* root);
	Token* removeMin(MinHeap* heap);
	int peekMinHeap(MinHeap* heap);

	TreeNode* pickMinTreeHuffman(MinHeap* heap, Queue* q);

	void printToken(Token* element, char* formatting); 
	void printHeap(MinHeap* heap_ptr);
	void printTree(TreeNode* root);
	void printAVLTree(AVLNode* root);
	void printQueue(Queue* q_ptr);

#endif
