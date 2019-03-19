/**
structures.c holds all the structures necessary to be used in fileCompression.c
**/
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include "structures_priv.h"

	
//WORDFREQ methods/////////////////////////////////////////////////////////////////

/**
Initializes a WordFreq pointer to a word frequency object because it will be passed around in many structs.
To avoid time spent copying, WordFreq is represented by a pointer to a WordFreq object because it is reused in so many structs.
@params - char* word, int frequency
@ret - WordFreq pointer
**/
WordFreq* createWordFreq(char* word, int frequency){
	WordFreq* ret = (WordFreq*)malloc(sizeof(WordFreq));
	ret->word = word;
	ret->frequency = frequency;
	return ret;
}


/**
Frees a WordFrequency and its String
**/
void freeWordFreq(WordFreq* element){
	free(element->word);
	free(element);
}

/**
Initializes AVLNode
**/
AVLNode* createAVLNode(char* word){
	AVLNode* ret = (AVLNode*)malloc(sizeof(AVLNode));
	ret->element = createWordFreq(word,1);
	ret->height = 1;
	ret->left = NULL;
	ret->right = NULL;
	return ret;
}



//AVLNode methods (note this is the same type as TreeNode by typedef)////////////////////////////////////////////////

/**
Searches through AVL tree. 
If found: Updates word's frquency
If not found: Creates a WordFreq element and inserts it into the tree.
MAINTAINS AVL properties of the tree and balances if necessary
@params: root_ptr - address of the pointer to the AVL Tree root (neccesary to survive the scope of the function)
**/
void insertOrUpdateAVL(AVLNode** root_ptr, char* word){ //TODO
	if(root_ptr==NULL){
		PRINT_ERROR("can't pass in NULL pointer into insertOrUpdateAVL()");
		return;
	}
		 
	if(*root_ptr==NULL){ //no elements in AVLTree yet
		*root_ptr = createAVLNode(word);
		return;
	}
	
	int found = searchAVL(*root_ptr, word);

	//if the word is found, exit, otherwise insert the word
	if(found==0)
		return;
	else{
		*root_ptr = insert(*root_ptr, word);
	}
}

/**[private method]
searches for node in tree. If found, updates frequency and returns 0. If not found, returns 1.
**/
static int searchAVL(AVLNode* root, char* word){	
	while(root!=NULL){
		if(strcmp(root->element->word,word)==0){
			root->element->frequency++;
			return 0;
		}
		if(strcmp(root->element->word,word)>0)//1 is greater than 2
			root = root->left;
		else if(strcmp(root->element->word,word)<0)//2 is greater than 1
			root = root->right;
	}
	return 1;
}

/**[private method]
inserts nodes in tree. Re-Arranges Heights, set up balance factor to balance tree after inertion.
**/
static AVLNode* insert(AVLNode* root, char* word){
	AVLNode* root_ptr = root;

	if(root_ptr == NULL)
		return createAVLNode(word);
	
	if(strcmp(root_ptr->element->word,word)>0)
		root_ptr->left = insert(root_ptr->left,word);
	else if(strcmp(root_ptr->element->word,word)<0)
		root_ptr->right = insert(root_ptr->right,word);
	else
		return root_ptr;

	root_ptr->height = greater(root_ptr)+1;

	int balanceFactor;
	if(root_ptr==NULL)
		balanceFactor = 0;
	else
		balanceFactor = height(root_ptr->left)- height(root_ptr->right);

	root = balanceAVL(root_ptr,balanceFactor,word);
	
	return root;	
}

/**[private method]
balances nodes in AVL after one insert*/

static AVLNode* balanceAVL(AVLNode* toBalance, int balanceFactor,char* word){

	//Case with Left Left
	if(balanceFactor>1 && strcmp(toBalance->left->element->word,word)>0){
		AVLNode* x = toBalance->left;
		AVLNode* T2 = x->right;

		x->right = toBalance;
		toBalance->left = T2;

		toBalance->height = greater(toBalance)+1;
		x->height = greater(x)+1;

		return x;
	}
	//Case with Left Right
	else if(balanceFactor>1 && strcmp(toBalance->left->element->word,word)<0){
		AVLNode* x = toBalance->left->right;
		AVLNode* T2 = x->left;

		x->left = toBalance->left;
		toBalance->left->right = T2;

		toBalance->left->height = greater(toBalance->left)+1;
		x->height = greater(x)+1;

		toBalance->left = x;

		AVLNode* x1 = toBalance->left;
		AVLNode* T21 = x1->right;

		x1->right = toBalance;
		toBalance->left = T21;

		toBalance->height = greater(toBalance)+1;
		x1->height = greater(x1)+1;

		return x1;
	}
	//Case with Right Right
	else if(balanceFactor<-1 && strcmp(toBalance->right->element->word,word)<0){
		AVLNode* x = toBalance->right;
		AVLNode* T2 = x->left;

		x->left = toBalance;
		toBalance->right = T2;

		toBalance->height = greater(toBalance)+1;		
		x->height = greater(x)+1;

		return x;
	}
	//Case with Right Left
	else if(balanceFactor<-1 && strcmp(toBalance->right->element->word,word)>0){
		AVLNode* x = toBalance->right->left;
		AVLNode* T2 = x->right;

		toBalance->right->left = T2;		
		x->right = toBalance->right;

		toBalance->right->height = greater(toBalance->right)+1;			
		x->height = greater(x)+1;			

		toBalance->right = x;

		AVLNode* x1 = toBalance->right;
		AVLNode* T21 = x1->left;

		toBalance->right = T21;
		x1->left = toBalance;
		
		toBalance->height = greater(toBalance)+1;
		x1->height = greater(x1)+1;

		return x1;
	}

	return toBalance;	
	
}

