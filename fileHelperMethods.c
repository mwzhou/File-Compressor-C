/*
fileHelperMethods.c is a self-made file library since we're not allowed to use f-commands
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<sys/types.h>
#include"fileHelperMethods.h"


/**
reads a file given a filename.
@returns: string of contents of file if successful
 returns: NULL if invalid, non-urgent error
**/
char* readFile(char* file_name){
	//VARIABLES
		int file, file_cpy; //files
		int file_len; //length of file in bytes
		char* fstr; //string with file's contents, return this string on success
		int bytes_read; //number of bytes read through read

	//INITIALIZING VARIABLES AND OPENING FILE
		//Opening files
		file = open(file_name, O_RDONLY);
		file_cpy = open(file_name, O_RDONLY); //copy of file
			 if( file < 0 || file_cpy < 0 ){ pRETURN_ERROR("error opening file", NULL); }

		//Initializing file length in bytes
		file_len = (int)lseek( file_cpy, 0, SEEK_END ); //gets file size in bytes by going to end og file_cpy
			if ( file_len < 0){ pRETURN_ERROR("error getting file length with lseek()", NULL); } //checking if file_len is a valid length
			else if( file_len == 0 ){  pRETURN_ERROR( "error, can't pass in empty file", NULL); } //TODO: verify if this is a condition	
			close(file_cpy);
			
		//Initializing File Strings to return
		fstr = (char*)calloc((file_len + 1), 1);
		if( fstr == NULL ){ pEXIT_ERROR("calloc()"); }


	//READING FILE
	bytes_read = read(file, fstr, file_len);
		if(bytes_read < 0){ pRETURN_ERROR("error reading file", NULL); }
	
	fstr[bytes_read] = '\0'; //mark end of string

	close(file);
	return fstr;
}


/**
Combines a path string with a file string and returns the new path
@returns: a copy of the new path
returns: NULL if invalid, non-urgent issue
**/
char* combinedPath(char* path_name, char* file_name){
	if(path_name==NULL || file_name==NULL){ pRETURN_ERROR("cannot pass in NULL string into combinedPath()", NULL); }

	//reallocate enough space
	char* ret = (char*)malloc( 2 + strlen(path_name) + strlen(file_name) );
		if(ret==NULL){ pEXIT_ERROR("malloc"); }

	//copies and concatenates string
	strcpy(ret, path_name);
	strcat(ret, "/");
	strcat(ret, file_name);

	return ret;
}


/**
Combines two strings together into a new concatenated string
NOTE: does not free original strings
**/
char* concatStr(char* s1, char* s2){
	if(s1==NULL || s2==NULL){ pRETURN_ERROR("cannot pass in NULL string into combinedPath()", NULL); }

	//reallocate enough space
	char* ret = (char*)malloc( 1 + strlen(s1) + strlen(s2) );
		if(ret==NULL){ pEXIT_ERROR("malloc"); }

	//copies and concatenates string
	strcpy(ret, s1);
	strcat(ret, s2);

	return ret;
}


/**
Appends a character to the end of a string, returns a malloced new string with a character appended to the end
@params: char* prev_str = string to append to
		 char add_c = character to add
@return: new_string(malloced in different memory)
Note: does NOT free prev_str
**/
char* appendCharToString( char* prev_str , char add_c){
	//Variables
		int len_prev = (prev_str == NULL)? 0 : strlen(prev_str); //length of prev_str, length is 0 if prev_str was NULL
		char* new_str = (char*)malloc( len_prev + 2 ); //string to return: malloc one byte larger than prev_str + space for the terminating char
			if( new_str==NULL ){ pEXIT_ERROR("malloc()"); }

	//CREATING NEW ENCODING
		if(prev_str!= NULL)  //copy old encoding into new encoding (if old encoding not NULL)
			strcpy( new_str , prev_str); 
		new_str[len_prev] = add_c;
		new_str[len_prev+1] = '\0'; //terminating character

	return new_str;
}


