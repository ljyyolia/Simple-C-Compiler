%{
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include "error.c"//must be included before symbol.c
#include "lex.yy.c"
#include "symbol.c"

extern int e_flag;
extern int e1_flag;
extern int e2_flag;
extern int e3_flag;
extern int e4_flag;
extern int e5_flag;
extern int e6_flag;
extern int e7_flag;
extern int e9_flag;
extern int e10_flag;
extern int e11_flag;
extern int e12_flag;
extern int e13_flag;
extern int e14_flag;
extern int e17_flag;

int yylex();
void yyerror(char *);
#include "ast.c"
#include "intercode.c"
%}
%union{
	struct ast* x;
}
%token <x> INT FLOAT
%token <x> ASSIGNOP AND OR PLUS MINUS STAR DIV LC RC LP RP NOT LB RB DOT
%token <x> ID TYPE RELOP COMMA SEMI RETURN STRUCT IF WHILE ELSE
//declared non-terminals
%type <x> Program ExtDefList ExtDef ExtDecList
%type <x> Specifier StructSpecifier OptTag Tag
%type <x> VarDec FunDec VarList ParamDec
%type <x> CompSt StmtList Stmt
%type <x> DefList Def DecList Dec
%type <x> Exp Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
//priority
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB


%%

//High-level Definitions  Traversal($$,0);
Program:ExtDefList{$$=newast("Program",1,$1);ir_init();trans($$);freesymlist();outir();}
  ;
ExtDefList:ExtDef ExtDefList{$$=newast("ExtDefList",2,$1,$2);}
  |{$$=newast("ExtDefList",0,-1);}
  ;
ExtDef:Specifier ExtDecList SEMI{$$=newast("ExtDef",3,$1,$2,$3);addsymbol1($1->name,$2->namearg,$1->line);}
  |Specifier SEMI{$$=newast("ExtDef",2,$1,$2);}
  |Specifier FunDec CompSt{$$=newast("ExtDef",3,$1,$2,$3);modfunctype($2->name,$1->name);}
  ;
ExtDecList:VarDec{$$=newast("ExtDecList",1,$1);$$->namearg=addnamelist($$,$1->name,$1->arraymark);}
  |VarDec COMMA ExtDecList{$$=newast("ExtDecList",3,$1,$2,$3);$$->namearg=addnamelist($3,$1->name,$1->arraymark);}
  ;

//Specifiers
Specifier:TYPE{$$=newast("Specifier",1,$1);strcpy($$->name,$1->info.name);}
  |StructSpecifier{$$=newast("Specifier",1,$1);strcpy($$->name,$1->name);}
  ;
StructSpecifier:STRUCT OptTag LC DefList RC{$$=newast("StructSpecifier",5,$1,$2,$3,$4,$5);strcpy($$->name,$2->name); 
                          addstruct($2->name,$4->namearg,1,$1->line); strcpy($$->name,$1->info.name);
                          char* name="struct"; modfunctype($2->name,name);
                        //error15($4->functype,$$->line);
}
  |STRUCT Tag{$$=newast("StructSpecifier",2,$1,$2);strcpy($$->name,$2->name); error17($2->name,$1->line);}
  ;
OptTag:ID{$$=newast("OptTag",1,$1);strcpy($$->name,$1->info.name);}
  |{$$=newast("OptTag",0,-1);}
  ;
Tag:ID{$$=newast("Tag",1,$1);strcpy($$->name,$1->info.name);}
  ;

//Declarators
VarDec:ID{$$=newast("VarDec",1,$1);strcpy($$->name,$1->info.name);
        $$->arraymark=0; }
  |VarDec LB INT RB{$$=newast("VarDec",4,$1,$2,$3,$4);strcpy($$->name,$1->name); $$->arraymark=1;}
  ;
FunDec:ID LP VarList RP{$$=newast("FunDec",4,$1,$2,$3,$4);addfunc($1->info.name,$3->namearg,3,$1->line); strcpy($$->name,$1->info.name);}
  |ID LP RP{$$=newast("FunDec",3,$1,$2,$3);addfunc($1->info.name,NULL,3,$1->line); strcpy($$->name,$1->info.name);}
  ;
VarList:ParamDec COMMA VarList{$$=newast("VarList",3,$1,$2,$3);$$->namearg=addnamelist($3,$1->name,$1->arraymark);}
  |ParamDec{$$=newast("VarList",1,$1);$$->namearg=addnamelist($$,$1->name,$1->arraymark);}
  ;
ParamDec:Specifier VarDec{$$=newast("ParamDec",2,$1,$2);$$->arraymark=$2->arraymark; strcpy($2->typename,$1->name);if($2->left->type==2){strcpy($2->left->typename,$1->name);}
                strcpy($$->name,$2->name); $$->namearg=addnamelist($$,$2->name,$2->arraymark); addsymbol1($1->name,$$->namearg,$1->line);}
  ;

//Statements
CompSt:LC DefList StmtList RC{$$=newast("CompSt",4,$1,$2,$3,$4);strcpy($$->functype,$3->functype);}
  ;
StmtList:Stmt StmtList{$$=newast("StmtList",2,$1,$2);
if(!strcmp($1->functype,"int") || !strcmp($1->functype,"float")){ 
  strcpy($$->functype,$1->functype);}
      else if(!strcmp($2->functype,"int") || !strcmp($2->functype,"float")) {strcpy($$->functype,$2->functype);}
      else {strcpy($$->functype,$1->functype);}
      }
  |{$$=newast("StmtList",0,-1);}
  ;
