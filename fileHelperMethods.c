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
		int file, file_cpy;
		int file_len;
		int bytes_read;
		char* str_f;

	//INITIALIZING VARIABLES AND OPENING
		//Opening files
		file = open(file_name, O_RDONLY);
		file_cpy = open(file_name, O_RDONLY);
			 if( file < 0 || file_cpy < 0 ){ pRETURN_ERROR("error opening file", NULL); }

		//finding length of filemake
		file_len = (int)lseek( file_cpy, 0, SEEK_END ); //gets file size in bytes by going to end og file_cpy
    	//checking if file_len is a valid length
			if ( file_len < 0){ pEXIT_ERROR("error getting file length with lseek()");
		}else if( file_len == 0 ){ pRETURN_ERROR( "error, can't pass in empty file", NULL);} //TODO: verify if this is a condition
			close(file_cpy);

		//file string : to return
		str_f = (char*)calloc((file_len + 1), 1);
		if( str_f == NULL ){ pEXIT_ERROR("calloc"); }


	//READING FILE
	  bytes_read = read(file, str_f, file_len);
			if(bytes_read < 0){ pRETURN_ERROR("error reading file", NULL); }
		str_f[bytes_read] = '\0'; //mark end of string

	close(file);
	return str_f;
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
	if(path_name==NULL){ pEXIT_ERROR("malloc"); }

	//copies and concatenates string
	strcpy(ret, path_name);
	strcat(ret, "/");
	strcat(ret, file_name);

	return ret;
}


/**
returns the type of the string given in
@params: char* name - file_name or path_name
@returns:
	DIR - directory
	REG - regular file
	LINK - link
	UNDEF - error
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
