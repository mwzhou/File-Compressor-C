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

	//file/path name passed in, will be initialized to file/path name passed in as arg
	char* orig_pathfile = NULL;

	//codebook name, will be initializzed to codebook name passed in as arg
	char* codebook_name = NULL;
	
	//TREE STRUCTURE FOR ALL FLAGS
		//if flag is b - tree is a frequency tree, keeps track of tokens
		//if flag is c - tree is a codebook search tree to quickly search up tokens and get encodings
		//if flag is d - tree is a codebook search tree to quickly search up encodings and get tokens
	CodeNode* tree = NULL;



//BUILD_CODEBOOK methods////////////////////////////////////////////

/**
builds codebook in current directory called HuffmanCodebook
**/
void buildcodebook(char* file_name){
	if(file_name==NULL){ pEXIT_ERROR("NULL file name passed in"); }
	
	//Create File Of Codebook:
		//opening file:
		int codebook = openFileW("./HuffmanCodebook");
			if(codebook<0){ pRETURN_ERRORvoid("open write"); } //if error opening file
		
	//Build Huffman Tree
		TreeNode* huffman_tree = huffmancoding( tree );//Note: AVLtree is freed in huffmancoding
					
	//FIND ENCODINGS AND WRITE TO CODEBOOK
		WRITE_AND_CHECKv( codebook, "\\\n", 2); //writes starting line, and then checks if unsuccesful
		writeEncodings(codebook, huffman_tree, NULL); //finds and writes encodings of each token into file
		WRITE_AND_CHECKv( codebook, "\n", 1); //writes terminating line, and then checks if unsuccesful

	//FREEING and CLOSING
		close(codebook);
		if( huffman_tree != NULL ) freeTreeAndTok(huffman_tree); //frees all TreeNodes AND also frees the encoding and token of each node
}


