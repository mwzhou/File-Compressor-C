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


//FILE methods/////////////////////////////////////////////////////////////////////

/**
returns size of file in bytes
returns -1 on error
**/
int sizeOfFile(char* file_name){
	int file = open(file_name, O_RDONLY, S_IRUSR);
		if( file < 0 ){ printf("file_name: %s\n",file_name); pRETURN_ERROR("error opening file (O_RDONLY)", -1); }

	int file_len = (int)lseek( file , 0, SEEK_END ); //gets file size in bytes by going to end of file_cpy
		if ( file_len < 0 ){ pEXIT_ERROR("error getting file length with lseek()"); }//checking if file_len is a valid length

	close(file);
	return file_len;
}


/**
reads a file given a filename.
@returns: string of contents of file if successful
 returns NULL if unsuccessful
**/
char* readFile(char* file_name){
	//INITIALIZING VARIABLES AND OPENING THE FILE
		//Opening files
		int file = open(file_name, O_RDONLY, S_IRUSR);
			 if( file < 0 ){ printf("file_name: %s\n",file_name); pRETURN_ERROR("error opening file (O_RDONLY)", NULL); }

		//Initializing file length in bytes
		int file_len = sizeOfFile(file_name); //length of file in bytes
			
		//Initializing File Strings to return
		char* fstr = (char*)calloc((file_len + 1), 1); //string with file's contents, return this string on success
			if( fstr == NULL ){ pEXIT_ERROR("calloc()"); }


	//READING THE FILE
	int bytes_read = read(file, fstr, file_len); //number of bytes read through read
		if(bytes_read < 0){ printf("file_name: %s\n",file_name); pRETURN_ERROR("error reading file", NULL); }

	fstr[bytes_read] = '\0'; //mark end of string

	close(file);
	return fstr;
}


/**
opens file for writing with name (file_name)
returns file descriptor if successful
returns -1 if not
Note: does not free file_name
**/
int openFileW(char* file_name){
	if(file_name ==NULL){ pRETURN_ERROR("null file path", -1); }
	
	int fd = open( file_name, O_WRONLY|O_CREAT|O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) ); //creates a new file, overwrites old file if exists
		if(fd < 0){ printf("file:%s\n",file_name ); pRETURN_ERROR("tried to open file flags: (O_WRONLY|O_CREAT|O_TRUNC)", -1); }
	
	return fd;
}



/**
returns the type of the string given in
@params: char* name - file_name or path_name
@returns FileType:
	isDIR - directory
	isREG - regular file
	isUNDEF - error
**/
FileType typeOfFile(char* file_name){
	if(file_name ==NULL){ pRETURN_ERROR("passed in NULL path", isUNDEF); }
	if( file_name[(int)strlen(file_name)-1] == '~' ) return isUNDEF; 

	struct stat dpstat;
	if(stat( file_name  , &dpstat) < 0){ return isUNDEF; } //file doesn't exist

	//check if DIR, REG, or LINK, and returns the respective number (defined in macro)
	if(S_ISREG(dpstat.st_mode)) //directory or file
		return isREG;
	else if(S_ISDIR(dpstat.st_mode))
		return isDIR;
	else
		return isUNDEF;
}




//STRING MANIPULATION methods/////////////////////////////////////////////////////////////////////

/**
To be used in fileCompressor.c recurse() to keep track of paths
Combines a path name with a file name and returns the new path
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
if extension is NULL, removes file_name's extension, otherwise, adds on the extension to file name and returns new file_name
**/
char* getNewExtensionAndPath( char* old_file_name, const char* extension ){
	if( old_file_name == NULL){ pRETURN_ERROR("can't pass in NULL file_name", NULL); }
	if( typeOfFile(old_file_name)!=isREG  ){ pRETURN_ERROR("file_name passed in is not a file", NULL); }

	//get real path of old_file_name
	char* oldfname_path = realpath( old_file_name, NULL); 

	//IF REMOVING THE EXTENSION	
	if(extension == NULL){
		int chars_b4Dot = lengthBeforeLastOccChar( oldfname_path, '.');
			if(chars_b4Dot==-1){ pRETURN_ERROR("error, no extension to remove from.", NULL); }
			
		//malloc enough space for file_name without the extension
		char* ret = malloc( chars_b4Dot + 1 );
		memcpy (ret, oldfname_path, chars_b4Dot);
			ret[chars_b4Dot] = '\0';
			free(oldfname_path);

		return ret;
	
	//IF ADDING THE EXTENSION
	}else{
		//realloc space for extension
		char* ret = realloc( oldfname_path, (strlen(oldfname_path)  + (strlen(extension)) + 1) );
			if(ret == NULL){ pEXIT_ERROR("realloc"); } //realloc error
		//add the extension to the end of file_name
		ret = strcat(ret , extension);
		
		return ret;
	}


	return NULL; //if dot at first char

}


