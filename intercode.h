#ifndef INTERCODE_H_INCLUDED
#define INTERCODE_H_INCLUDED
#include "ast.h"
struct paramlist
{
	char* name;
	struct paramlist* next;
};
struct ir
{
	int target;//num of reg1
	int op;//num of op
	int arg1;//reg2
	int arg2;//reg3
	char* funcname;
	char* relop_op;
	char* labelname;
	struct ir* next;
}*ir_head;
int tempcount,labelcount,regcount,firstreg;
char* regname[12];
void ir_init();
char* combine_strings(char *a,char *b);
char* new_temp();
char* new_label();
void outir();
void trans_vardecArray(struct ast* vardec);
void trans_exp(struct ast* exp,char* place);
void trans_args(struct ast* args,struct paramlist* arg_head);
void trans_cond(struct ast* exp,char* labeltrue,char* labelfalse);
void trans_stmt(struct ast* stmt);
void trans_paramdec(struct ast* pdec);
void trans(struct ast* current);

int allocreg(char* name);

#endif
