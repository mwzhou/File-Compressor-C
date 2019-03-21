#ifndef FILEC_H
#define FILEC_H

#include "structures.h"

	TreeNode* huffmancoding(AVLNode* frequencies);
	static TreeNode* pickMinTree(MinHeap* heap, Queue* q);

	static char* combinedPath(char* path_name, char* file_name);
	int typeStat(char* pathfile_name);
	char* readFile(char* file_name);

	bool runFlag(char* path_file);
	void Recursive(char* path_file);

#endif
