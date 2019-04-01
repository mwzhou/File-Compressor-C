#ifndef FILE_HELP
#define FILE_HELP

//ERROR MACROS
	//prints error along with line number, file name, and function name
	#define PRINT_ERROR(txt) ( fprintf( stderr, "ERROR: %s\nProgram:%s in Function:%s() on Line:%d\n",txt,__FILE__,  __func__, __LINE__))
	//exits program on failure
	#define pEXIT_ERROR(txt) do{ PRINT_ERROR(txt); perror("exit"); fprintf(stderr, "\n"); exit(EXIT_FAILURE); }while(0) 
	//returns out of method and prints error
	#define pRETURN_ERROR(txt, return_val) do{ PRINT_ERROR(txt); perror("return"); fprintf(stderr, "\n"); return return_val; }while(0) 
	//returns void and prints out error
	#define pRETURN_ERRORvoid(txt) do{ PRINT_ERROR(txt); perror("return"); fprintf(stderr, "\n"); return; }while(0) 
	
//MACROS FOR READABILITY
	#define DELIMS " \a\b\t\n\v\f\r\'\"\?\\\0" //string of control characters as well as the space char to separate a file into tokens
	#define WRITE_AND_CHECKv(file, buf, nbytes) do{  if( write(file, buf , nbytes) < 0 ) { pRETURN_ERRORvoid("write()"); } }while(0) //writes to file, if failed, prints out error and returns void
	#define REMOVE_AND_CHECK(file_name) do{ if(remove(file_name) == 0) fprintf( stderr, "removed file:%s\n",file_name); else fprintf( stderr, "couldn't remove file:%s",file_name);  }while(0) //removes file and prints if successful

//ENUMS
	//enum to differentiate between FileTypes
	typedef enum{ isDIR=017, isREG=736, isUNDEF=-1 }FileType;


//METHOD SIGNATURES
	int sizeOfFile(char* file_name);
	char* readFile(char* file_name);
	int openFileW(char* file_name);
	FileType typeOfFile(char* file_name);

	char* combinedPath(char* path_name, char* file_name);
	char* getNewExtensionAndPath( char* old_file_name, const char* extension );

	int lengthBeforeLastOccChar( char* s, char c);
	char* appendCharToString( char* prev_str , char add_c);
	char* substr(char* s, size_t start_ind, size_t length);
	
	char* getStringRepOfDELIM( char c );
	char* getCharRepOfDELIM( char* s );
	bool isDELIMStr( char* s );

	bool endsWithHCZ(char* file_name);

#endif
