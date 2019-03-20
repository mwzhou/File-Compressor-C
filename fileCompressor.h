#ifndef FILEC_H
#define FILEC_H

#include "structures.h"

	TreeNode* huffmancoding(AVLNode* frequencies);
	static TreeNode* pickMinTree(MinHeap* heap, Queue* q);
	
	static char* combinedPath(char* path, char* file);
	int typePath(struct dirent* dp, DIR* curr_dir);
	
	bool runFlag(char* path_file);
	void Recursive(char* path_file);

#endif
