#ifndef FILEC_H
#define FILEC_H

#include "structures.h"

	TreeNode* huffmancoding(AVLNode* frequencies);
	static TreeNode* pickMinTree(MinHeap* heap, Queue* q);
	
	static char* combinedPath(char* path, char* file);
	int typeDirent(char* path);
	
	bool runFlag(char* path_file);
	void Recursive(char* path_file);

#endif