Stmt:Exp SEMI{$$=newast("Stmt",2,$1,$2);}
  |CompSt{$$=newast("Stmt",1,$1);}
  |RETURN Exp SEMI{$$=newast("Stmt",3,$1,$2,$3);strcpy($$->functype,$2->functype);}
  |IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{$$=newast("Stmt",5,$1,$2,$3,$4,$5);}
  |IF LP Exp RP Stmt ELSE Stmt{$$=newast("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
  |WHILE LP Exp RP Stmt{$$=newast("Stmt",5,$1,$2,$3,$4,$5);}
  ;
//Local Definitions
DefList:Def DefList{$$=newast("DefList",2,$1,$2);$$->namearg=linknamelist($1->namearg,$2->namearg); if(!strcmp($1->functype,"no") || !strcmp($2->functype,"no"))
              strcmp($$->functype,"no");}
  |{$$=newast("DefList",0,-1);$$->namearg=NULL;}
  ;
Def:Specifier DecList SEMI{$$=newast("Def",3,$1,$2,$3);addsymbol1($1->name,$2->namearg,$1->line);
                 $$->namearg=$2->namearg; strcpy($$->functype,$2->functype);}
  ;
DecList:Dec{$$=newast("DecList",1,$1);$$->namearg=addnamelist($$,$1->name,$1->arraymark);strcpy($$->functype,$1->functype);}
  |Dec COMMA DecList{$$=newast("DecList",3,$1,$2,$3);$$->namearg=addnamelist($3,$1->name,$1->arraymark);}
  ;
Dec:VarDec{$$=newast("Dec",1,$1);strcpy($$->name,$1->name); $$->arraymark=$1->arraymark;}
  |VarDec ASSIGNOP Exp{$$=newast("Dec",3,$1,$2,$3);strcpy($$->name,$1->name); $$->arraymark=$1->arraymark; strcpy($$->functype,"no");}
  ;

//Expressions
Exp : Exp ASSIGNOP Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,"int",0,0);if(!e1_flag)error6($1->rmark,$1->line); }
  |Exp AND Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,"int",0,0);}
  |Exp OR Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,"int",0,0);}
  |Exp RELOP Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,"int",0,0);}
  |Exp PLUS Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,$1->typename,0,0);if(!e1_flag)error7($1,$3,$1->line);strcpy($$->functype,$1->functype);}
  |Exp MINUS Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,$1->typename,0,0);if(!e1_flag)error7($1,$3,$1->line);strcpy($$->functype,$1->functype);}
  |Exp STAR Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,$1->typename,0,0);if(!e1_flag)error7($1,$3,$1->line);strcpy($$->functype,$1->functype);}
  |Exp DIV Exp{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,$1->typename,0,0);if(!e1_flag)error7($1,$3,$1->line);strcpy($$->functype,$1->functype);}
  |LP Exp RP{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,$2->typename,0,0);}
  |MINUS Exp{$$=newast("Exp",2,$1,$2);setarraytype($$,$2->typename,0,0);}
  |NOT Exp{$$=newast("Exp",2,$1,$2);setarraytype($$,"int",0,0);}
  |ID LP Args RP{$$=newast("Exp",4,$1,$2,$3,$4);strcpy($$->name,$1->info.name);strcpy($$->typename,gettype($1->info.name));error2($1->info.name,$1->line);setarraytype($$,$1->typename,0,1);error9($1->info.name,$3->namearg,$1->line);if(!e2_flag)error11($1->info.name,$1->line);}
  |ID LP RP{$$=newast("Exp",3,$1,$2,$3);$$->type=5;
  if(strcmp($1->info.name,"read")||strcmp($1->info.name,"write")){strcpy($$->name,"orifunc");strcpy($$->typename,"int");}
  else {strcpy($$->typename,gettype($1->info.name));strcpy($$->name,$1->info.name);}
  error2($1->info.name,$1->line);setarraytype($$,$1->typename,0,1);if(!e2_flag)error11($1->info.name,$1->line);}
  |Exp LB Exp RB{$$=newast("Exp",4,$1,$2,$3,$4);setarraytype($$,gettype($1->name),1,0);error10($1->name,$1->line);error12($3->typename,$1->line); }
  |Exp DOT ID{$$=newast("Exp",3,$1,$2,$3);setarraytype($$,judgestruct(gettype($1->name),$3->info.name,$1->line),0,1);}
  |ID {$$=newast("Exp",1,$1);strcpy($$->name,$1->info.name);if(gettype($1->info.name))strcpy($$->typename,gettype($1->info.name));error1($1->info.name,$$->line);$$->type=2;}
  |INT {$$=newast("Exp",1,$1);strcpy($$->name,"no"); setarraytype($$,"int",0,1);$$->type=0;}
  |FLOAT {$$=newast("Exp",1,$1);strcpy($$->name,"no"); setarraytype($$,"float",0,1);$$->type=1;}
  ;
Args : Exp COMMA Args{$$=newast("Args",3,$1,$2,$3);$$->namearg=addnamelist($3,$1->typename,$1->arraymark);}
  |Exp {$$=newast("Args",1,$1);$$->namearg=addnamelist($$,$1->typename,$1->arraymark);}
  ;

%%
int main(int argc,char** argv) {
	initsymbollist();//init cannot be put on the tree of program,must be here
  freopen("out2.s","w",stdout);
  if(argc>1){
    if(!(yyin=fopen(argv[1],"r"))){
      perror(argv[1]);
      return 1;
    }
    yyparse();
  }
  return 0;
    	/*if(argc<=1)
		return 1;
	FILE* f=fopen(argv[1],"r");
	if(!f)
	{
	perror(argv[1]);
	return 1;
	}
	yyrestart(f);
	yyparse();
	return 0;*/
}

void yyerror(char *msg) {   //错误信息
    fprintf(stderr,"Error Type B at line %d: %s\n", yylineno, msg);
}
