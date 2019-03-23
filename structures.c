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
@params: char* tok - token in file; int frequency: frequency of occurences of tok in file
@ret: pointer to Token
**/
Token* createTokenInt(char* tok, int frequency){
	Token* ret = (Token*)malloc(sizeof(Token));
	if( ret == NULL ){ pEXIT_ERROR("malloc"); }

	ret->hasFrequency = true;
	ret->tok = tok;
	ret->frequency = frequency;

	return ret;
}


/**
Initializes a pointer to a Token object (String version)
(To avoid time spent copying, Token is represented by a pointer to a Token object because it is reused in other methods.)
@params: char* tok - token in file; char* encoding - huffman byte_encoding of tok in codebook
@ret: pointer to Token
**/
Token* createTokenStr( char* tok, char* encoding){
	Token* ret = (Token*)malloc(sizeof(Token));
	if( ret == NULL ){ pEXIT_ERROR("malloc"); }

	ret->hasFrequency = false;
	ret->tok = tok;
	ret->encoding = encoding;

	return ret;
}


/**
Frees a Token and its items
**/
void freeToken(Token* element){
	if(element==NULL)
		return;
	
	if(element->tok!=NULL)
		free(element->tok);
	if( element->hasFrequency==false && element->encoding!=NULL ) // if element has a Non-NULL Encoding
		free(element->encoding);
		
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

	ret->element = createTokenInt(tok,1);
	ret->height = 1;
	ret->left = NULL;
	ret->right = NULL;

	return ret;
}


/**
wrapper method for insertOrUpdateAVLRec()
updates root by calling insertOrUpdateAVLRec() taking in the address to the root, updates root in memory
@returns true if updates Frequency
		 false if inserted new node
**/
bool insertOrUpdateAVL(AVLNode**root_ptr, char* tok){
	bool updatedFreq = false;
	(*root_ptr) = insertOrUpdateAVLRec(*root_ptr, tok, &updatedFreq);
	return updatedFreq;
}



