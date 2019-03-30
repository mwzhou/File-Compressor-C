#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<sys/types.h>

#include "fileCompressor.h"

//GLOBALS: items passed through the terminal

	//flag passed in, will be initialized to flag
	char flag ='\0';

	//if'-R' flag passed in, will be initialized to true
	bool isRecursive = false;

	//file/path name passed in, will be initialized to file/path name
	char* orig_pathfile = NULL;



//BUILD_CODEBOOK methods////////////////////////////////////////////

/**
builds codebook in current directory called HuffmanCodebook
**/
void buildcodebook(char* file_name){
	if(file_name==NULL){ pEXIT_ERROR("passed in NULL file_name into buildcodebook"); }

	//build Huffman Tree
		TreeNode* huffman_tree = huffmancoding( buildFrequencyAVL(file_name) );	//Note: AVLtree is freed in huffmancoding
			if(huffman_tree==NULL) return; //Note: huffmancoding method already outputs errors

	//Create File:
		//get path of huffmancodebook:
		char* cb_path = getNewFilePath(file_name, "HuffmanCodebook");

		//opening file:
		int codebook = open(cb_path, O_WRONLY|O_CREAT|O_TRUNC, 0644); //creates a new file, overwrites old HuffmanCoding File if exists
			if(codebook < 0){ pRETURN_ERRORvoid("tried to open HuffmanCodebook"); }
			free(cb_path);

	//FIND ENCODINGS AND WRITE TO CODEBOOK
		WRITE_AND_CHECKv( codebook, "\\\n", 2); //writes starting line, and then checks if unsuccesful
		writeEncodings(codebook, huffman_tree, NULL); //finds and writes encodings of each token into file
		WRITE_AND_CHECKv( codebook, "\n", 1); //writes terminating line, and then checks if unsuccesful

	//FREEING and CLOSING
		close(codebook);
		freeTreeAndTok(huffman_tree); //frees all TreeNodes AND also frees the encoding and token of each node
}


/**
goes through the Huffman Tree passed in and changes each node's frequency into the encoding for that word.
Adds each NON-NULL encoding for a NON-NULL token into the codebook
**/
static void writeEncodings(int codebook, TreeNode* root, char* encoding){ //TODO: check if codebook has to be certain order
	if( root == NULL )
		return;

	//WRITE ROOT'S ENCODING TO THE CODEBOOK AND UPDATES ROOT
		//update root's element and swaps the frequency member for an encoding member so that it can be freed later in freeTreeAndTok()
		(root->element)->encoding = encoding;
		(root->element)->hasFrequency=false;

		//WRITE encoding and tok if not NULL into codebook in the format specified by Assignment. (Checks for write() errors after each write)
		if( (root->element)->tok!= NULL && encoding!=NULL){
			WRITE_AND_CHECKv( codebook, (root->element)->encoding , strlen((root->element)->encoding) );//write the encoding into codebook
			WRITE_AND_CHECKv( codebook, "\t" , 1 );
			WRITE_AND_CHECKv( codebook, (root->element)->tok , strlen((root->element)->tok) );//write the token into codebook
			WRITE_AND_CHECKv( codebook, "\n" , 1 );
		}

	//UPDATE new encoding by adding a 0 or a 1 to the current encoding, then RECURSE
	writeEncodings( codebook, root->left, appendCharToString(encoding, '0') );
	writeEncodings( codebook, root->right, appendCharToString(encoding, '1') );
}



/**
goes through a file, tokenizes it, and gets the frequency of each token
@returns AVLTree* based on frquencies of each element
 returns NULL is file wasn't passed in, or there were no tokens
**/
static AVLNode* buildFrequencyAVL(char* file_name){
	if(file_name == NULL ){ pRETURN_ERROR("cannot pass in NULL file_name into buildFrequencyAVL()", NULL); }

	//DECLARE VARIABLES
		char* fstr = readFile(file_name); //file read into a string
		AVLNode* freq_tree = NULL;

	//TOKENIZES fstr INTO A TOKEN AND A DELIM TOKEN - loops through fstr until there are no more delimiters
		char* fstr_ptr = fstr; //pointer to front of string for manipulation later
		int indOfDELIM = 0; //index to keep track of the first instance of a delimiter in fstr_ptr

		while( (indOfDELIM = strcspn(fstr_ptr , DELIMS) ) < strlen(fstr_ptr) ){ //updates index of delimiter and stops once there are no more delimiters
			//TOKEN BEFORE DELIM:
				if(indOfDELIM!=0){ //if there is a token b4 the delimiter
					//finding the token
					char* tokB4 = (char*)malloc( indOfDELIM + 1 );
						if(tokB4 ==NULL){ pEXIT_ERROR("malloc"); }
						memcpy(tokB4, fstr_ptr , indOfDELIM);//copy (indOfDELIM) number of characters from fstr into tok
						tokB4[indOfDELIM] = '\0'; //string terminator

					//insert tokB4 into AVL, frees tokB4 if insertOrUpdateAVL() did an Update
					if( insertOrUpdateAVL(&freq_tree, tokB4) == UPDATED )
							free(tokB4);
				}


			//TOKEN OF DELIM:
				//finding the token
				char* tokDELIM = getStringRepOfDELIM(fstr_ptr[indOfDELIM]);
					if(tokDELIM==NULL){ free(fstr); free(freq_tree); pRETURN_ERROR("did not enndOfDELIM = strcspn(fstr_ptr , DELIMS) )ter in delimiter", NULL); }

				//inserting
				insertOrUpdateAVL(&freq_tree, tokDELIM); //insert string (is constant string so don't need to free)


			//UPDATE fstr_ptrency_tree==NULL){ pRETURN_ERROR("passe
				fstr_ptr  += ( indOfDELIM + 1 ); //update pointer of fstr_ptr to after current tokens
		}

	free(fstr);
	return freq_tree;
}



