%{
#include<stdio.h>
#include<stdlib.h>
#include "ast.h"
#include "syntax.tab.h"

int e_flag=1;
%}
%option yylineno
%x COMMENT
SPACE " "
INT ([1-9][0-9]*)|0|0[xX][1-9a-fA-F][0-9a-fA-F]*|0[1-7][0-7]*
FLOAT [+-]?((([1-9][0-9]*)|0)\.[0-9]+)
Exp (([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))(E|e)([+|-]?[0-9]+)
ID ([a-zA-Z]|_)(_|[a-zA-Z]|[0-9])*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
TYPE "int"|"float"
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
%%
{SPACE} {}
"\n" {}
"//" {
	//printf("Comments at Line %d\n",yylineno);
	char c=input();
	while (c!='\n') c=input();
}
"/*" {
	//printf("Comments start at Line %d\n",yylineno);
	char c=input();
	while(!(c=='*'&&(input()=='/'))){
		c=input();
	}
	//printf("Comments end at Line %d\n",yylineno);
}
"*/" {
	e_flag=1;printf("Error type at Line %d:haven't match comment,lack of /*\n",yylineno);
}
{SEMI} {yylval.x=newast("SEMI",0,yylineno);return SEMI;}
{COMMA} {yylval.x=newast("COMMA",0,yylineno);return COMMA;}
{ASSIGNOP} {yylval.x=newast("ASSIGNOP",0,yylineno);return ASSIGNOP;}
{RELOP} {yylval.x=newast("RELOP",0,yylineno);return RELOP;}
{PLUS} {yylval.x=newast("PLUS",0,yylineno);return PLUS;}
{MINUS} {yylval.x=newast("MINUS",0,yylineno);return MINUS;}
{STAR} {yylval.x=newast("STAR",0,yylineno);return STAR;}
{DIV} {yylval.x=newast("DIV",0,yylineno);return DIV;}
{AND} {yylval.x=newast("AND",0,yylineno);return AND;}
{OR} {yylval.x=newast("OR",0,yylineno);return OR;}
{DOT} {yylval.x=newast("DOT",0,yylineno);return DOT;}
{NOT} {yylval.x=newast("NOT",0,yylineno);return NOT;}
{LP} {yylval.x=newast("LP",0,yylineno);return LP;}
{RP} {yylval.x=newast("RP",0,yylineno);return RP;}
{LB} {yylval.x=newast("LB",0,yylineno);return LB;}
{RB} {yylval.x=newast("RB",0,yylineno);return RB;}
{LC} {yylval.x=newast("LC",0,yylineno);return LC;}
{RC} {yylval.x=newast("RC",0,yylineno);return RC;}
{STRUCT} {yylval.x=newast("STRUCT",0,yylineno);return STRUCT;}
{RETURN} {yylval.x=newast("RETURN",0,yylineno);return RETURN;}
{IF} {yylval.x=newast("IF",0,yylineno);return IF;}
{ELSE} {yylval.x=newast("ELSE",0,yylineno);return ELSE;}
{WHILE} {yylval.x=newast("WHILE",0,yylineno);return WHILE;}
{TYPE} {yylval.x=newast("TYPE",0,yylineno);return TYPE;}
{INT} {yylval.x=newast("INT",0,yylineno);return INT;}
{FLOAT} {yylval.x=newast("FLOAT",0,yylineno);return FLOAT;}
{ID} {yylval.x=newast("ID",0,yylineno);return ID;}

0[xX]([0]+)?|0[xX]([g-zG-Z]*[1-9a-fA-F]*)*[g-zG-Z]+([g-zG-Z]*[1-9a-fA-F]*)* {e_flag=0;printf("Error type A at Line %d:illegal hexadecimal number %s\n",yylineno,yytext);}

0[0]|0([8-9]*[0-7]*)*[89]+([8-9]*[0-7]*)* {e_flag=0;printf("Error type A at Line %d:illegal Octal number %s\n",yylineno,yytext);}
\t {}
. { e_flag=0;printf("Error type A at Line %d:Mysterious characters \'%s\'\n",yylineno,yytext);}

%%