/**
returns number of characters in s before the last occurrence of c
returns -1 if hits a '/' character before the dot
**/
int lengthBeforeLastOccChar( char* s, char c){
	//FINDING NUMBER OF CHARACTERS before the '.' if it exists, if no dot, it returns -1
		int len = strlen(s);
		int i;
		for(i = len; i>=0; i--){
			if( s[i] == '/'){
				return -1;
			}else if ( s[i] == c){
				return i;
			}
		}
		
		return -1;
}


/**
To be used in fileCompressor.c writeEncodings() to keep track of encodings of each node in a huffman tree.
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
To be used in fileCompressor.c for decompress.
returns a subtring of s from the start_index to desired length of substring
returns NULL if length>strlen(s) or could not get a substring
**/
char* substr(char* s, size_t start_ind, size_t length){
	if( s==NULL||start_ind<0||length<0 ){ pRETURN_ERROR("faulty parameters", NULL); }
	if( length-1 > strlen(s) ){ pRETURN_ERROR("length-1 cannot be larger than the string passed in",NULL); }
		
	char* ret = (char*)malloc(length); //malloc string to return
		if(ret==NULL){ pEXIT_ERROR("malloc"); }
		
	memcpy(ret, s+start_ind, length); //copies s+start to length into ret
	ret[length - 1] = '\0';
	
	if(ret==NULL){ pRETURN_ERROR("Substring not found", NULL); }
	return ret;
}




//DELIM methods/////////////////////////////////////////////////////////////////////

/**
returns a string representation of a delimiter
**/
char* getStringRepOfDELIM( char c ){
	 switch(c){
	 	case ' ':
			return "\\S";
	 	case '\a':
	 		return "\\a";
	 	case '\b':
	 		return "\\b";
	 	case '\t':
	 		return "\\t";
	 	case '\n':
	 		return "\\n";
	 	case '\v':
	 		return "\\v";
	 	case '\f':
	 		return "\\f";
	 	case '\r':
	 		return "\\r";
	 	case '\\':
	 		return "\\e";
		case '\'':
				return "\\1";
		case '\"':
				return "\\2";
		case '\?':
				return "\\?";
	 	case '\0':
	 		return "\\0";
	 	default:
	 		pRETURN_ERROR("passed in non-delimiter",NULL);
	 }

	return NULL;
}


/**
returns the single character string that a string delimiter represents
if not a delimiter, then returns NULL
getCharRepOfDELIM() avoids having to do a strcmp to save time
**/
char* getCharRepOfDELIM( char* s ){
	if(s==NULL)
		pRETURN_ERROR("s is null", NULL);
	if(strlen(s)!=2)
		pRETURN_ERROR("not a delim", NULL);


	if(s[0]!='\\') //must start with esc char
		pRETURN_ERROR("not a delim", NULL);

	switch(s[1]){ //if matches one of the delimiters
	 	case 'S':
			return " ";
	 	case 'a':
			return "\a";
	 	case 'b':
			return "\b";
	 	case 't':
			return "\t";
	 	case 'n':
			return "\n";
	 	case 'v':
			return "\v";
	 	case 'f':
			return "\f";
	 	case 'r':
			return "\r";
	 	case 'e':
			return "\\";
		case '1':
				return "\'";
		case '2':
				return "\"";
		case '?':
				return "\?";
	 	case '0':
	 		return "\0";
	 	default:
	 		pRETURN_ERROR("not a delim", NULL);
	 }

	pRETURN_ERROR("not a delim", NULL);
}


/**
checks if a string is a delimiter string without having to use strcmp (to save time)
**/
bool isDELIMStr(char* s){
	if(s==NULL)
		return false;
	else if(strlen(s)!=2)
		return false;


	if(s[0]!='\\') //must start with esc char
		return false;

	switch(s[1]){ //if matches one of the delimiters
	 	case 'S':
	 	case 'a':
	 	case 'b':
	 	case 't':
	 	case 'n':
	 	case 'v':
	 	case 'f':
	 	case 'r':
	 	case 'e':
		case '1':
		case '2':
		case '?':
	 	case '0':
	 		return true;

	 	default:
	 		return false;
	 }

	return false;
}




//FILE-CHECKING methods/////////////////////////////////////////////////////////////////////

/**
returns true if file name ends in .hcz
**/
bool endsWithHCZ(char* file_name){
	if(file_name==NULL){ pRETURN_ERROR("null string",false);}
	int len = strlen(file_name);
	if(len<4) //minimum length is 4, i.e.: ".hcz""
		return false;

	if( file_name[len-4]=='.' && file_name[len-3]=='h' && file_name[len-2]=='c' && file_name[len-1]=='z' ) //ends with .hcz
		return true;

	return false;
}

