/***************************************************************
*      scanner routine for Mini C language                    *
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Scanner.h"

extern FILE *sourceFile;	// miniC source program

int superLetter(char ch);			//keyword
int superLetterOrDigit(char ch);	//digit
double getNumber(char firstCharacter);	//number
int hexValue(char ch);				//hex
void lexicalError(int n);			//error
bool isDigitOrPoint(char c);		//digit or character

int line_num = 1;
int col_num = 1;
bool point_check = false;
char temp_str[MAX_LENGTH];

char *tokenName[] = {
	"!",        "!=",      "%",       "%=",     "%ident",   "%number",
	/* 0          1           2         3          4          5        */
	"&&",       "(",       ")",       "*",      "*=",       "+",
	/* 6          7           8         9         10         11        */
	"++",       "+=",      ",",       "-",      "--",	    "-=",
	/* 12         13         14        15         16         17        */
	"/",        "/=",      ";",       "<",      "<=",       "=",
	/* 18         19         20        21         22         23        */
	"==",       ">",       ">=",      "[",      "]",        "eof",
	/* 24         25         26        27         28         29        */
	//   ...........    word symbols ................................. //
	/* 30         31         32        33         34         35        */
	"const",    "else",     "if",      "int",     "return",  "void",
	/* 36         37         38        39                              */
	"while",    "{",        "||",       "}"
};

char *keyword[NO_KEYWORD] = {
	"const", "else", "if", "int", "return", "void", "while"	,"char",
	"double", "for", "do", "goto", "switch", "case", "break", "default"
};

enum tsymbol tnum[NO_KEYWORD] = {
	tconst, telse, tif, tint, treturn, tvoid, twhile, tchar,
	tdouble, tfor, tdo, tgoto, tswitch, tcase, tbreak, tdefault
};