/**
gets the directory of a given filename
eturns string of directory (malloced)
**/
char* getDirOfFile( char* file_name){
	if( typeOfFile(file_name)==isDIR ){ //if already a directory
		return file_name;
	}
	
	char* realp = realpath(file_name, NULL); //real path of file_name
	
	//loops through end of realp and 0's out each character until first '/' character
	int i;
	for(i= strlen(realp)-1 ; i>=0; i--){
		if(realp[i]== '/')
			break;
		realp[i] = '\0';
	}
	
	if( typeOfFile(realp)==isDIR  )
		return realp;
	else
		return NULL;
}


/**
returns the type of the string given in
@params: char* name - file_name or path_name
@returns FileType:
	isDIR - directory
	isREG - regular file
	isLINK - link
	isUNDEF - error
**/
FileType typeOfFile(char* file_name){
	if(file_name ==NULL){ pRETURN_ERROR("passed in NULL path", isUNDEF); }

	struct stat dpstat;
	if(stat( file_name  , &dpstat) < 0){ pRETURN_ERROR("lstat failed", isUNDEF); }

	//check if DIR, REG, or LINK, and returns the respective number (defined in macro)
	if(S_ISREG(dpstat.st_mode)) //directory or file
		return isREG;
	else if(S_ISLNK(dpstat.st_mode))
		return isLNK;
	else if(S_ISDIR(dpstat.st_mode))
		return isDIR;
	else
		return isUNDEF;
}

/**
returns true if file name ends in .hcz
**/
bool endsWithHCZ(char* file_name){
	int len = strlen(file_name);
	if(len<5) //must have enough character, minimum length is 5, i.e.: "c.hcz""
		return false;
	
	if( file_name[len-4]=='.' && file_name[len-3]=='h' && file_name[len-2]=='c' && file_name[len-1]=='z' ) //ends with .hcz
		return true;
		
	return false;
}


/**
Goes through file_name given and checks if the file matches the format of a huffman codebook
**/
bool isHuffmanCodebook(char* file_name){ 
	if( typeOfFile(file_name) != isREG ) //file passed in is not a regular file
		return false; 		
		
		
	//READ FILE as a string
		char* fstr = readFile(file_name);
			if( fstr == NULL ) //error reading file
				return false; 
	
	
	//CHECK FORMAT OF FILE
		//checking first line:
		if(fstr[0] != '\\' && fstr[0] != '\n'){ //first line must be in the format of "\\\n"
			free(fstr);
			return false;
		}
		
		
		//checking contents:
		int len_fstr = strlen(fstr);
			if(len_fstr< 3){ //must have at least 3 characters in huffman codebook
				free(fstr); 
				return false;
			}
		bool isEncoding = true; //boolean to check format of encoding/token
		
		//loop in the body of the file
		int i;
		for(i = 2; i< len_fstr-1; i++){ 
			//check format of encoding
			if(isEncoding){
				if( fstr[i] == '\t' ){ //reached end of current encoding
					isEncoding = false; //update to check format for token
					
				}else if( !(fstr[i] == '0' || fstr[i] == '1') ){ //encoding must be a string of '0's and '1's, if not: return false
					free(fstr); 
					return false;
				}
			
			//check format of token
			}else{
				if(fstr[i] == '\n'){ //reached end of token
					isEncoding = true; //update to check format for encoding
					
				}else if(fstr[i] ==' ' || fstr[i] == '\t'){ //token should not have a tab or whitespace in the token
					free(fstr); 
					return false;
					
				}	
			}
		}
		
		if(!isEncoding){ //did not end the body with a token (should have set isEncoding=true)
			free(fstr); 
			return false;		
		}
		
		//checking terminating line: 
		if( fstr[len_fstr-2]!='\n' && fstr[len_fstr-1]!='\n' ){ //last character befor terminating line must be "\n" and terminating line must be "\n"
			free(fstr); 
			return false;
		}
		
	free(fstr);
	return true;
}
