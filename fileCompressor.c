#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 
#include "structures.h"

#define EXIT_ERROR(txt) do{PRINT_ERROR(txt);return 0;} while(0)


/**
Takes in a AVLTree of words and their frequencies in a file and performs huffman coding
returns a huffman tree to be used in encoding
**/
TreeNode* huffmancoding(AVLNode* frequencies){ //TODO
	if(frequencies==NULL){
		return NULL;
	}
	
	MinHeap leafs = createMinHeap(frequencies);
}


void buildcodebook(){ //TODO: add params and return

}


void compress(){ //TODO: add params and return
}


void decompress(){ //TODO: add params and return
}


/**
Runs a single flag operation.
Returns true if succesful, returns false if not.
**/
bool singleFlag(char* flag, char* path_file, char* codebook){ //TODO
	switch(flag){
		case 'b': //TODO
			break;
		case 'c': //TODO
			break;
		case 'd': //TODO
			break;
		default:
			return false;
	}
	return true;
}


int main(int argc, char** argv){
	//INPUT CHECKS
		if(argc!=3||argc!=4){
			EXIT_ERROR("Not the correct number of arguments"); 
		} 
		
	//Variables
		char* str_flg = argv[1]; 
		int length_flg = strlen(str_flg);
			
		char flag = str_flag[0];
		bool isRecursive =false;
		char* path_file = argv[2];
		char* codebook = (argc==4)? argv[3]:NULL; //if codebook was passed in as argument	
			
	/*checking length of str_flag*/
		if(length_flg>2){
			EXIT_ERROR("flag must be one to two characters");
		}else if(length_flg==2){
			if(str_flag[1]=='R'){
				isRecursive=true;
			}else{
				EXIT_ERROR("second char in flag can only be 'R'");
			}
		}
		
	return 0;
}
