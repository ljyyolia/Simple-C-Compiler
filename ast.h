#ifndef HEAD_H_INCLUDED
#define HEAD_H_INCLUDED
extern int yylineno;
extern char* yytext;
//void yyerror(char *s,...);
union Info{
	char name[50];//除int、float外终结符的token名
	int int_v;//int的值
	float float_v;//float的值
};
//abstract syntax tree
struct ast{
    int line;//行号
    char* name;//综合属性值
    union Info info;
    int arraymark;//数组标记 1:is array
    int rmark;//数组左右值 0:left 1:right
    char* typename;//数组类型名
    char* functype;//函数类型
    int type;//类型号 0 int 1 float 2 id 3 type 4 relop 5 func -1 other
    struct ast *left;
    struct ast *right;
    struct namelist* namearg;//参数列表
};
struct namelist//名字列表(用于结构体成员变量或函数参数)
{
	char* name;//采用名等价机制，所以不需要记录类型
	int arraymark;
	struct namelist* next;
};
void setarraytype(struct ast* ast,char* name,int arraymark,int rmark);//标记数组类型和左右值
struct namelist* addnamelist(struct ast* ast,char* name,int arraymark);//添加到namelist
struct namelist* linknamelist(struct namelist* list1,struct namelist* list2);//连接两个namelist
struct ast *newast(char* name,int num,...);//创建新结点
void Traversal(struct ast*,int level);
#endif