/**[private method]
returns height that is greater between the children Nodes of the node passed in, accounts for if node passed in is NULL*/
static int greater(AVLNode* parent){
	int testLeft = height(parent->left);
	int testRight = height(parent->right);

	if(testLeft >= testRight)
		return testLeft;
	return testRight;
}

/**[private method]
returns height of a node in order to account for if the node is NULL or not*/
static int height(AVLNode* findFor){
	if(findFor == NULL)
		return 0;
	return findFor->height;
}


/**
gets number of nodes in an AVL Frequency Tree
**/
int sizeOfTree(AVLNode* root){
	if(root==NULL)
		return 0;
		
	return 1+sizeOfTree(root->left)+sizeOfTree(root->right);
}



//TREENODE methods/////////////////////////////////////////////////////////////////

/**
Initializes a TreeNode structure and returns a pointer to the created TreeNode
**/
TreeNode* createTreeNode(WordFreq* element){
	TreeNode* ret = (TreeNode*)malloc(sizeof(TreeNode));
	ret->element = element;
	ret->left = NULL;
	ret->right = NULL;
	return ret;
}


/**
merges two trees into one and returns the root (the combined frequency)
Note: the root->element->word=NULL (because it only represents a frequency!)
**/
TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2){ //TODO
	if(t1==NULL||t2==NULL||t1->element==NULL||t2->element==NULL){
		PRINT_ERROR("cannot pass in null TreeNodes or uninitialized TreeNodes into mergeTrees()");
		return NULL;
	}
	WordFreq* root_wordf = createWordFreq(NULL, (t1->element->frequency + t2->element->frequency) );
	TreeNode* root = createTreeNode(root_wordf);
	
	root->left = t1;
	root->right = t2;
	return root;
}


/**
frees all nodes in a Tree. PostOrder Traversal.
Note: DOES NOT free the WordFreq element
**/
void freeTreeOnly(TreeNode* root){
	if(root==NULL) return;
	
	freeTreeOnly(root->left);
	freeTreeOnly(root->right);
	free(root);
}


/**
frees all nodes in a Tree. PostOrder Traversal.
Note: Frees WordFreq AND its string. Be careful if you want to use the String for further use.
**/
void freeTreeAndWF(TreeNode* root){
	if(root==NULL) return;
	
	freeTreeAndWF(root->left);
	freeTreeAndWF(root->right);
	
	freeWordFreq(root->element);
	free(root);
}



//TREEQUEUE methods////////////////////////////////////////////////////////////////