/**
goes through the Huffman Tree passed in and changes each node's frequency into the encoding for that word.
Adds each NON-NULL encoding for a NON-NULL token into the codebook
the encoding is recursively updated depending on how the tree was traversed
**/
static void writeEncodings(int codebook, TreeNode* root, char* encoding){ 
	if( root == NULL )
		return;

	//WRITE ROOT'S ENCODING TO THE CODEBOOK AND UPDATES ROOT
		
		//update root's element and swaps the frequency member for an encoding member so that it can be freed later with freeTreeAndTok()
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
goes through a file, tokenizes it, and gets the frequency of each token, then adds it to the tree
@params: file_name, and a pointer to an AVL tree where tokens are stored
**/
static void buildFrequencyAVL(char* file_name){
	if(file_name == NULL ){ pRETURN_ERRORvoid("cannot pass in NULL file_name"); }
	
	//READ FILE
		char* fstr = readFile(file_name); //file read into a string
			if(fstr==NULL){ pRETURN_ERRORvoid("error reading file name"); }
		
	//edge case: file has no delimiters
		if( strcspn(fstr , DELIMS) >= strlen(fstr) ){
			insertOrUpdateAVL(&tree, fstr);
			return;
		}
			
	//TOKENIZES fstr INTO A TOKEN AND A DELIM TOKEN - loops through fstr until there are no more delimiters
		char* fstr_ptr = fstr; //pointer to front of string for manipulation later
		int indOfDELIM = 0; //index to keep track of the first instance of a delimiter in fstr_ptr
	
		while( (indOfDELIM = strcspn(fstr_ptr , DELIMS) ) < strlen(fstr_ptr) ){ //updates index of delimiter and stops once there are no more delimiters
			
			//TOKEN BEFORE DELIM:
				if(indOfDELIM!=0){ //if there is a token b4 the delimiter
					//finding the token
					char* tokB4 = (char*)malloc( indOfDELIM + 1 );
						if(tokB4 == NULL){ pEXIT_ERROR("malloc"); }
						memcpy(tokB4, fstr_ptr , indOfDELIM);//copy (indOfDELIM) number of characters from fstr into tok
						tokB4[indOfDELIM] = '\0'; //string terminator

					//insert tokB4 into AVL, frees tokB4 if insertOrUpdateAVL() did an Update
					if( insertOrUpdateAVL(&tree, tokB4) == UPDATED )
						free(tokB4);
				}


			//TOKEN OF DELIM:
				//finding the token
				char* tokDELIM = getStringRepOfDELIM(fstr_ptr[indOfDELIM]);
					if(tokDELIM==NULL){ pEXIT_ERROR("did not enter in valid delimiter"); }

				//inserting
				insertOrUpdateAVL(&tree, tokDELIM); //insert string (is constant string so don't need to free)


			//UPDATE fstr_ptr
				fstr_ptr  += ( indOfDELIM + 1 ); //update pointer of fstr_ptr to after current tokens
		}

	free(fstr);
}



/**
Takes in a AVLTree of tokss and their frequencies in a file and performs huffman coding
returns a huffman tree to be used in encoding
FREES AVL TREE AFTER USE
**/
static TreeNode* huffmancoding(){
	if(tree==NULL) //if no tokens
		return NULL;
	
	//if there is only one token: create a tree and return it	
	if( tree->left==NULL && tree->right==NULL ){ 
		TreeNode* ret = createTreeNode(  createTokenInt(NULL, tree->element->frequency) );
		ret->left = createTreeNode( tree->element );
		//free global AVL Tree and set it to NULL
		free(tree);
		tree=NULL;
		return ret;
	}
	
	//HUFFMAN CODING
	MinHeap tokens_heap = buildMinHeap(tree);
		//free global AVL Tree and set it to NULL
		freeAVLTree(tree);
		tree = NULL;
		
	Queue trees_queue = {NULL,NULL};

	//continuously merge the two min elements from the heap and queue until we get one merged huffman tree
	//takes the smaller item in the heap/queue and merges the two minimums, then enqueues the tree onto the queue
	while( !(tokens_heap.size==0 && hasSizeOne(&trees_queue)) ){ //stops when heap is empty and only one element left in tree
		TreeNode* t1 = pickMinTreeHuffman(&tokens_heap, &trees_queue);
		TreeNode* t2 = pickMinTreeHuffman(&tokens_heap, &trees_queue);
		enqueue(&trees_queue, mergeTrees(t1,t2));
	}

	free(tokens_heap.heapArr);
	return dequeue(&trees_queue);
}




//COMPRESS METHODS////////////////////////////////////////////

/**
compresses file_name given and writes <file_name>.hcz into the same directory
**/
void compress( char* file_name ){
	if( file_name==NULL ){ pRETURN_ERRORvoid("passed in NULL file_name"); }
	
	//file contents of fname read into fstr (readFile is a fileHelperMethods.c method)
		char* fstr = readFile(file_name);
			if(fstr==NULL){ pRETURN_ERRORvoid("error reading codebook_name"); }
			
	//compressed file to write to:
		char* fcompr_name = getNewExtensionAndPath( file_name, ".hcz" ); //replaces extension with .hcz and gets string
		
		//opening file:
		int fcompr = openFileW(fcompr_name);
			if(fcompr<0){ free(fstr); free(fcompr_name); pRETURN_ERRORvoid("open write"); } //if error opening file
			
	
	//edge case: file has no delimiters
		if( strcspn(fstr , DELIMS) >= strlen(fstr) ){
			//get encoding associated with string
			char* fcode = getCodeItem( tree, fstr, cmpByTokens);
			
			//if file string is not found in codebook
			if( fcode == NULL){
				//free, delete, and remove
				free(fstr); REMOVE_AND_CHECK(fcompr_name); free(fcompr_name); close(fcompr);
				//error output
					printf("file has no delimiters and is not a token:%s\n", file_name);
					pRETURN_ERRORvoid("token doesn't exist in codebook");
			
			//if file string is found in codebook
			}else{
				WRITE_AND_CHECKv( fcompr , fcode , strlen(fcode) );
				return;
			}
		}
		
		
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
						tok1[indOfDELIM] = '\0';
						memcpy(tok1, fstr_ptr, indOfDELIM);//copy (indOfDELIM) number of characters from fstr into tok

					//finds encoding associated with tok1 and writes in fcompr
					char* tok1_encoding = getCodeItem( tree, tok1, cmpByTokens);
						free(tok1);
						
					//Error Check: if tok1 doesn't exist in codebook
						if( tok1_encoding == NULL){ 
							//free, delete, and remove
								free(fstr); REMOVE_AND_CHECK(fcompr_name);free(fcompr_name); close(fcompr);
							//error output
								printf("token:\"%s\" in file:%s\n", tok1, file_name);
								pRETURN_ERRORvoid("token doesn't exist in codebook");
					
					//if token exists, write encoding into the file		
						}else{ 
							WRITE_AND_CHECKv( fcompr , tok1_encoding , strlen(tok1_encoding) ); //writes encoding to file
						}

				}


			//TOKEN OF DELIM:
				//finding the token
				char* tok2 = getStringRepOfDELIM(fstr_ptr[indOfDELIM]);
					if(tok2==NULL){ pEXIT_ERROR("did not enter in delimiter"); }

				//finds encoding associated with tok2 and writes in fcompr
					char* tok2_encoding = getCodeItem( tree, tok2, cmpByTokens); //finds token
					
				//Error Check: if token does not exist in codebook
					if( tok2_encoding == NULL){ 
						//free, delete, and remove
							free(fstr); REMOVE_AND_CHECK(fcompr_name); free(fcompr_name); close(fcompr);
						//error output
							printf("token:\"%s\" in file:%s\n", tok2, file_name);
							pRETURN_ERRORvoid("token doesn't exist in codebook");
				
				//if Token exists in codebook, write the encoding into the file
					}else{ 
						WRITE_AND_CHECKv( fcompr , tok2_encoding , strlen(tok2_encoding) ); //writes encoding to file
					}


			//UPDATE fstr_ptr
				fstr_ptr  += ( indOfDELIM + 1 ); //update pointer of fstr_ptr to after current tokens
		}


	free(fcompr_name);
	free(fstr);
	close(fcompr);
}




