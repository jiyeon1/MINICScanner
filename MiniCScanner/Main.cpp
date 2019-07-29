#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Scanner.h"

void icg_error(int n);

FILE *astFile;                          // AST file
FILE *sourceFile;                       // miniC source program
FILE *ucodeFile;                        // ucode file

#define FILE_LEN 30

void main(int argc, char *argv[])
{
	char fileName[FILE_LEN];
	int err;
	
	printf(" *** start of Mini C Compiler\n");
	if (argc != 2) {
		icg_error(1);
		exit(1);
	}
	strcpy_s(fileName, argv[1]);
	printf("   * source file name: %s\n", fileName);

	err = fopen_s(&sourceFile, fileName, "r");
	if (err != 0) {
		icg_error(2);
		exit(1);
	}

	struct tokenType token;

	printf(" === start of Scanner\n\n");
	printf("       Type         ->    Token    (  Token number  ,  Token value  ,  File name  ,  Line number  ,  Column number )\n");	//출력형식
	printf("------------------------------------------------------------------------------------------------------------------\n");
	
	do
	{
		token = scanner();
		if (token.number != teof)
		{
			if (token.comment_type == 0)
				printf("       Token        ->  %7s    (  %7d       ,  %7s      ,  %5s   ,   %5d       ,    %5d )\n", token.str, token.number, token.value, fileName, token.line_num, token.col_num);
			else if (token.comment_type == 1)
				printf("   Single - line    ->  // %s  \n", token.comment);
			else if (token.comment_type == 2)
				printf("    Multi - line    ->  /* %s */  \n", token.comment);
			else if (token.comment_type == 3)
				printf("Documented comments ->  /** %s */ \n", token.comment);
		}
	} while (token.number != teof);
	printf("\n");

	printf(" *** end   of Mini C Compiler\n");
} // end of main

void icg_error(int n)
{
	printf("icg_error: %d\n", n);
	//3:printf("A Mini C Source file must be specified.!!!\n");
	//"error in DCL_SPEC"
	//"error in DCL_ITEM"
}