/**
Takes in a AVLTree of tokss and their frequencies in a file and performs huffman coding
returns a huffman tree to be used in encoding
FREES AVL TREE AFTER USE
**/
static TreeNode* huffmancoding(AVLNode* frequency_tree){
	if(frequency_tree==NULL){ pRETURN_ERROR("passed in NULL frequency tree", NULL); }


	if( frequency_tree->left==NULL && frequency_tree->right==NULL ){ //if there is only one token: create a tree and return it
		TreeNode* ret = createTreeNode(  createTokenInt(NULL, frequency_tree->element->frequency) );
		ret->left = createTreeNode(  frequency_tree->element );
		free(frequency_tree);
		return ret;
	}

	//Variables
	MinHeap tokens = buildMinHeap(frequency_tree);
		freeAVLTree(frequency_tree); //do not need frequencies anymore, free AVLNodes (not the element though)
	Queue trees = {NULL,NULL};


	//continuously merge the two min elements from the heap and queue until we get one merged huffman tree
	//takes the smaller item in the heap/queue and merges the two minimums, then enqueues the tree onto the queue
	while( !(tokens.size==0 && hasSizeOne(&trees)) ){ //stops when heap is empty and only one element left in tree
		TreeNode* t1 = pickMinTreeHuffman(&tokens, &trees);
		TreeNode* t2 = pickMinTreeHuffman(&tokens, &trees);
		enqueue(&trees, mergeTrees(t1,t2));
	}

	free(tokens.heapArr);
	return dequeue(&trees);
}




//COMPRESS METHODS////////////////////////////////////////////

/**
compresses file_name given and writes <file_name>.hcz into the same directory
Note: checks on file_name have already been done in inputCheck() and runFlag()
**/
void compress( char* file_name ){
	char* codebook_name = getNewFilePath(file_name, "HuffmanCodebook"); //codebook in current directory
	if(typeOfFile(codebook_name) != isREG ){ printf("%s\n", file_name); pRETURN_ERRORvoid("codebook doesn't exist"); }
	if( endsWithHCZ(file_name)) { pRETURN_ERRORvoid("file can't end with hcz in compress"); }

	//tree to search up tokens quickly and get an encoding
		CodeNode* codebook_tree = buildCodebookTree( codebook_name, cmpByTokens);

	//file contents of fname read into fstr (readFile is a fileHelperMethods.c method)
		char* fstr = readFile(file_name);

	//compressed file to write to:
		char* fcompr_path_name = getNewExtensionAndPath( file_name, ".hcz" ); //replaces extension with .hcz and gets string

		int fcompr = open(fcompr_path_name , O_WRONLY| O_CREAT|O_TRUNC, 0644); //creates a new file called "fcomp_name" in directory, overwrites old file if exists
			if(fcompr < 0){ pRETURN_ERRORvoid("tried to open()"); }
			free(fcompr_path_name);




	//TOKENIZE AND WRITE TO FILE:
	//TOKENIZES fstr INTO A TOKEN AND A DELIM TOKEN - loops through fstr until there are no more delimiters
		char* fstr_ptr = fstr; //pointer to front of string for manipulation later
		int indOfDELIM = 0; //index to keep track of the first instance of a delimiter in fstr_ptr

		while( (indOfDELIM = strcspn(fstr_ptr , DELIMS) ) < strlen(fstr_ptr) ){ //updates index of delimiter and stops once there are no more delimiters

			//TOKEN BEFORE DELIM:
				if(indOfDELIM!=0){ //if there is a token b4 the delimiter
					//finding the token (mallocs and copies)
					char* tok1 = (char*)malloc( indOfDELIM + 1 );
						if(tok1 ==NULL){ pEXIT_ERROR("malloc"); }
						tok1[indOfDELIM] = '\0'; //string terminator
						memcpy(tok1, fstr_ptr , indOfDELIM);//copy (indOfDELIM) number of characters from fstr into tok


					//finds encoding associated with tok1 and writes in fcompr
					char* tok1_encoding = getCodeItem( codebook_tree, tok1, cmpByTokens);
						free(tok1);

					if( tok1_encoding == NULL){ //if tok1 doesn't exist in codebook
							free(fstr); close(fcompr); freeCodeTreeAndTok( codebook_tree );
							printf("token1: %s\n", tok1);
							pRETURN_ERRORvoid("token doesn't exist in codebook");
					}else{
							WRITE_AND_CHECKv( fcompr , tok1_encoding , strlen(tok1_encoding) ); //writes encoding to file
					}

				}

			//TOKEN OF DELIM:
				//finding the token
				char* tok2 = getStringRepOfDELIM(fstr_ptr[indOfDELIM]);
					if(tok2==NULL){ pEXIT_ERROR("did not enter in delimiter"); }

				//finds encoding associated with tok2 and writes in fcompr
					char* tok2_encoding = getCodeItem( codebook_tree, tok2, cmpByTokens); //finds token

					if( tok2_encoding == NULL){ //if tok2 doesn't exist in codebook
							free(fstr); close(fcompr); freeCodeTreeAndTok( codebook_tree );
							pRETURN_ERRORvoid("token doesn't exist in codebook");
					}else{
							WRITE_AND_CHECKv( fcompr , tok2_encoding , strlen(tok2_encoding) ); //writes encoding to file
					}

			//UPDATE fstr_ptr
				fstr_ptr  += ( indOfDELIM + 1 ); //update pointer of fstr_ptr to after current tokens
		}


	free(fstr);
	free(codebook_name);
	close(fcompr);
	freeCodeTreeAndTok( codebook_tree );
}