struct tokenType scanner()
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];

	token.number = tnull;

	do {
		while (isspace(ch = fgetc(sourceFile)))	// state 1: skip blanks
		{
			if (ch == '\n')
			{
				line_num++;
				col_num = 1;
			}
			else
				col_num++;
		}
		if (superLetter(ch)) { // identifier or keyword
			token.line_num = line_num;
			token.col_num = col_num;
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(sourceFile);
				col_num++;
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			strcpy(token.str, id);
			ungetc(ch, sourceFile);  //  retract
									 // find the identifier in the keyword table
			for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORD)    // found, keyword exit
				token.number = tnum[index];
			else {                     // not found, identifier exit
				token.number = tident;
				strcpy(token.value, id);
			}
		}  // end of identifier or keyword
		else if (isdigit(ch)) {  // number
			token.line_num = line_num;
			token.col_num = col_num;
			token.number = tnumber;
			char temp[ID_LENGTH];
			double tmp = getNumber(ch);
			strcpy(token.str, temp_str);
			if (tmp >= 99999999999)
				strcpy(token.value, temp_str);
			else if (point_check)
			{
				sprintf(temp, "%lf", tmp);
				strcpy(token.value, temp);
			}
			else
			{
				sprintf(temp, "%d", (int)tmp);
				strcpy(token.value, temp);
			}
			memset(temp_str, '\0', sizeof(temp_str));
		}
		else switch (ch) {  // special character
		case '/':
			ch = fgetc(sourceFile);
			col_num++;
			if (ch == '*')			// text comment
			{
				char temp[MAX_LENGTH];
				i = 0;
				ch = fgetc(sourceFile);
				col_num++;
				if (ch == '*') //documented comment
				{
					token.comment_type = 3;
					do {
						while (ch != '*')
						{
							if (ch == '\n')
								ch = ' ';
							temp[i++] = ch;
							ch = fgetc(sourceFile);
							col_num++;
						}
						ch = fgetc(sourceFile);
						col_num++;
					} while (ch != '/');
					temp[i] = '\0';
					strcpy(token.comment, temp);
					ungetc(ch, sourceFile);
				}
				else		//multi line comment
				{
					token.comment_type = 2;
					do {
						while (ch != '*')
						{
							if (ch == '\n')
								ch = ' ';
							temp[i++] = ch;
							ch = fgetc(sourceFile);
							col_num++;
						}
						ch = fgetc(sourceFile);
						col_num++;
					} while (ch != '/');
					temp[i] = '\0';
					strcpy(token.comment, temp);
					ungetc(ch, sourceFile);
				}
			}
			else if (ch == '/')		// single line comment
			{
				char temp[MAX_LENGTH];
				i = 0;
				token.comment_type = 1;
				do {
					ch = fgetc(sourceFile);
					col_num++;
					if (ch != '\n')
						temp[i++] = ch;
				} while (ch != '\n');
				temp[i] = '\0';
				strcpy(token.comment, temp);
				ungetc(ch, sourceFile);
				token.number = tcomment;
			}
			else if (ch == '=')  token.number = tdivAssign;
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "/");
				token.number = tdiv;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '"':
			char temp[MAX_LENGTH];
			i = 0;
			ch = fgetc(sourceFile);
			col_num++;
			while (ch != '"')
			{
				if (ch == '\n')
					ch = ' ';
				temp[i++] = ch;
				ch = fgetc(sourceFile);
				col_num++;
			}
			temp[i] = '\0';
			token.number = tstringlit;
			strcpy(token.value, temp);
			token.line_num = line_num;
			token.col_num = col_num;
			strcpy(token.str, temp);
			break;
		case '\'':
			char temp1[MAX_LENGTH];
			ch = fgetc(sourceFile);
			col_num++;
			temp1[0] = ch;
			temp1[1] = '\0';
			token.number = tcharlit;
			strcpy(token.value, temp1);
			token.line_num = line_num;
			token.col_num = col_num;
			strcpy(token.str, temp1);
			ch = fgetc(sourceFile);
			break;
		case '!':
			ch = fgetc(sourceFile);
			if (ch == '=')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "!=");
				token.number = tnotequ;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "!");
				token.number = tnot;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '%':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "%=");
				token.number = tremAssign;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "%");
				token.number = tremainder;
				ungetc(ch, sourceFile);
			}
			break;
		case '&':
			ch = fgetc(sourceFile);
			if (ch == '&')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "&&");
				token.number = tand;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "&");
				lexicalError(2);
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '*':
			ch = fgetc(sourceFile);
			if (ch == '=')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "*=");
				token.number = tmulAssign;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "*");
				token.number = tmul;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '+':
			ch = fgetc(sourceFile);
			if (ch == '+')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "++");
				token.number = tinc;
				col_num++;
			}
			else if (ch == '=')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "+=");
				token.number = taddAssign;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "+");
				token.number = tplus;
				col_num++;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '-':
			ch = fgetc(sourceFile);
			if (ch == '-')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "--");
				token.number = tdec;
				col_num++;
			}
			else if (ch == '=')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "-=");
				token.number = tsubAssign;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "-");
				token.number = tminus;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '<':
			ch = fgetc(sourceFile);
			if (ch == '=')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "<=");
				token.number = tlesse;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "<");
				token.number = tless;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '=':
			ch = fgetc(sourceFile);
			if (ch == '=')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "==");
				token.number = tequal;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, "=");
				token.number = tassign;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '>':
			ch = fgetc(sourceFile);
			if (ch == '=')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, ">=");
				token.number = tgreate;
				col_num++;
			}
			else {
				token.line_num = line_num;
				token.col_num = col_num;
				col_num++;
				strcpy(token.str, ">");
				token.number = tgreat;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '|':
			ch = fgetc(sourceFile);
			if (ch == '|')
			{
				token.line_num = line_num;
				token.col_num = col_num;
				strcpy(token.str, "||");
				token.number = tor;
				col_num++;
			}
			else {
				lexicalError(3);
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '(': token.number = tlparen;	token.line_num = line_num; token.col_num = col_num; strcpy(token.str, "("); col_num++;    break;
		case ')': token.number = trparen;	token.line_num = line_num; token.col_num = col_num; strcpy(token.str, ")"); col_num++;    break;
		case ',': token.number = tcomma;	token.line_num = line_num; token.col_num = col_num; strcpy(token.str, ",");  col_num++;   break;
		case ';': token.number = tsemicolon; token.line_num = line_num; token.col_num = col_num; strcpy(token.str, ";");  col_num++;      break;
		case '[': token.number = tlbracket; token.line_num = line_num; token.col_num = col_num; strcpy(token.str, "[");    col_num++;    break;
		case ']': token.number = trbracket;   token.line_num = line_num; token.col_num = col_num; strcpy(token.str, "]");  col_num++;    break;
		case '{': token.number = tlbrace;    token.line_num = line_num; token.col_num = col_num; strcpy(token.str, "{");   col_num++;    break;
		case '}': token.number = trbrace;    token.line_num = line_num; token.col_num = col_num; strcpy(token.str, "}");   col_num++;    break;
		case ':': token.number = tcolon;    token.line_num = line_num; token.col_num = col_num; strcpy(token.str, ":");    col_num++;   break;
		case EOF: token.number = teof;            break;
		default: {
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}
		} // switch end
	} while (token.number == tnull);
	return token;

} // end of scanner

