/**
structures.c  is a self-made library that holds all the structures necessary to be used in fileCompression.c
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "structures_priv.h"


//Token methods/////////////////////////////////////////////////////////////////
//(description of struct in structures.h)

/**
Initializes a pointer to a Token object  (int version)
(To avoid time spent copying, Token is represented by a pointer to a Token object because it is reused in other methods.)
@params - tok - token in file, frequency - frequency of occurences of tok
@ret - Token pointer
**/
Token* createToken(char* tok, int frequency){
	Token* ret = (Token*)malloc(sizeof(Token));
	if( ret == NULL ){ pEXIT_ERROR("malloc"); }
	ret->tok = tok;
	ret->frequency = frequency;
	return ret;
}


/**
Initializes a pointer to a Token object (String version)
(To avoid time spent copying, Token is represented by a pointer to a Token object because it is reused in other methods.)
@params - tok - token in file, encoding - huffman byte_encoding of tokS
@ret - Token pointer
**/
Token* createTokenStr( char* tok, char* encoding){
	Token* ret = (Token*)malloc(sizeof(Token));
	if( ret == NULL ){ pEXIT_ERROR("malloc"); }
	ret->tok = tok;
	ret->encoding = encoding;
	return ret;
}


/**
Frees a Tokenuency and its String
**/
void freeToken(Token* element){
	free(element->tok);
	free(element);
}



//AVLNode methods (note this is the same type as TreeNode by typedef)////////////////////////////////////////////////
//(description of struct in structures.h)

/**
Initializes AVLNode
**/
AVLNode* createAVLNode(char* tok){
	AVLNode* ret = (AVLNode*)malloc(sizeof(AVLNode));
	if( ret== NULL){ pEXIT_ERROR("malloc"); }
	
	ret->element = createToken(tok,1);
	ret->height = 1;
	ret->left = NULL;
	ret->right = NULL;
	
	return ret;
}


/**
updates root by calling insertOrUpdateAVLRec() taking in the address to the root
@returns true if updated Frequency
 returns false if inserted new node
**/
bool insertOrUpdateAVL(AVLNode**root_ptr, char* tok){
	bool updatedFreq = false;
	(*root_ptr) = insertOrUpdateAVLRec(*root_ptr, tok, &updatedFreq);
	return updatedFreq;
}



/**
Searches through AVL tree recursively O(logn)
If found: Updates frequency and changes *updatedFreq bool to true
If not found: Creates a Token element and inserts it into the tree.
MAINTAINS AVL properties of the tree and balances if necessary
@params: root - root of AVL Tree, tok - tok to insert/update, updatedFreq - address of boolean to note whether updated or inserted
@returns: updated root after one insert/update
**/
static AVLNode* insertOrUpdateAVLRec(AVLNode* root, char* tok, bool* updatedFreq){
	if(root ==NULL)//no elements in the AVLTree yet
		return createAVLNode(tok);
	

	int strcmp_tok = strcmp(root->element->tok,tok);

	if(strcmp_tok<0){ //tok passed in is greater than current node
		root->left = insertOrUpdateAVLRec((root->left) , tok, updatedFreq);
	}else if(strcmp_tok>0){ //tok passed in is less than current node
		root->right = insertOrUpdateAVLRec((root->right) , tok, updatedFreq);
	}else{
		root->element->frequency++;
		*updatedFreq = true;
		return root;
	}

	//update height
	root->height = greaterHeight(root)+1;

	//Balancing the tree

	int balance_factor = (root==NULL)? 0 : (heightAVL(root->left)) - (heightAVL(root->right));
	root = BalanceAVL(root , balance_factor, tok);
	return root;
}


/** [private method]
Balances AVL Tree given a root after one insert (Constant time)
returns updated pointer to root
**/
static AVLNode* BalanceAVL(AVLNode* root, int balance_factor, char* tok){
	if(root==NULL||balance_factor==1|| balance_factor==0 ||tok==NULL){
		return root;
	}

	int strcmp_left = (root->left==NULL)? 0 : strcmp((root->left)->element->tok , tok);
	int strcmp_right = (root->right==NULL)? 0 : strcmp((root->right)->element->tok , tok);


	if(balance_factor>1 && strcmp_left<0){ //Case: left-left
		Case1Balance(&root,true);
	}else if(balance_factor>1 && strcmp_left>0){ //Case: left-right
		Case2Balance(&root,true);
	}else if(balance_factor<-1 && strcmp_right<0){ //Case: right-right
		Case1Balance(&root,false);
	}else if(balance_factor<-1 && strcmp_right>0){ //Case: right-left
		Case2Balance(&root,false);
	}

	return root;
}


