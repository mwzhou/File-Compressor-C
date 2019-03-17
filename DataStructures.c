/**
DataStructures.c holds all the structures necessary to be used in FileCompression.c
**/
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h> 
#include "DataStructures.h"


//WORDFREQ methods/////////////////////////////////////////////////////////////////

/**
Initializes a WordFreq pointer to a word frequency object because it will be passed around in many structs.
To avoid time spent copying, WordFreq is represented by a pointer
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

//AVLNode methods/////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes an AVLNode structure
@params - the word (the frequency is always 1 at first)
@ret - initialized AVLNode pointer to created AVLNode.
**/
AVLNode* createAVLNode(char* word){
	AVLNode* ret = (AVLNode*)malloc(sizeof(AVLNode));
	ret->element = createWordFreq(word, 1);
	ret->left = NULL;
	ret->right = NULL;
	return ret;
}


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
	
	//1. search for word
	//2. if found - update frequency and return
	//3. if not found - add to Tree, then balance
}


/**
**/
void balanceAVL(/*insert parameters here*/){ //TODO
	//REMEMBER to add this into the header file, or else it won't compile
}


/**
gets number of nodes in an AVL Frequency Tree
**/
int sizeOfAVL(AVLNode* root){
	if(root==NULL)
		return 0;
		
	return 1+sizeOfAVL(root->left)+sizeOfAVL(root->right);
}


/**
free's all nodes in an AVLTree
Note: DOES NOT free the WordFreq element
**/
void freeAVLNode(AVLNode* root){
	if(root==NULL) return;
	
	freeAVLNode(root->left);
	freeAVLNode(root->right);
	free(root);
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
free's all nodes in a Tree
Note: Frees WordFreq AND its string. Be careful if you want to use the String for further use.
**/
void freeTreeNode(TreeNode* root){
	if(root==NULL) return;
	
	freeTreeNode(root->left);
	freeTreeNode(root->right);
	
	freeWordFreq(root->element);
	free(root);
}



//TREEQUEUE methods////////////////////////////////////////////////////////////////

/**
Initializes a QueueItem given an existing tree and returns a pointer to that QueueItem
**/
QueueItem* createQueueItem(TreeNode* tree){
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
	if(q==NULL||(q->end)==NULL||(q->front) == NULL){ //Queue has no elements yet
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
free's all Queue nodes if necessary (does not touch the trees because it is necessary for future use)
however, if implemented correctly, FileCompression.c will not need to call upon this method
**/
void freeQueue(Queue* q){
	if(q==NULL||(q->end)==NULL||(q->front)==NULL) return;
	
	QueueItem* ptr = (q->front);
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
	
	ret.size = sizeOfAVL(tree);
	ret.heapArr = (WordFreq**)malloc( (ret.size) * sizeof(WordFreq*) ); //creates array with sizeOfAVL(tree) size
	initializeMinHeapArr(tree, ret.heapArr, 0); //inserts each node of AVLtree into heapArr	
	heapify(&ret); //turns array into a heap
		
	return ret;
}


/**
Serializes an AVL tree into an array.
Traverses through an AVL tree and inserts each WordFreq element into the heapArr
@params: i is the current index of the next element in heapArr (updated recursively)
@returns: an int to keep track of the index
**/
int initializeMinHeapArr(AVLNode* node, WordFreq** heapArr, int i){ 	
	if(node->left != NULL)
		i= initializeMinHeapArr(node->left, heapArr, i);
	heapArr[i++]= node->element;
	if(node->right != NULL)
		i = initializeMinHeapArr(node->right, heapArr, i);
	
	return i;
}


/**
linear heapify
takes initialized MinHeap pointer and heapifies it with the O(n) time algorithm. Starts from first non-leaf node and sifts-down
**/
void heapify(MinHeap* heap){ 
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


/**
sift-down algorithm for heaps
given an index in a heapArray, siftDown() checks if all nodes in the subtree are smaller than that element
performs the necessary swaps to mantain a minheap structure
**/
void siftDown(WordFreq** heapArr, int size, int ind){ 
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


/**
Swaps two WordFreq pointers given a pointer to the original pointer
**/
void swap(WordFreq** element1, WordFreq** element2){
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


void printHeap(MinHeap heap){	
	if(heap.size==0||heap.heapArr==NULL){
		printf("\nHEAP:\nEMPTY\n\n");
		return;
	}

	printf("\nHEAP:\n");	
	int i;
	for(i=0; i<(heap.size); i++){
		int level = 1<<i;//current level
		
		int j = 0;
		for(j=0; j<level && (j+level)<=(heap.size); j++){ //prints children on current level
			int j_ind = j+level-1;
			PRINT_WORDFREQ(heap.heapArr[j_ind], "	");
		}
		printf("\n");
	}
}


void printHeapArray(WordFreq** arr, int size){
	int i;
	for(i=0; i<size; i++){
		PRINT_WORDFREQ(arr[i],"  ");
	}
	printf("\n");
}


void printTree(TreeNode* t){ //TODO printTree

}


void printQueue(Queue q){
	if(q.end==NULL||q.front==NULL){
		printf("\nNULL\n\n");
		return;
	}
	
	printf("\nQUEUE:\n");
	QueueItem* ptr = q.front;
	while(ptr!=NULL){
		PRINT_WORDFREQ(ptr->tree->element, ""); //TODO: print entire tree
		
		if(ptr->prev==NULL) printf("\tfront");
		if(ptr->next==NULL) printf("\tend");
		printf("\n");
		
		ptr = ptr->next;
	}
	printf("\n");
}

///////////////////////////////////////////////////////////////


int main(){	//TODO get rid of this in final prod
	WordFreq* wf = createWordFreq("hi",2);
	Queue q = {NULL,NULL};
	enqueue(&q, createTreeNode(wf));
	enqueue(&q, createTreeNode(createWordFreq("hio",3)));
	enqueue(&q, createTreeNode(wf));
	enqueue(&q, createTreeNode(createWordFreq("hig",5)));
	printQueue(q);
	
	dequeue(&q);
	printQueue(q);
	dequeue(&q);
	printQueue(q);
	dequeue(&q);
	printQueue(q);
	dequeue(&q);
	printQueue(q);
	dequeue(&q);
	
	
	AVLNode* root = createAVLNode("hello");
	root->left = createAVLNode("hellol");
	root->right = createAVLNode("hellor");
	root->right->left = createAVLNode("righton");
	root->left->left = createAVLNode("nahh");
	root->left->left->left = createAVLNode("wog");
	root->left->element->frequency = 0; //ERROR
	root->right->element->frequency = 3;
	root->left->left->element->frequency = 10;
	root->left->left->left->element->frequency = 7;
	root->right->left->element->frequency=4;
	
	WordFreq** arr = (WordFreq**)malloc(sizeOfAVL(root)*sizeof(WordFreq*));
	initializeMinHeapArr(root, arr, 0);
	printHeapArray(arr,sizeOfAVL(root));

	MinHeap heap = createMinHeap(root);
	
	printHeap(heap);
	removeMin(&heap);
	printHeap(heap);
	removeMin(&heap);
	printHeap(heap);
	removeMin(&heap);
	printHeap(heap);
	removeMin(&heap);
	printHeap(heap);
	removeMin(&heap);
	printHeap(heap);
	removeMin(&heap);
	printHeap(heap);	
	return 0;
}


