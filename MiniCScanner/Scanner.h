/***************************************************************
*      scanner routine for Mini C language                    *
*                                   2003. 3. 10               *
***************************************************************/

#pragma once


#define NO_KEYWORD 16
#define ID_LENGTH 12
#define MAX_LENGTH 1000

struct tokenType {
	char str[ID_LENGTH] = { '\0' };
	int number;
	char value[ID_LENGTH] = { '0' };
	int line_num;
	int col_num;
	int comment_type = 0;
	char comment[MAX_LENGTH] = { '0' };
};


enum tsymbol {
	tnull = -1,
	tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
	/* 0          1            2         3            4          5     */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	/* 6          7            8         9           10         11     */
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	/* 12         13          14        15           16         17     */
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
	/* 18         19          20        21           22         23     */
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	/* 24         25          26        27           28         29     */
	//   ...........    word symbols ................................. //
	/* 30         31          32        33           34         35     */
	tconst, telse, tif, tint, treturn, tvoid,
	/* 36         37          38        39           40         41     */
	twhile, tlbrace, tor, trbrace, tchar, tdouble,
	/* 42         43          44        45           46         47     */
	tfor, tdo, tgoto, tswitch, tcase, tbreak,
	/* 48		  49		  50		51			 52  			   */
	tdefault, tcolon, tcomment, tstringlit, tcharlit
};


struct tokenType scanner();