/**[private method]
Balances Tree if Case1: i.e. left-left or left-right
@params: isLeft - meaning is Case Left-Left
**/
static void Case1Balance(AVLNode** root_ptr, bool isLeft){
	if(root_ptr==NULL||*root_ptr==NULL) return;

	//Case with Left-Left
	if (isLeft ){
		AVLNode* x = (*root_ptr)->left;
		AVLNode* T2 = x->right; //x's right subtree
		x->right = (*root_ptr);
		(*root_ptr)->left = T2;

		//update heights
		(*root_ptr)->height = greaterHeight((*root_ptr))+1;
		x->height = greaterHeight(x)+1;

		(*root_ptr) = x; //update root

	//Case with Right-Right
	}else{
		AVLNode* x = (*root_ptr)->right;
		AVLNode* T2 = x->left; //x's left-subtree
		x->left = (*root_ptr);
		(*root_ptr)->right = T2;

		//update heights
		(*root_ptr)->height = greaterHeight((*root_ptr))+1;
		x->height = greaterHeight(x)+1;

		(*root_ptr) = x; //update root
	}
}


/**[private method]
Balances Tree if Case2: i.e. left-right or right-left
@params: isLeftRight - meaning is Case Left-Right
**/
static void Case2Balance(AVLNode** root_ptr, bool isLeftRight){
	if(root_ptr==NULL||*root_ptr==NULL) return;

	//Case with Left Right
	if(isLeftRight){
		//turn into Left-Left Case1
			AVLNode* x = ((*root_ptr)->left)->right;
			AVLNode* T2 = x->left; //x's left subtree
			x->left = (*root_ptr)->left;
			(*root_ptr)->left->right = T2;

			//update height
			(*root_ptr)->left->height = greaterHeight((*root_ptr)->left)+1;
			x->height = greaterHeight(x)+1;

			(*root_ptr)->left = x;

		Case1Balance(root_ptr, true);

	}else{
		//turn into Right-Right Case1
			AVLNode* x = (*root_ptr)->right->left;
			AVLNode* T2 = x->right; //x's right subtree
			(*root_ptr)->right->left = T2;
			x->right = (*root_ptr)->right;

			//update heights
			(*root_ptr)->right->height = greaterHeight((*root_ptr)->right)+1;
			x->height = greaterHeight(x)+1;

			(*root_ptr)->right = x;

		Case1Balance(root_ptr, false);
	}
}


/**[private method]
returns height that is greater between the children Nodes of the node passed in, accounts for if node passed in is NULL*/
static int greaterHeight(AVLNode* parent){
	int left_h = heightAVL(parent->left);
	int right_h = heightAVL(parent->right);
	return (left_h > right_h)? left_h : right_h;
}


/**
gets number of nodes in an AVL Frequency Tree
**/
int sizeOfAVLTree(AVLNode* root){
	if(root==NULL)
		return 0;

	return 1+sizeOfAVLTree(root->left)+sizeOfAVLTree(root->right);
}


/**
frees all nodes in a AVLTree. PostOrder Traversal.
Note: DOES NOT free the Token element
**/
void freeAVLTree(AVLNode* root){
	if(root==NULL) return;

	freeAVLTree(root->left);
	freeAVLTree(root->right);
	free(root);
}



//CODEBOOK SEARCH NODE methods (Note: is same type as AVLNode)/////////////////////////////////////////////////////////////////
//(description of struct in structures.h)

/**
Initializes CodebookSearchNode
**/
CodebookSearchNode* createCodebookSearchNode(char* tok, char* encoding){
	CodebookSearchNode* ret = (CodebookSearchNode*)malloc(sizeof(CodebookSearchNode));
	if( ret== NULL){ pEXIT_ERROR("malloc"); }
	
	ret->element = createTokenStr(tok, encoding);
	ret->height = 1;
	ret->left = NULL;
	ret->right = NULL;
	
	return ret;
}


/**
**/
CodebookSearchNode* insertCodebookSearchTree(){ //TODO
	return NULL;
}



//TREENODE methods/////////////////////////////////////////////////////////////////
//(description of struct in structures.h)