/** [private method]
Searches through AVL tree recursively O(logn)
If found: Updates frequency and changes *updatedFreq bool to true
If not found: Creates a Token element and inserts it into the tree.
MAINTAINS AVL properties of the tree and balances if necessary
@params: AVLNode* root - root of AVL Tree,
		 char* tok - tok to insert/update,
		 bool* updatedFreq - address of boolean to note whether updated or inserted
@returns: updated root after one insert/update
**/
static AVLNode* insertOrUpdateAVLRec(AVLNode* root, char* tok, bool* updatedFreq){
	if(root ==NULL)//no elements in the AVLTree yet
		return createAVLNode(tok);

	//Comparisons and Insert
	int strcmp_tok = strcmp(root->element->tok,tok);
	if(strcmp_tok<0){ //tok passed in is greater than root's tok
		root->left = insertOrUpdateAVLRec((root->left) , tok, updatedFreq);
	}else if(strcmp_tok>0){ //tok passed in is less than root's tok
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
	root = BalanceAVL(root , balance_factor, tok, cmpByTokens);
	return root;
}


/** [private method]
Balances AVL Tree given a root after one insert (Constant time)
@params: AVLNode* root - root of AVLTree;
		 int balance_factor - how imbalanced the tree is;
		 char* key  - token/encoding string to compare root->left and root->right to, to figure out what type of imbalance
		 CMPMode mode - comparison mode, whether to compare by tokens or encodings
@returns: updated pointer to balanced root.
**/
static AVLNode* BalanceAVL(AVLNode* root, int balance_factor, char* key , CMPMode mode){
	if(root==NULL||balance_factor==1|| balance_factor==0 ||key ==NULL) //can't compare
		return root;
	else if( mode == cmpByEncodings && root->element->hasFrequency ){ //if root has a frequency (i.e. no encoding), you can't compare the encodings
		pRETURN_ERROR("cannot compare root's encoding if the root does not have an encoding", root);
	}

	int strcmp_left;
	int strcmp_right;

	//initializes values of strcmp_left and strcmp_right based on comparison mode
	if(mode == cmpByEncodings ){ //compares children's encodings
		strcmp_left = (root->left==NULL)? 0 : strcmp( (root->left)->element->encoding , key);
		strcmp_right = (root->right==NULL)? 0 : strcmp( (root->right)->element->encoding , key);
	}else if( mode == cmpByTokens ){//compares children's toks
		strcmp_left = (root->left==NULL)? 0 : strcmp( (root->left)->element->tok , key);
		strcmp_right = (root->right==NULL)? 0 : strcmp( (root->right)->element->tok , key);
	}


	//Balancing - finds what type of imbalance root is
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
Initializes CodeNode
**/
CodeNode* createCodeNode(char* tok, char* encoding){
	CodeNode* ret = (CodeNode*)malloc(sizeof(CodeNode));
	if( ret== NULL){ pEXIT_ERROR("malloc"); }

	ret->element = createTokenStr(tok, encoding);
	ret->height = 1;
	ret->left = NULL;
	ret->right = NULL;

	return ret;
}


/**
wrapper method just to be consistent with insertOrUpdateAVL()
takes address of CodeTree root and updates it in memory by inserting it
**/
void insertCodeTree( CodeNode** root_ptr, char* tok, char* encoding, CMPMode mode){
	*root_ptr = insertCodeTreeRec( (*root_ptr), tok, encoding, mode);
}


/**[private method]
inserts a token and an encoding into a CodeTree Recursively
@params: CodeNode* root: root of Tree;
		 char* tok - tok to insert ; char* encoding - encoding to insert associated with tok;
		 CMPMode mode- whether comparing based on Encoding, or comparing based on Token
**/
static CodeNode* insertCodeTreeRec( CodeNode* root, char* tok, char* encoding, CMPMode mode ){
	if(root ==NULL)//no elements in the AVLTree yet
		return createCodeNode( tok, encoding );

	//COMPARISONS AND INSERTING
	//stores value of comparison between tokens or a comparison between encodings
	int strcmp_tok = (mode == cmpByEncodings )? strcmp(root->element->encoding, encoding) : strcmp(root->element->tok,tok);

	if(strcmp_tok<0){
		root->left = insertCodeTreeRec(root->left, tok, encoding, mode);
	}else if(strcmp_tok>0){
		root->right = insertCodeTreeRec(root->right, tok, encoding, mode);
	}else{
		pRETURN_ERROR("duplicate Node in CodeNode", root);
		return root;
	}

	//update height
	root->height = greaterHeight(root)+1;

	//BALANCING TREE
	int balance_factor = (root==NULL)? 0 : (heightAVL(root->left)) - (heightAVL(root->right));
	//update root to balanced root based on if comparing Lexicographically
	root = (mode == cmpByEncodings)? BalanceAVL(root , balance_factor, tok, true) : BalanceAVL(root , balance_factor, tok, false);
	return root;
}


/**
Searches through CodeNode based on the key given, returns the tok/encoding associated with the key
@params:
@returns: String Item associated with key (if mode is cmpByEncodings, returns the token; if not, returns the encoding)
**/
char* getCodeItem( CodeNode* root, char* key, CMPMode mode ){ //TODO: search function
	return NULL;

}


/**
frees CodeTree AND the Token associated with it along with its tok and encoding strings
**/
void freeCodeTreeAndTok( CodeNode* root ){
	if(root==NULL) return;

	freeCodeTreeAndTok(root->left);
	freeCodeTreeAndTok(root->right);
	freeToken(root->element);
	free(root);
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
		pRETURN_ERROR("cannot pass in TWO NULL TreeNodes into mergeTrees() (at most one tree can be NULL)", NULL);
	}else if(t1==NULL){
		return t2;
	}else if(t2==NULL){
		return t1;
	}

	Token* root_tokf = createTokenInt(NULL, (t1->element->frequency + t2->element->frequency) );
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
void freeTreeAndTok(TreeNode* root){
	if(root==NULL) return;

	freeTreeAndTok(root->left);
	freeTreeAndTok(root->right);

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
FREES queue pointer that was removed from the queue (but not the tree pointer)
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
	if(q==NULL||(q->front)==NULL||(q->end)==NULL)
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
		pRETURN_ERROR("cannot create a heap from a NULL tree", ret);
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
	if(heap==NULL||heap->heapArr==NULL){ pRETURN_ERRORvoid("can't pass in null heap or uninitialized heap into heapify()"); }

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
	if(heapArr==NULL||size<=ind){ pRETURN_ERRORvoid("either heapArr uninitialized or faulty index/size passed into siftDown()"); }

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
	if(heap==NULL||heap->size==0){ pRETURN_ERROR("cannot remove min from empty or null heap" ,NULL); }

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
 returns: NULL on error
**/
TreeNode* pickMinTreeHuffman(MinHeap* heap, Queue* q){
	if((heap==NULL || heap->size==0) && q==NULL){ pRETURN_ERROR("Nothing to compare, both heap and q are empty/NULL", NULL); }

	if(heap==NULL || heap->size==0){ //heap is Empty
		return dequeue(q); //Note: this frees the element in dequeue()
	}else if(q==NULL || (q->front==NULL&& q->end==NULL)){ //queue is empty
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

	if(wf->hasFrequency)
		printf("%s:%d %s",wf->tok,wf->frequency,formatting);
	else
		printf("%s:%s %s",wf->tok,wf->encoding,formatting);
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
