#ifndef FILEC_H
#define FILEC_H

#include "structures.h"

	TreeNode* huffmancoding(AVLNode* frequencies);
	static TreeNode* pickMinTree(MinHeap* heap, Queue* q);

#endif