//DECOMPRESS methods////////////////////////////////////////////

/**
decompresses given file_name.hcz given and writes decompressed file_name into the same directory
**/
void decompress( char* file_name){
	if( file_name==NULL ){ pRETURN_ERRORvoid("passed in NULL file name"); }
	if( !endsWithHCZ(file_name) ) { printf("file:%s\n",file_name); pRETURN_ERRORvoid("file must end with .hcz"); }

	//file contents of file_name read into fstr_hcz (readFile is a fileHelperMethods.c method)
	char* fstr_hcz = readFile(file_name);
		if(fstr_hcz==NULL){ pRETURN_ERRORvoid("error reading codebook_name"); }
		
	//decompressed file to write to:
	char* fdec_name = getNewExtensionAndPath(file_name, NULL); //removes .hcz extension

	int fdecompr = openFileW(fdec_name);
		if(fdecompr<0){ free(fstr_hcz); free(fdec_name); pRETURN_ERRORvoid("open write"); } //if error opening file
			

	//goes through file and takes a substring of bytes.
	//If substring is not associated with a token, the substring adds the next byte to the substring and tries to find the encoding. 
	//If substring is found and has an associated token, take the next substring in the file and repeat
	int file_len = strlen(fstr_hcz);
	int substr_start = 0;
	int substr_length = 1;
	
	
	while(substr_start  <  file_len){	
		//Find Substring and Token Associated (if Exists)
		char* curr_substr = substr(fstr_hcz, substr_start , substr_length+1 ); 
			if(curr_substr == NULL){ REMOVE_AND_CHECK(fdec_name); free(fdec_name);free(fstr_hcz);close(fdecompr); pRETURN_ERRORvoid("could not get substring");}
		char* token = getCodeItem(tree, curr_substr , cmpByEncodings); //tries to find token associated with encoding
				
		//IF TOKEN ASSOCIATED WITH SUBSTRING DOES NOT EXIST, add another byte to it
		if (token==NULL){
			substr_length += 1;
			
		//IF TOKEN ASSOCIATED WITH SUBSTRING EXISTS, write token to file and reset the next substring
		}else{
			//If the token is a Delimiter, get the Delimiter Representation and write the actual character to the file
			if(isDELIMStr(token)){
				char* insert = getCharRepOfDELIM(token);
					if(insert == NULL){ REMOVE_AND_CHECK(fdec_name); free(fdec_name);free(fstr_hcz);close(fdecompr); pRETURN_ERRORvoid("entered in a non-delimiter"); }
				WRITE_AND_CHECKv(fdecompr, insert, strlen(insert));
			}

			//if not Delimiter, just write it into file.
			else
				WRITE_AND_CHECKv(fdecompr, token, strlen(token));

			substr_start += substr_length;
			substr_length = 1; //reset length
		}
		
		free(curr_substr);
	}
	
	
	free(fdec_name);
	free(fstr_hcz);
	close(fdecompr);
}