/**[private method]
Initializes a QueueItem given an existing tree and returns a pointer to that QueueItem
**/
static QueueItem* createQueueItem(TreeNode* tree){
	QueueItem* ret = (QueueItem*)malloc(sizeof(QueueItem));
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
	if(q==NULL||(q->front) ==NULL||(q->end) ==NULL){
		PRINT_ERROR("passed in NULL Queue into dequeue");
		return NULL;
	}
	
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
free's all Queue nodes if necessary (does not touch the trees in case of future use)
however, if implemented correctly, FileCompression.c will not need to call upon this method
**/
void freeQueue(Queue q){
	if((q.front)==NULL) return;
	
	QueueItem* ptr = (q.front);
	while(ptr!=NULL){
		QueueItem* temp = ptr;
		ptr = ptr->next;
		free(temp);
	}
}



//MINHEAP methods//////////////////////

/**
creates a MinHeap from an AVL Frequency Tree 
@params: tree - AVL tree root that keeps track of frequencies of each word 
@ret : a MinHeap w/ an initialized heap array, or if passed in tree is faulty, a Minheap with a NULL array and size 0
**/
MinHeap createMinHeap(AVLNode* tree){ 
	MinHeap ret = {NULL,0};
	
	if(tree==NULL){
		PRINT_ERROR("cannot create a heap from a NULL tree");
		return ret;
	}
	
	ret.size = sizeOfTree(tree);
	ret.heapArr = (WordFreq**)malloc( (ret.size) * sizeof(WordFreq*) ); //creates array big enough to hold all elements in AVLTree
	initializeMinHeapArr(tree, ret.heapArr, 0); //inserts each node of AVLtree into heapArr	
	heapify(&ret); //turns array into a heap
		
	return ret;
}


/**[private method]
Serializes an AVL tree into an array.
Traverses through an AVL tree and inserts each WordFreq element into the heapArr
@params: i is the current index of the next element in heapArr (updated recursively)
@returns: an int to keep track of the index
**/
static int initializeMinHeapArr(AVLNode* node, WordFreq** heapArr, int i){ 	
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
static void siftDown(WordFreq** heapArr, int size, int ind){ 
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
Swaps two WordFreq pointers given the addresses of the two pointers
**/
static void swap(WordFreq** element1, WordFreq** element2){
	WordFreq* temp = *element1;
	*element1 = *element2;
	*element2 = temp;
}


/**
returns from top of the heap and then updates the heap (sifts up)
**/
WordFreq* removeMin(MinHeap* heap){ 
	if(heap==NULL||heap->size==0){
		PRINT_ERROR("cannot remove min from empty or null heap");
		return NULL;
	}
	
	WordFreq* min= heap->heapArr[0];
	swap(&(heap->heapArr[0]), &(heap->heapArr[heap->size-1])); //swaps last element to the top
	heap->heapArr[heap->size-1]=NULL; //removes element from heapArray
	heap->size--; //decreases accesible elements in a heap
	if(heap->size!=0)siftDown(heap->heapArr,heap->size,0);	
	
	return min;
}



//PRINT methods/////////////////////////////////////////////////////////////

/**
prints out WordFreq
@params: s is any additional parameters to add to the string
**/
void printWordFreq(WordFreq* wf, char* s){
	if(wf==NULL){
		printf("NULL %s",s);
		return;
	}
	
	if(wf->word==NULL){
		printf("NULL:%d %s",wf->frequency,s);
	}else{
		printf("%s:%d %s",wf->word,wf->frequency,s);
	}
}

void printHeap(MinHeap heap){
	if(heap.heapArr==NULL){
		printf("NULL\n");
		return;
	}

	printf("/////////////////////////////////////////////////\n");
	printf("[HEAP:(horizontal)]\n");
	printHeapRec(heap,0,0);
	printf("/////////////////////////////////////////////////\n");
}

//[private method] recursively prints heap(horizontally)
static void printHeapRec(MinHeap heap, int root, int space){	
  	if (root>=heap.size)return;
  	 
  	int count = 10;
  	int i;
  	space += count; //increases space inbetween elements
  	
    printHeapRec(heap, 2*root+2, space); 
    
    printf("\n"); 
    for (i = count; i < space; i++){ printf(" ");} 
    printWordFreq(heap.heapArr[root],"\n");
    
 	printHeapRec(heap, 2*root+1, space);
}

//printsHeapArray for testing
void printHeapArray(WordFreq** arr, int size){
	int i;
	for(i=0; i<size; i++){
		printWordFreq(arr[i],"\t");
	}
	printf("\n");
}

void printTree(AVLNode* root){
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
static void printTreeRec(AVLNode* root, int space){ 
  	if (root == NULL)return;
  	 
  	int count = 10;
  	int i;
  	space += count; //increases space inbetween elements
  	
    printTreeRec(root->right, space); 
  
    printf("\n"); 
    for (i = count; i < space; i++){ printf(" ");} 
    printWordFreq(root->element,"\n");
    
    printTreeRec(root->left, space); 
}

void printQueue(Queue q){ 
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



///////////////////////////////////////////////////////////////


int main(){//TODO get rid of this in final prod	
	AVLNode** root = (AVLNode**)malloc(sizeof(AVLNode*));
	insertOrUpdateAVL(root,"Happy");
	insertOrUpdateAVL(root,"Happy");
	insertOrUpdateAVL(root,"Hot");
	insertOrUpdateAVL(root,"Happy");
	insertOrUpdateAVL(root,"Hog");
	insertOrUpdateAVL(root,"Happy");
	insertOrUpdateAVL(root,"Hogy");
	insertOrUpdateAVL(root,"Hot");
	insertOrUpdateAVL(root,"Hot");
	insertOrUpdateAVL(root,"apple");
	insertOrUpdateAVL(root,"top");
	insertOrUpdateAVL(root,"Apple");
	insertOrUpdateAVL(root,"Corn");
	insertOrUpdateAVL(root,"Apple");
	insertOrUpdateAVL(root,"Apple");
	
	printTree(*root);	
	return 0;
}