void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &\n");
		break;
	case 3: printf("next character must be |\n");
		break;
	case 4: printf("invalid character\n");
		break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}

double getNumber(char firstCharacter)
{
	char* temp;
	double num = 0;
	int value;
	char ch;
	int point_count = 1;
	int i = 0;
	temp_str[i++] = firstCharacter;
	if (firstCharacter == '0') {
		ch = fgetc(sourceFile);
		col_num++;
		if (ch != '\n' && isDigitOrPoint(ch))
			temp_str[i++] = ch;
		col_num++;
		if ((ch == 'X') || (ch == 'x')) {		// hexa decimal
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
			{
				if (ch != '\n' && isDigitOrPoint(ch))
					temp_str[i++] = ch;
				col_num++;
				num = 16 * num + value;
			}
		}
		else if ((ch >= '0') && (ch <= '7'))	// octal
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
				col_num++;
				if (ch != '\n' && isDigitOrPoint(ch))
					temp_str[i++] = ch;
				col_num++;
			} while ((ch >= '0') && (ch <= '7'));
		else if (ch == '.' || ch == 'e' || ch == 'E')	//floating point
		{
			{
				point_check = true;
				do {
					if (ch == 'e' || ch == 'E')//using e
					{
						ch = fgetc(sourceFile);
						col_num++;
						if (ch != '\n' && isDigitOrPoint(ch))
							temp_str[i++] = ch;
						int temp = 0;
						do {
							temp = 10 * temp + (int)(ch - '0');
							ch = fgetc(sourceFile);
							col_num++;
							if (ch != '\n' && isDigitOrPoint(ch))
								temp_str[i++] = ch;
							col_num++;
						} while (isDigitOrPoint(ch));
						num *= pow(10, temp);
						if (num == (int)num)
							point_check = false;
					}
					else
					{
						if (ch >= '0' && ch <= '9')
							num += (double)(ch - '0') *(1.0 / (double)pow(10, (point_count++)));
						ch = fgetc(sourceFile);
						col_num++;
						if (ch != '\n' && isDigitOrPoint(ch))
							temp_str[i++] = ch;
						col_num++;
					}
				} while (isDigitOrPoint(ch));
			}
		}
		else num = 0;						// zero
	}
	else {									// decimal
		ch = firstCharacter;
		do {
			if (ch == '.' || ch == 'e' || ch == 'E')					//floating point
			{
				point_check = true;
				do {
					if (ch == 'e' || ch == 'E')//using e
					{
						ch = fgetc(sourceFile);
						col_num++;
						if (ch != '\n' && isDigitOrPoint(ch))
							temp_str[i++] = ch;
						int temp = 0;
						do {
							temp = 10 * temp + (int)(ch - '0');
							ch = fgetc(sourceFile);
							col_num++;
							if (ch != '\n' && isDigitOrPoint(ch))
								temp_str[i++] = ch;
							col_num++;
						} while (isDigitOrPoint(ch));
						num *= pow(10, temp);
						if (num == (int)num)
							point_check = false;
					}
					else
					{
						if (ch >= '0' && ch <= '9')
							num += (double)(ch - '0') *(1.0 / (double)pow(10, (point_count++)));
						ch = fgetc(sourceFile);
						col_num++;
						if (ch != '\n' && isDigitOrPoint(ch))
							temp_str[i++] = ch;
						col_num++;
					}
				} while (isDigitOrPoint(ch));
			}
			else
			{
				num = 10 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
				if (ch != '\n' && isDigitOrPoint(ch))
					temp_str[i++] = ch;
				col_num++;
			}
		} while (isDigitOrPoint(ch));
	}
	ungetc(ch, sourceFile);  /*  retract  */
	return num;
}

int hexValue(char ch)
{
	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10);
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10);
	default: return -1;
	}
}

bool isDigitOrPoint(char c)
{
	if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' || c == 'x' || c == 'X' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
		return true;
	else
		return false;
}

//Scanner.cpp