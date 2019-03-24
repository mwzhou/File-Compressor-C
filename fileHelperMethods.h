#ifndef FILE_HELP
#define FILE_HELP
	
//ERROR MACROS
	#define PRINT_ERROR(txt) (printf("ERROR: %s in: %s on line:%d\n",txt,__FILE__, __LINE__))
	#define pEXIT_ERROR(txt) do{ PRINT_ERROR(txt); perror(txt); exit(EXIT_FAILURE); }while(0) //exits program on failure
	#define pRETURN_ERROR(txt, return_val) do{ PRINT_ERROR(txt); perror(txt); return return_val; }while(0) //returns method out of EXIT_FAILURE
	#define pRETURN_ERRORvoid(txt) do{ PRINT_ERROR(txt); perror(txt); return; }while(0)
	
//ENUMS	
	//enum to differentiate between FileTypes
	typedef enum{ isDIR, isREG, isLNK, isUNDEF }FileType;


//METHOD SIGNATURES
	char* readFile(char* file_name);
	FileType typeOfFile(char* file_name);
	char* combinedPath(char* path_name, char* file_name);
	char* appendCharToString( char* prev_str , char add_c);
	bool isHuffmanCodebook(char* file_name);

#endif
