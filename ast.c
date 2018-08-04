#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include"ast.h"
extern char* yytext;
//#include"symbol.c"
int i;
struct ast *newast(char* name,int num,...){
	va_list valist;//variable arguments
	struct ast *a=(struct ast*)malloc(sizeof(struct ast));//new parent node
	struct ast *temp=(struct ast*)malloc(sizeof(struct ast));
	if(!a){
		yyerror("out of space");
		exit(0);
	}
	a->typename=(char*)malloc(sizeof(char));
	a->functype=(char*)malloc(sizeof(char));
	a->name=(char*)malloc(sizeof(char));
	strcpy(a->functype,"yes");
	va_start(valist,num);
	if(num>0){//nonterminal symbols
		strcpy(a->info.name,name);
		a->type=-1;
		temp=va_arg(valist,struct ast*);//get first node as left child
		a->left=temp;
		a->line=temp->line;//line number equal
		if(num>1){
			for(i=0;i<num-1;i++){//get other nodes as brothers
				temp->right=va_arg(valist,struct ast*);
				temp=temp->right;
			}
		}
	}
	else{//num==0 terminal symbols-param-line num or null-param-(-1).
		int t=va_arg(valist,int);
		a->line=t;
		if(!strcmp(name,"INT")){
			a->type=0;
			a->info.int_v=atoi(yytext);
		}
		else if(!strcmp(name,"FLOAT")){
			a->type=1;
			a->info.float_v=atof(yytext);
			}
		else if(!strcmp(name,"TYPE")){
			a->type=3;
			strcpy(a->info.name,yytext);
		}
		else if(!strcmp(name,"ID")){
			a->type=2;
			strcpy(a->info.name,yytext);
		}
		else if(!strcmp(name,"RELOP")){
			a->type=4;
			strcpy(a->info.name,yytext);
		}
		else{
			a->type=-1;
			strcpy(a->info.name,name);
		}
	}
	return a;
}
void setarraytype(struct ast* ast,char* name,int arraymark,int rmark){//标记数组类型和左右值
	strcpy(ast->typename,name);
	ast->arraymark=arraymark;
	ast->rmark=rmark;
}
struct namelist* addnamelist(struct ast* ast,char* name,int arraymark){
	if(ast->namearg==NULL){
		ast->namearg=(struct namelist*)malloc(sizeof(struct namelist));
		ast->namearg->next=NULL;
		ast->namearg->name=(char*)malloc(sizeof(char));
		ast->namearg->arraymark=arraymark;
		strcpy(ast->namearg->name,name);
	}
	else{
		struct namelist* temp=ast->namearg;
		while(temp->next!=NULL)
			temp=temp->next;
		struct namelist *temp2=(struct namelist*)malloc(sizeof(struct namelist));
		temp2->next=NULL;
		temp2->name=(char*)malloc(sizeof(char));
		strcpy(temp2->name,name);
		temp2->arraymark=arraymark;
		temp->next=temp2;
	}
	return ast->namearg;
}
struct namelist* linknamelist(struct namelist* list1,struct namelist* list2){
	struct namelist* newlist=(struct namelist*)malloc(sizeof(struct namelist));
	struct namelist* temp=newlist;
	struct namelist* t1=list1;
	while(t1!=NULL){
		struct namelist* tt=(struct namelist*)malloc(sizeof(struct namelist));
		tt->name=(char*)malloc(sizeof(char));
		strcpy(tt->name,t1->name);
		temp->next=tt;
		temp=temp->next;
		t1=t1->next;
	}
	t1=list2;
	while(t1!=NULL){
		struct namelist* tt=(struct namelist*)malloc(sizeof(struct namelist));
		tt->name=(char*)malloc(sizeof(char));
		strcpy(tt->name,t1->name);
		temp->next=tt;
		temp=temp->next;
		t1=t1->next;
	}
	temp=NULL;//end of paramlist
	return newlist->next;
}
void Traversal(struct ast *a,int level){
	if(a!=NULL){
		for(i=0;i<level;i++)
			printf("  ");
		if(a->line!=-1){
			if(a->type==0){
				printf("INT:%d",a->info.int_v);
			}
			else if(a->type==1){
				printf("FLOAT:%f",a->info.float_v);
			}
			else if(a->type==2){
				printf("ID:%s",a->info.name);
			}
			else if(a->type==3){
				printf("TYPE:%s",a->info.name);
			}
			else
				printf("%s",a->info.name);
			printf("(%d)",a->line);
		}
		printf("\n");
		Traversal(a->left,level+1);
		Traversal(a->right,level);
	}
}