//RECURSE methods////////////////////////////////////////////

/**
Runs the flag multiple times in ALL subdirectories of a given path. 
Note: does NOT runflag on the path given in
**/
void recurse(char* path){
	if(path==NULL){ pEXIT_ERROR("NULL PATH"); }
	
	//OPENS DIRECTORY
	DIR* curr_dir = opendir(path);
		if(curr_dir==NULL){ pEXIT_ERROR("opendir()"); }
	struct dirent* dp;

	
	//TRAVERSES THROUGH CURRENT DIRECTORY TO FIND ALL SUBDIRECTORIES
	while((dp = readdir(curr_dir))!=NULL){
		//if current/parent directory(ignore)
		if(strcmp(dp->d_name, ".")==0 || strcmp(dp->d_name, "..")==0) continue;
			
		//FINDS NEW_PATH NAME AND RECURSES IF NEW_PATH IS A DIRECTORY
		char* new_path = combinedPath(path,dp->d_name);
		if( typeOfFile(new_path) == isDIR ){
			runFlag(new_path);
			recurse(new_path);
		}
		
		free(new_path);
	}

	closedir(curr_dir);
}




//MISC methods//////////////////////////////////////////////


/**
Runs a flag operation on a pathfile_name given. 
Note: does NOT build codebook in runFlag
**/
void runFlag(char* pathfile_name){
	if( pathfile_name==NULL ){ printf("%s\n", pathfile_name); pEXIT_ERROR("path/file name passed in is NULL"); }
	
	//RUNS FLAG BASED ON IF DIRECTORY OR FILE : Note: if -b, just populates frequencies
	FileType type_pf = typeOfFile( pathfile_name ); //gets file type of path_file
	
	
	//PATH/FILE IS A DIRECTORY
	if( type_pf == isDIR ){			
		//opens directory of path_file
		DIR* dir_pf = opendir( pathfile_name ); 
			if(dir_pf ==NULL){ pEXIT_ERROR("opendir()"); }
		struct dirent* dpf;
			
		//TRAVERSES THROUGH CURRENT DIRECTORY TO FIND ALL ITS FILES
		while( (dpf = readdir(dir_pf))!=NULL ){
			//if current/parent directory(ignore)
			if(strcmp(dpf->d_name, ".")==0 || strcmp(dpf->d_name, "..")==0) continue;
				
			//get current file name
			char* currf_name = combinedPath( pathfile_name ,dpf->d_name ); 
			
			//runs flag on current file if it's a regular file
			if( typeOfFile( currf_name ) == isREG ){ 
				switch(flag){
					case 'b':
						buildFrequencyAVL( currf_name ); //adds onto the tree for every file
						break;
					case 'c':
						compress ( currf_name );
						break;
					case 'd':
						if( endsWithHCZ(currf_name) )//if compressed file - decompress
							decompress ( currf_name );
						break;
					default:
						pEXIT_ERROR("flag must be 'b', 'c', or 'd'");
				}
				
			}
			
			//free current file name after using it
			free(currf_name);
		}
		
		//close directory after done looping through all files	
		closedir(dir_pf);
		
		
			
	//IF PATH/FILE IS A REG FILE
	}else if( type_pf == isREG ){
		
		switch(flag){
			case 'b':
				buildFrequencyAVL( pathfile_name ); //gets frequency of each token in file
				break;
			case 'c':
				compress ( pathfile_name );
				break;
			case 'd':
				decompress ( pathfile_name );
				break;
			default:
				pEXIT_ERROR("flag must be 'b', 'c', or 'd'");
		}			
	}
	
}


