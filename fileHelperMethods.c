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
returns NULL if there's an error
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
			 if( file < 0 || file_cpy < 0 ){
	 			PRINT_ERROR("error opening file");perror(file_name);return NULL;
	 		 }

		//finding length of filemake
		file_len = (int)lseek( file_cpy, 0, SEEK_END ); //gets file size in bytes by going to end og file_cpy
			if ( file_len < 0){
				PRINT_ERROR("error getting file length with lseek()"); perror(file_name); close(file_cpy);return NULL;
			}else if( file_len == 0 ){ //TODO: verify if this is a condition
				PRINT_ERROR( "error, can't pass in empty file"); close(file_cpy); return NULL;
			}
			close(file_cpy);

		//file string : to return
		str_f = (char*)calloc((file_len + 1), 1);
		if( str_f == NULL ){ pEXIT_ERROR("calloc"); }


	//READING FILE
	  bytes_read = read(file, str_f, file_len);
			if(bytes_read < 0){
				PRINT_ERROR("error reading file");perror(file_name);return NULL;
			}
		str_f[bytes_read] = '\0'; //mark end of string

	close(file);
	return str_f;
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
	if(file_name ==NULL){ //passed in NULL dirent or curr_dir
		PRINT_ERROR("passed in NULL path");return isUNDEF;
	}

	struct stat dpstat;
	if(stat( file_name  , &dpstat) < 0){ //error calling lstat
		perror("lstat failed"); return isUNDEF;
	}

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