/**
Initializes a TreeNode structure and returns a pointer to the created TreeNode
**/
TreeNode* createTreeNode(Token* element){
	TreeNode* ret = (TreeNode*)malloc(sizeof(TreeNode));
	if(ret==NULL){ pEXIT_ERROR("malloc"); }
	ret->element = element;
	ret->left = NULL;
	ret->right = NULL;
	return ret;
}


/**
merges two trees into one and returns the root (the combined frequency)
Note: the root->element->tok=NULL (because it only represents a frequency!)
**/
TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2){ //TODO
	if((t1==NULL&&t2==NULL)){
		PRINT_ERROR("cannot pass in two NULL TreeNodes into mergeTrees()");
		return NULL;
	}else if(t1==NULL){
		return t2;
	}else if(t2==NULL){
		return t1;
	}

	Token* root_tokf = createToken(NULL, (t1->element->frequency + t2->element->frequency) );
	TreeNode* root = createTreeNode(root_tokf);

	root->left = t1;
	root->right = t2;
	return root;
}


/**
frees all nodes in a Tree. PostOrder Traversal.
Note: DOES NOT free the Token element
**/
void freeTreeOnly(TreeNode* root){
	if(root==NULL) return;

	freeTreeOnly(root->left);
	freeTreeOnly(root->right);
	free(root);
}


/**
frees all nodes in a Tree. PostOrder Traversal.
Note: Frees Token AND its string. Be careful if you want to use the String for further use.
**/
void freeTreeAndWF(TreeNode* root){
	if(root==NULL) return;

	freeTreeAndWF(root->left);
	freeTreeAndWF(root->right);

	freeToken(root->element);
	free(root);
}



//QUEUE methods////////////////////////////////////////////////////////////////
//(description of struct in structures.h)

/**[private method]
Initializes a QueueItem given an existing tree and returns a pointer to that QueueItem
**/
static QueueItem* createQueueItem(TreeNode* tree){
	QueueItem* ret = (QueueItem*)malloc(sizeof(QueueItem));
	if( ret == NULL ){ pEXIT_ERROR("malloc"); }
	ret->tree = tree;
	ret->prev = NULL;
	ret->next = NULL;
	return ret;
}


/**
takes first tree from the Queue's front
(free's queue pointer that was removed from the queue (but not the tree pointer))
updates q's front
@params: q - address of a Queue that contains pointer to front
@returns: TreeNode* at front
**/
TreeNode* dequeue(Queue* q){
	if(q==NULL||((q->front) ==NULL||(q->end) ==NULL))
		return NULL;

	QueueItem* temp = (q->front); //for freeing later
	TreeNode* ret = (q->front)->tree;

	if((q->front)->next == NULL){ //only one element left in Queue
		(q->front) = NULL; //update front
		(q->end) = NULL; //update end
		free(temp);
		return ret;
	}

	((q->front)->next)->prev=NULL;
	(q->front) = (q->front)->next;//update front of q
	free(temp);
	return ret;

}


/**
enqueues a tree onto the back of the queue, updates end of queue
@params: q - address of Queue that contains a pointer to the end
**/
void enqueue(Queue* q, TreeNode* tree){
	if(q==NULL) return;

	if((q->end)==NULL||(q->front) == NULL){ //Queue has no elements yet
		(q->front) = createQueueItem(tree);
		(q->end) = (q->front);
		return;
	}

	QueueItem* addEl = createQueueItem(tree);
	addEl->prev = (q->end);
	(q->end)->next = addEl;
	(q->end) = addEl; //update end of queue
}


/**
returns the frequency of the root from the front of Queue.
If q is NULL, tree at front of q is NULL, or element at the root of the tree is NULL, returns INT_MAX;
**/
int peekQueue(Queue* q){
	if(q==NULL || q->front->tree==NULL || q->front->tree->element==NULL){
		return INT_MAX;
	}
	return q->front->tree->element->frequency;
}

/**
checks if Queue has one element left
**/
bool hasSizeOne(Queue* q){
	if(q==NULL||q->front==NULL)
		return false;
	else if(q->front->next==NULL && q->front->prev==NULL)
		return true;

	return false;
}


/**
free's all Queue nodes if necessary (does not touch the trees in case of future use)
however, if implemented correctly, FileCompression.c will not need to call upon this method
**/
void freeQueue(Queue* q){
	if(q==NULL||(q->front)==NULL||q->end==NULL)
		return;

	QueueItem* ptr = (q->front);
	while(ptr!=NULL){
		QueueItem* temp = ptr;
		ptr = ptr->next;
		free(temp);
	}
}



//MINHEAP methods//////////////////////
//(description of struct in structures.h)