/**
checks inputs of items passed through terminal and initializes globals
exits program if arguments do not match the requirements
**/
void inputCheck(int argc, char** argv){
	if(argc<3 || argc>5){ pEXIT_ERROR("Must pass inbetween 2 to 4 arguments in addition to the executable"); }

	//LOOPING THROUGH EACH ARGUMENT (excluding the first argument:the executable)
	int i;
	for(i=1; i<argc; i++){
		char* s = argv[i];

		//CHECK IF FLAG - if it is initialize it
		if(strlen(s) == 2 && s[0]=='-'){

			//Regular flag
			if( s[1]=='b'||s[1]=='c'||s[1]=='d' ){
				if(flag!='\0'){ pEXIT_ERROR("cannot have multiple flags"); } //already came across a flag
				flag = s[1]; //initialize flag
				
			//Recursive flag
			}else if( s[1]=='R' ){
				if(isRecursive){ pEXIT_ERROR("cannot have multiple '-R' flags"); } //already came across '-R' flag
				isRecursive = true; //initialize recursive boolean
			}


		//CHECK IF IS PATH/FILE and that it exists - if it is, initialize it
		}else{
			if( flag=='\0' ){ pEXIT_ERROR("must specify flag(s) before specifying file arguments!"); }
			if( typeOfFile(s) == isUNDEF ) { printf("argument:%s\n",s); pEXIT_ERROR("undefined file name"); } //checking if PATH/FILE exists , if not, exit
				
			//Path/File
			if(orig_pathfile==NULL){
				if( ( (flag=='c'||flag=='d')&& i!=argc-2)){ printf("argument:%s\n",s); pEXIT_ERROR("File must be specified as second to last argument if flag -c or -d. Last argument is reserved for the codebook."); }
				else if( (flag=='b'&&i!=argc-1) ){ printf("argument:%s\n",s); pEXIT_ERROR("Just a single file must be specified as last argument if flag -b. Can't have multiple files/flags"); }
				
				orig_pathfile = realpath(s, NULL); //gets real path and initializes orig_pathfile
			
			//Codebook
			}else{ 
				if(i!=argc-1){ printf("argument:%s\n",s); pEXIT_ERROR("codebook must be specified as last argument"); }
				if(flag=='b'){ pEXIT_ERROR("no reason to pass in multiple files for -b flag"); }
				codebook_name = s; //initializes codebook name
			}
		}
	}
	

	//CHECK if all necessary globals have been initialized
		if(flag=='\0'){ pEXIT_ERROR("must specify flag as an argument"); }
		if( orig_pathfile == NULL ){ pEXIT_ERROR("must give in a path or a file as an argument"); }
		
	//CHECK IF ARGUMENTS MATCH FLAG
		if(isRecursive && typeOfFile(orig_pathfile) != isDIR ){ isRecursive=false; printf("passed in -R flag but only passed in a REGULAR file, will run flag as if NOT recursive\n"); } //'-R' flag called but path not handed in
		if( (flag=='c'||flag=='d') && codebook_name == NULL ){ pEXIT_ERROR("must specify a codebook when running -c or -d flags"); }

	//INITIALIZE TREE FOR ALL FLAGS
		tree = NULL;
		if(flag=='c' || flag=='d'){
			//creates tree based on flag: if c, then builds codebooktree based on tokens, if d, then builds codebooktree based on encodings
			tree = (flag=='c')? buildCodebookTree(codebook_name, cmpByTokens) : buildCodebookTree( codebook_name, cmpByEncodings); 
				if( tree==NULL ){  pEXIT_ERROR("tried to build tree from codebook"); }
		}
		
}


int main(int argc, char** argv){
	//CHECKS ARGUMENTS PASSED IN THROUGH TERMINAL
	inputCheck(argc, argv);
	
	//Running the respective flag operation
	if(isRecursive){ //recursive
		runFlag(orig_pathfile);
		recurse(orig_pathfile);
	}else{
		runFlag(orig_pathfile);
	}

	//if flag is b, builds a single codebook in directory of executable using the tree creates
	if(flag=='b') 
		buildcodebook(orig_pathfile);
	//If flag is c or d, frees the tree
	else if(flag=='c' || flag=='d') 
		freeCodeTreeAndTok( tree );
	
	//free the original path/file since it was malloced getting the real path
	free(orig_pathfile);
	
	return 0;
}