//DECOMPRESS methods////////////////////////////////////////////

/**
decompresses given file_name.hcz given and writes decompressed file_name into the same directory
Note: checks on file_name have already been done in inputCheck() and runFlag()
**/
void decompress( char* file_name ){
	char* codebook_name = getNewFilePath(file_name, "HuffmanCodebook"); //codebook in current directory
	if(typeOfFile(codebook_name) != isREG ){ printf("%s\n", file_name); pRETURN_ERRORvoid("codebook doesn't exist"); }
	if( !endsWithHCZ(file_name) ) { pRETURN_ERRORvoid("file must end with .hcz"); }

	//tree to search up encodings quickly and get a token
	CodeNode* codebook_tree = buildCodebookTree( codebook_name, cmpByEncodings);


	//file contents of file_name read into fstr_hcz (readFile is a fileHelperMethods.c method)
	char* fstr_hcz = readFile(file_name);

	//decompressed file to write to:
	char* fdec_path_name = getNewExtensionAndPath(file_name, NULL); //removes .hcz extension

	int fdecompr = open(fdec_path_name , O_WRONLY|O_CREAT|O_TRUNC, 0644); //creates a new file in directory, overwrites old file if exists
		if(fdecompr < 0){ pRETURN_ERRORvoid("tried to open()"); }
		free(fdec_path_name);


	//loops through each encoding of fstr_hcz, tries to find each substring in codebook_tree. If not found, adds another "letter" to the substring and tries to find. If found, starts again from ending index of found substring.
	int index = 0;
	int length = 1;
	while(index<strlen(fstr_hcz)){
		char* find = substr(fstr_hcz,index,length+1);
		char* found = getCodeItem(codebook_tree, find, cmpByEncodings);

		//if substring not found, add another to ending index of substring
		if (found==NULL){
			length+=1;
		//if found, increase starting index by length of found, reset ending index to start index+1
		}else{
			//account for if encoding is Deliminator. If it is, write in the corresponding eliminator, not what is stored
			if(isDELIMStr(found)){
				char* insert = getCharRepOfDELIM(found);
				if(insert == NULL)
					pEXIT_ERROR("This is not a delimiter");
				WRITE_AND_CHECKv(fdecompr, insert, strlen(insert));
			}

			//if not deliminator, write it into file.
			else
				WRITE_AND_CHECKv(fdecompr, found, strlen(found));

			index += length;
			length = 1;
		}
		free(find);
	}
	free(fstr_hcz);
	free(codebook_name);
	close(fdecompr);
	freeCodeTreeAndTok( codebook_tree );
}




//RECURSE methods////////////////////////////////////////////