/**
creates a MinHeap from an AVL Frequency Tree
@params: tree - AVL tree root that keeps track of frequencies of each tok
@ret : a MinHeap w/ an initialized heap array, or if passed in tree is faulty, a Minheap with a NULL array and size 0
**/
MinHeap createMinHeap(AVLNode* tree){
	MinHeap ret = {NULL,0};

	if(tree==NULL){
		PRINT_ERROR("cannot create a heap from a NULL tree");
		return ret;
	}

	ret.size = sizeOfAVLTree(tree);
	ret.heapArr = (Token**)calloc( (ret.size), (ret.size) * sizeof(Token*) ); //creates array big enough to hold all elements in AVLTree
	initializeMinHeapArr(tree, ret.heapArr, 0); //inserts each node of AVLtree into heapArr
	heapify(&ret); //turns array into a heap

	return ret;
}


/**[private method]
Serializes an AVL tree into an array.
Traverses through an AVL tree and inserts each Token element into the heapArr
@params: i is the current index of the next element in heapArr (updated recursively)
@returns: an int to keep track of the index
**/
static int initializeMinHeapArr(AVLNode* node, Token** heapArr, int i){
	if(node->left != NULL)
		i= initializeMinHeapArr(node->left, heapArr, i);
	heapArr[i++]= node->element;
	if(node->right != NULL)
		i = initializeMinHeapArr(node->right, heapArr, i);

	return i;
}


/**[private method]
linear heapify
takes initialized MinHeap pointer and heapifies it with the O(n) time algorithm. Starts from first non-leaf node and sifts-down
**/
static void heapify(MinHeap* heap){
	if(heap==NULL||heap->heapArr==NULL){
		PRINT_ERROR("can't pass in null heap or uninitialized heap");
		return;
	}

	int currind = (heap->size)/2 - 1;
	while(currind>=0){
		siftDown(heap->heapArr, heap->size, currind);
		currind--;
	}
}


/**[private method]
sift-down algorithm for heaps
given an index in a heapArray, siftDown() checks if all nodes in the subtree are smaller than that element
performs the necessary swaps to mantain a minheap structure
**/
static void siftDown(Token** heapArr, int size, int ind){
	if(heapArr==NULL||size<=ind){
		PRINT_ERROR("either heapArr uninitialized or faulty index/size passed");
		return;
	}

	while(ind < (size/2)){ //while ind is not a leaf node index
		int l = 2*ind + 1; //left node index
		int r = 2*ind + 2; //right node index

		if(r>=size){ //if curr node has no right child
			if(((heapArr[ind]->frequency)>(heapArr[l]->frequency))){//if currnode is greater than left node
				swap(&heapArr[ind], &heapArr[l]);
				ind = l;
			}else{ //no more comparisons left
				return;
			}
		}else{ //curr node has two children
			int minChildInd = (heapArr[l]->frequency <= heapArr[r]->frequency)? l:r; //index of smallest child

			if((heapArr[ind]->frequency)>(heapArr[minChildInd]->frequency)){//if currnode is greater than smallest child node
				swap(&heapArr[ind], &heapArr[minChildInd]);
				ind = minChildInd;
			}else{ //if currnode is equal to both children
				return;
			}
		}
	}
}


/**[private method]
Swaps two Token pointers given the addresses of the two pointers
**/
static void swap(Token** element1, Token** element2){
	Token* temp = *element1;
	*element1 = *element2;
	*element2 = temp;
}


/**
returns from top of the heap and then updates the heap (sifts up)
**/
Token* removeMin(MinHeap* heap){
	if(heap==NULL||heap->size==0){
		PRINT_ERROR("cannot remove min from empty or null heap");
		return NULL;
	}

	Token* min= heap->heapArr[0];
	swap(&(heap->heapArr[0]), &(heap->heapArr[heap->size-1])); //swaps last element to the top
	heap->heapArr[heap->size-1]=NULL; //removes element from heapArray
	heap->size--; //decreases accesible elements in a heap
	if(heap->size!=0)siftDown(heap->heapArr,heap->size,0);

	return min;
}


/**
returns frequency from min item on heap
If heap is NULL or element on heap is NULL, returns INT_MAX;
**/
int peekMinHeap(MinHeap* heap){
	if(heap==NULL|| heap->heapArr[0]==NULL){
		return INT_MAX;
	}

	return heap->heapArr[0]->frequency;
}



//MISC methods/////////////////////////////////////////////////////////////

