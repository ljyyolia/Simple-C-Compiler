#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
struct arg//参数表
{
    char* typname;
    char* name;
    int functype;   //0表示普通变量，1表示结构体变量，2表示数组变量
    struct arg* next;
    char* tempname;
};
struct symbol//符号表
{
    struct symbol* next;
    char* name;
    char* typname;
    int functype;   //0表示普通变量，1表示结构体，2表示数组，3表示函数
    struct arg* arg;
    char* tempname;
    int visited;
}*symbollist;//head
//int e15_flag;
void insertsym1(char* name,char* typname,int arraytype);//插入普通变量和数组
void insertsym2(char* name,char* typname,int functype,struct arg* arg);//插入函数和结构体
char* gettype(char* name);    //获得符号的type名字
int getfunctype(char* name);   //获得符号的functype
int lookup(char* name);   //查询符号是否在符号表
char* judgestruct(char* name,char* name2,int line);
void symbollisttrace();   //遍历符号表，并输出
void addsymbol1(char* name,struct namelist* namelist,int line);    //插入一组普通变量
void addfunc(char* name,struct namelist* namelist,int mark,int line);   //添加函数
void addstruct(char* name,struct namelist* namelist,int mark,int line);	//添加结构体
struct arg* getarg(struct namelist* namelist);   //将namelist转换为参数列表
int checkargs(char* name,struct namelist* namelist);	//检查参数是否一致
void freesymlist();    //清空符号表
void modfunctype(char* name,char* typname);     //修改函数和结构体的functype
void initsymbolist();    //初始化符号表
void settempname(char* name,char* tempname);//set tempname by searching name
void setfunctempname(char* argname,char* tempname);//set tempname of func arg
char* gettempname(char* name); //get tempname by searching name
char* getfunctempname(char* argname);//get tempname of func arg
int visited(char* name);//whether visited or not
#endif
