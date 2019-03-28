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
returns size of file in bytes
**/
int sizeOfFile(char* file_name){
	int file = open(file_name, O_RDONLY);
		if( file < 0 ){ pEXIT_ERROR("error opening file"); }

	int file_len = (int)lseek( file , 0, SEEK_END ); //gets file size in bytes by going to end of file_cpy
		if ( file_len < 0 ){ pEXIT_ERROR("error getting file length with lseek()"); }//checking if file_len is a valid length

	close(file);
	return file_len;
}


/**
reads a file given a filename.
@returns: string of contents of file if successful
**/
char* readFile(char* file_name){
	//INITIALIZING VARIABLES AND OPENING THE FILE
		//Opening files
		int file = open(file_name, O_RDONLY); //file
			 if( file < 0 ){ pEXIT_ERROR("error opening file"); }

		//Initializing file length in bytes
		int file_len = sizeOfFile(file_name); //length of file in bytes

		//Initializing File Strings to return
		char* fstr = (char*)calloc((file_len + 1), 1); //string with file's contents, return this string on success
			if( fstr == NULL ){ pEXIT_ERROR("calloc()"); }


	//READING THE FILE
	int bytes_read = read(file, fstr, file_len); //number of bytes read through read
		if(bytes_read < 0){ pEXIT_ERROR("error reading file"); }

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
	if(file_name == NULL){ pRETURN_ERROR("file_name passed in is NULL", NULL);  }
	if( typeOfFile(file_name)!=isREG  ){ pRETURN_ERROR("file_name passed in is not a file", NULL); }

	char* realp = realpath(file_name, NULL); //gets the real path of file_name

	//FINDS NUM CHARACTERS BEFORE SLASH
	int chars_b4Slash;
	for( chars_b4Slash = strlen(realp)-1 ; chars_b4Slash>=0; chars_b4Slash--){
		if(realp[chars_b4Slash]== '/') break;
	}
	if(chars_b4Slash <= 0){ pRETURN_ERROR("error, no instance of '/'", NULL); }


	//malloc string to return and copies the number of charactes before the slash
	char* ret = (char*)malloc(chars_b4Slash+1);
		if(ret==NULL){ pEXIT_ERROR("malloc"); }
	memcpy(ret, realp, chars_b4Slash);//copy characters before the slash
		ret[chars_b4Slash] = '\0';
		free(realp);

	//check if return string is a directory
	if( typeOfFile(ret)==isDIR  )
		return ret;
	else
		return NULL;
}


/**
returns the full path name of where a new_file would be inserted in the directory of the old_file
**/
char* getNewFilePath(char* old_file, char* new_file_name){
		if(old_file==NULL||new_file_name==NULL){ pRETURN_ERROR("can't pass in NULL string", NULL); }

		char* dir = getDirOfFile(old_file); //gets the directory where old_file lies
		char* dirslash = appendCharToString(dir, '/'); //adds dash to end
		free (dir);

		char* new_path = concatStr(dirslash, new_file_name);
		free(dirslash);


		return new_path;
}


/**
replaces old_file_name's extension with a new extension and returns the PATH of the filename with the new extension (the path is in the directory of thr old file)
If extension passed in is NULL, removes the extension and returns the path

i.e. getNewExtension("file.txt", ".hcz"); returns <file.txt's path>/"file.hcz"
	   getNewExtension("file.txt", NULL); returns <file.txt's path>/"file"
**/
char* getNewExtensionAndPath( char* old_file_name, const char* extension ){
	if( old_file_name == NULL){ pRETURN_ERROR("can't pass in NULL file_name", NULL); }
	if( typeOfFile(old_file_name)!=isREG  ){ pRETURN_ERROR("file_name passed in is not a file", NULL); }


	char* oldfname_path = realpath( old_file_name, NULL); //get real path of old_file_name


	//FINDING NUMBER OF CHARACTERS before the '.' if it exists, if no dot, it returns -1
	int len_oldfnpath = strlen(oldfname_path);
	int chars_b4Dot = -1;

	int i;
	for(i = len_oldfnpath ; i>=0; i--){
		if( oldfname_path[i] == '/'){
			chars_b4Dot = -1;
			break;
		}else if ( oldfname_path[i] == '.'){
			chars_b4Dot = i;
			break;
		}
	}


	//IF REMOVING THE EXTENSION
	if(extension == NULL){ //want to remove extension
		if(chars_b4Dot==-1){ pRETURN_ERROR("error, no extension to remove from.", NULL); }

		//malloc enouth space without extension
		char* ret = malloc( chars_b4Dot + 1 );
		memcpy (ret, oldfname_path, chars_b4Dot);
			ret[chars_b4Dot] = '\0';
			free(oldfname_path);

		return ret;
	}


	//IF REPLACING THE EXTENSION
	if( chars_b4Dot == -1){ //there is no dot
		//realloc space for extension
			char* ret = realloc( oldfname_path, (len_oldfnpath  + (strlen(extension)) + 1) );
				if(ret == NULL){ pEXIT_ERROR("realloc"); } //realloc error
		//add the extension to the end
			ret = strcat(ret , extension);

		return ret;

	}else if( chars_b4Dot!= 0 ){ // there is a dot after the first character

		//mallocspace for extension
			char* ret = (char*)malloc( chars_b4Dot + strlen(extension) + 1 ); // Make space for adding the extension.
				if(ret == NULL){ pEXIT_ERROR("realloc"); } //realloc error
		//copies oldfname_path before the dot into ret and frees old string
			memcpy(ret, oldfname_path , chars_b4Dot);
				ret[chars_b4Dot] = '\0';
				free(oldfname_path);
		//add extension to end
			ret = strcat(ret , extension); //copy the extension to the end

		return ret;
	}


	return NULL; //if dot at first char

}

/**
returns a string of a delimiter
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
	 	case '\0':
	 		return "\\0";
	 	default:
	 		pRETURN_ERROR("passed in non-delimiter",NULL);
	 }

	return NULL;
}


/**
checks if a string is a delimiter
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
	 	case '0':
	 		return true;

	 	default:
	 		return false;
	 }

	return false;
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
	else if(S_ISLNK(dpstat.st_mode)) //symbolic link
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

				}else if(fstr[i] ==' ' || fstr[i] == '\t'){ //token should not have a tab or a space in the token
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