/**
method to be used for huffman coding.
picks minimum frequency from top of Heap or top of Queue, removes the min from the data structure
@returns: the tree of the minimum frequency
**/
TreeNode* pickMinTreeHuffman(MinHeap* heap, Queue* q){
	if((heap==NULL || heap->size==0) && q==NULL){
		PRINT_ERROR("Nothing to compare");
		return NULL;
	}

	if(heap==NULL || heap->size==0){
		return dequeue(q);
	}else if(q==NULL || (q->front==NULL&& q->end==NULL)){
		return createTreeNode( removeMin(heap) );
	}

	return (peekMinHeap(heap) < peekQueue(q))? createTreeNode( removeMin(heap) ) : dequeue(q);
}



//PRINT methods/////////////////////////////////////////////////////////////

/**
prints out Token
@params: formatting is any additional parameters to add to the string output
**/
void printToken(Token* wf, char* formatting){
	if(wf==NULL){
		printf("NULL %s",formatting);
		return;
	}

	if(wf->tok==NULL){
		printf("NULL:%d %s",wf->frequency,formatting);
	}else{
		printf("%s:%d %s",wf->tok,wf->frequency,formatting);
	}
}

void printHeap(MinHeap* heap_ptr){
	if(heap_ptr==NULL || heap_ptr->heapArr==NULL){
		printf("NULL\n");
		return;
	}

	printf("/////////////////////////////////////////////////\n");
	printf("[HEAP:(horizontal)]\n");
	printHeapRec(heap_ptr,0,0);
	printf("/////////////////////////////////////////////////\n");
}

//[private method] recursively prints heap(horizontally)
static void printHeapRec(MinHeap* heap_ptr, int root, int space){
  	if (root>=heap_ptr->size)return;

  	int count = 10;
  	int i;
  	space += count; //increases space inbetween elements

    printHeapRec(heap_ptr, 2*root+2, space);

    printf("\n");
    for (i = count; i < space; i++){ printf(" ");}
    printToken(heap_ptr->heapArr[root],"\n");

 	printHeapRec(heap_ptr, 2*root+1, space);
}

//printsHeapArray for testing
void printHeapArray(Token** arr, int size){
	int i;
	for(i=0; i<size; i++){
		printToken(arr[i],"\t");
	}
	printf("\n");
}

void printTree(TreeNode* root){
	if(root==NULL){
		printf("NULL\n");
		return;
	}

	printf("---------------------------------------------\n");
	printf("[TREE:(horizontal)]\n");
	printTreeRec(root,0);
	printf("---------------------------------------------\n");
}

//[private method] prints Tree recursively (horizontally). Root on far left. In order Traversal.
static void printTreeRec(TreeNode* root, int space){
  	if (root == NULL)return;

  	int count = 10;
  	int i;
  	space += count; //increases space inbetween elements

    printTreeRec(root->right, space);

    printf("\n");
    for (i = count; i < space; i++){ printf(" ");}
    printToken(root->element,"\n");

    printTreeRec(root->left, space);
}

void printAVLTree(AVLNode* root){
	if(root==NULL){
		printf("NULL\n");
		return;
	}

	printf("---------------------------------------------\n");
	printf("[AVLTREE:(horizontal)]\n");
	printAVLTreeRec(root,0);
	printf("---------------------------------------------\n");
}

//[private method] prints Tree recursively (horizontally). Root on far left. In order Traversal.
static void printAVLTreeRec(AVLNode* root, int space){
  	if (root == NULL)return;

  	int count = 10;
  	int i;
  	space += count; //increases space inbetween elements

    printAVLTreeRec(root->right, space);

    printf("\n");
    for (i = count; i < space; i++){ printf(" ");}
    printToken(root->element,"\n");

    printAVLTreeRec(root->left, space);
}

void printQueue(Queue* q_ptr){
	Queue q = *q_ptr;

	if(q.end==NULL||q.front==NULL){
		printf("\nNULL\n\n");
		return;
	}

	printf("/////////////////////////////////////////////////\n");
	printf("\nQUEUE:\n");
	QueueItem* ptr = q.front;
	while(ptr!=NULL){
		//front and end indicators
		if(ptr->prev==NULL && ptr->next==NULL)
			printf("[front&end]");
		else if(ptr->next==NULL)
			printf("[end]");
		else if(ptr->prev==NULL)
			printf("[front]");

		printTree(ptr->tree);
		printf("\n");
		ptr = ptr->next;
	}

	printf("/////////////////////////////////////////////////\n");
}
