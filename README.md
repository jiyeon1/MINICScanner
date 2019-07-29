# MINIC_Scanner
miniC Scanner

<pre>
MiniC 소스코드(*.mc) 파일을 입력으로 받아 Token을 지정된 형식에 따라 출력한다.  


<인식할 수 있는 Token> 
*	연산자 (Operator)    
  	사칙 연산자 : +, -, *, /, %  
  	배정 연산자 : =, +=, -=, *=, /=, %=  
  	논리 연산자 : !, &&, ||  
  	관계 연산자 : ==, !=, <, >, <=, >=  
  	증감 연산자 : ++, --  
  	그 외 : 대괄호('[', ']'), 중괄호('{', '}'), 소괄호('(', ')'), 컴마(','), 세미콜론(';'), 콜론(‘:’)   
    
*	지정어 (Keyword)
  	const, else, if, int, return, void, while, char, double, for, do, goto, switch, case, break, default
  
*	명칭(Identifier)  
  	[a-zA-Z_][a-zA-Z0-9_]* 형태의 명칭을 인식   
  	이미 의미와 용법이 지정되어 있는 예약어는 명칭으로 사용할 수 없다.  
  
*	정수 상수 (Integer Constant)  
  	10진수(Decimal Number) : 0으로 시작하지 않는 일반 숫자  
  	8진수(Octal Number) : 0으로 시작하는 숫자  
  	16진수(Hexa-decimal Number) : 0x로 시작하는 숫자 혹은 알파벳 A-F  
  
  
<인식할 수 있는 주석 (Comment)>    
  *	Single – line Comment : //  
  *	Multi – line Comment : /*~*/  
  *	Documented comments : /** ~ */  
  
  
<인식할 수 있는 literal>  
*	Integer literal  / Character literal / String literal / Double literal  


<token 출력 형식>  
*	Token -> token ( token number, token value, file name, line number, column number )  
*	Documented Comments ->  comment contents  
</pre>