/**
Runs the flag multiple times in all subdirectories of a given path. Checks on input parameter path have already been done in inputCheck;
**/
void recurse(char* path){
	//DIRECTORY VARIABLES
	DIR* curr_dir = opendir(path);
		if(curr_dir==NULL){ pEXIT_ERROR("opendir()"); }
	struct dirent* dp;

	//TRAVERSES THROUGH CURRENT DIRECTORY TO FIND ALL SUBDIRECTORIES
	while((dp = readdir(curr_dir))!=NULL){
		if(strcmp(dp->d_name, ".")==0 || strcmp(dp->d_name, "..")==0) //if current/parent directory(ignore)
			continue;

		//Checks type of dp and combines filepath (frees after entering the directory)
		char* new_path = combinedPath(path,dp->d_name);
		FileType type = typeOfFile(new_path); //file type of new_path


		//RECURSE if Sub-Directory, RUN-FLAG, if not
		if( type == isDIR ){ //new_path is a directory
			recurse(new_path);
		}else if( type == isREG ){ //new_path is a regular file
			if( (flag=='c' && !endsWithHCZ(new_path)) || (flag=='d' && endsWithHCZ(new_path)) || flag=='b')
				runFlag(new_path); //Note: flag only runs if the file meets the requirment for each respective flag in runFlag()
		}

		free(new_path);
	}

	closedir(curr_dir);
}




//MISC methods//////////////////////////////////////////////


/**
Runs a single flag operation on file_name given. (note: must be a regular file!)
Returns true if succesful, returns false if not.
**/
bool runFlag(char* file_name){
	//Error Checks
	if(file_name==NULL){ pEXIT_ERROR("path_file NULL");}
	if( typeOfFile(file_name)!= isREG ){ pRETURN_ERROR("must input a REG FILE into runFlag()", false); }
	if ( sizeOfFile(file_name)== 0){ printf("Empty File: %s\n", file_name); return false; }

	 if( strstr(file_name, "HuffmanCodebook") != NULL ){ //if Codebook, ignore
		 return false;
	 }

	switch(flag){
		case 'b':
			//if file sent in is not a HuffmanCodebook creates by b
				buildcodebook( file_name );
			break;
		case 'c':
				compress ( file_name );
			break;
		case 'd':
				decompress ( file_name );
			break;
		default:
			pEXIT_ERROR("flag must be 'b', 'c', or 'd'");
	}

	return true;
}


/**
checks inputs of items passed through terminal and initializes globals
exits program if arguments do not match the requirements
**/
bool inputCheck(int argc, char** argv){
	if(argc<3 || argc>5){ pEXIT_ERROR("Must pass inbetween 2 to 4 arguments in addition to the executable"); }

	//LOOPING THROUGH EACH ARGUMENT (excluding the first argument: the executable)
	int i;
	for(i=1; i<argc; i++){
		char* s = argv[i];

		//CHECK IF FLAG
		if(strlen(s) == 2 && s[0]=='-'){

			//if arg is a regular flag
			if( s[1]=='b'||s[1]=='c'||s[1]=='d' ){
				if(flag!='\0'){ pEXIT_ERROR("cannot have multiple flags"); } //already came across a flag
				flag = s[1];
			//if argc is a recursive flag
			}else if( s[1]=='R' ){
				if(isRecursive){ pEXIT_ERROR("cannot have multiple '-R' flags"); } //already came across '-R' flag
				isRecursive = true;
			}

		//CHECK IF IS PATH/FILE and that it exists
		}else{
			if( typeOfFile(s) == isUNDEF ) { pEXIT_ERROR("undefined file name"); } //checking if PATH/FILE exists , if not, exit

			if(strstr(s, "HuffmanCodebook") == NULL){ //not codebook
				if(orig_pathfile!=NULL){ pEXIT_ERROR("can only have at most one file/path"); }  //orig_pathfile already initialized
				orig_pathfile = realpath(s, NULL); //gets real path
			}


		}



	}

	//CHECK if all necessary globals have been initialized
		if(flag =='\0'){ pEXIT_ERROR("must specify a flag as an argument"); }
		if( orig_pathfile == NULL ){ pEXIT_ERROR("must give in a path or a file as an argument"); }

	//CHECK IF ARGUMENTS MATCH FLAG
		if(isRecursive && typeOfFile(orig_pathfile) != isDIR ){ isRecursive=false; printf("passed in -R flag but only passed in a REGULAR file, will run flag as if only one iteration"); } //'-R' flag called but path not handed in
		else if( !isRecursive && typeOfFile(orig_pathfile) != isREG ){ pEXIT_ERROR ( "must pass in a REGULAR FILE if not calling flag '-R'"); }//'-R' flag not called, but file is not a regular file

	return true;
}


int main(int argc, char** argv){

	//CHECKS ARGUMENTS PASSED IN THROUGH TERMINAL
		if(!inputCheck(argc, argv))
			return 0;

	//Running the respective flag operation
		if(isRecursive){ //recursive
			recurse( orig_pathfile);
		}else{
			runFlag(orig_pathfile);
		}

	return 0;
}
