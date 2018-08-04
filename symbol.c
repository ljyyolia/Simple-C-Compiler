#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"symbol.h"
//#include"error.c"
int e15_flag=0;
void insertsym1(char* name,char* typname,int arraytype){//basic or array
    struct symbol* sym=(struct symbol*)malloc(sizeof(struct symbol));
    sym->name=(char*)malloc(sizeof(char));
    strcpy(sym->name,name);
    sym->typname=(char*)malloc(sizeof(char));
    sym->tempname=(char*)malloc(sizeof(char));
    sym->visited=0;
    strcpy(sym->typname,typname);
    if(arraytype==1)
        sym->functype=2;
    else
        sym->functype=0;
    sym->next=NULL;
    //插入链尾
    struct symbol* tsym=symbollist;
    int count=0;
    while(tsym->next!=NULL){
        tsym=tsym->next;
        count++;
    }
    tsym->next=sym;//symbollist不存
    //printf("INSERT into symbollist type1 name:%s turn %d\n",name,count);
}
void insertsym2(char* name,char* typname,int functype,struct arg* arg){//struct or func
    struct symbol* sym=(struct symbol*)malloc(sizeof(struct symbol));
    sym->name=(char*)malloc(sizeof(char));
    strcpy(sym->name,name);
    sym->typname=(char*)malloc(sizeof(char));
    sym->tempname=(char*)malloc(sizeof(char));
    strcpy(sym->typname,typname);
    sym->functype=functype;
    sym->arg=arg;
    sym->visited=0;
    sym->next=NULL;
    //插入链尾
    struct symbol* tsym=symbollist;
    int count=0;
    while(tsym->next!=NULL){
        tsym=tsym->next;
        count++;
    }
    tsym->next=sym;
    //printf("INSERT into symbollist type2 name:%s turn %d\n",name,count);
}
char* gettype(char* name){
    char* typname=(char*)malloc(sizeof(char));
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(!strcmp(name,sym->name)){
            strcpy(typname,sym->typname);
            return typname;
            break;
        }
        sym=sym->next;
    }
}
int getfunctype(char* name){
    int type=-1;
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(!strcmp(name,sym->name)){
            type=sym->functype;
            return type;
            break;
        }
        sym=sym->next;
    }
}
int lookup(char* name){//查询符号是否在符号表
    int mark=0;
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(!strcmp(name,sym->name)){
            mark=1;
            break;
        }
        sym=sym->next;
    }
    return mark;
}
char* judgestruct(char* name,char* name2,int line){
    int mark=0,mark2=0;
    struct symbol* sym=symbollist;
    sym=sym->next;
    char* result="INT";
    while(sym!=NULL){
        if(!strcmp(name,sym->name) && sym->functype==1){
            mark=1;
            struct arg* arg=sym->arg;
            while(arg!=NULL){
                if(!strcmp(arg->name,name2)){//按name找type
                    mark2=1;
                    strcpy(result,arg->typname);
                    break;
                }
                arg=arg->next;
            }
        }
        if(mark)
            break;
        sym=sym->next;
    }
    if(!mark)
        error13(line);//illegal use of '.'
    else if(!mark2)
        error14(name2,line);//non-existent field
    return result;//return type
}
void symbollisttrace(){
    struct symbol* tsym=symbollist;
    tsym=tsym->next;
    while(tsym!=NULL)
    {
        if(tsym->functype==3){//func
            printf("func:  name:%s  type:%s  functype:%d\n",tsym->name,tsym->typname,tsym->functype);
            struct arg* arg=tsym->arg;
            while(arg!=NULL)
            {
                printf("     name:%s  type:%s\n",arg->name,arg->typname);
                arg=arg->next;
            }
            printf("\n");
        }
        else if(tsym->functype==1){//struct
           printf("struct:  name:%s  type:%s  functype:%d\n",tsym->name,tsym->typname,tsym->functype);
           struct arg* arg=tsym->arg;
           while(arg!=NULL)
           {
               printf("     name:%s  type:%s\n",arg->name,arg->typname);
               arg=arg->next;
           }
           printf("\n");
       }
       else printf("name:%s  type:%s  functype:%d  tempname%s\n",tsym->name,tsym->typname,tsym->functype,tsym->tempname);
       tsym=tsym->next;
   }
}
void addsymbol1(char* name,struct namelist* namelist,int line){//insert a group of . e.g.:int a,b
    struct namelist* t=namelist;
    while(t!=NULL)
    {
        if(!lookup(t->name))//not defined
            insertsym1(t->name,name,t->arraymark);
        else{
            if(!e15_flag)error3(t->name,line);}//redefined var/array
        t=t->next;
    }
}
void addfunc(char* name,struct namelist* namelist,int mark,int line){
    if(lookup(name)==0)
    {
        char* tname="int";
        insertsym2(name,tname,mark,getarg(namelist));
    }
    else
        error4(name,line);//redefined func
}
void addstruct(char* name,struct namelist* namelist,int mark,int line){
    if(lookup(name)==0)
    {
        char* tname="int";
        insertsym2(name,tname,mark,getarg(namelist));
    }
    else
        error16(name,line);//redefined struct
    struct namelist* temp=(struct namelist*)malloc(sizeof(struct namelist));
    temp=namelist;
    char* name2[10];
    int count=0;
    while(temp!=NULL){
     name2[count]=(char*)malloc(sizeof(char));
     strcpy(name2[count],temp->name);
     count++;
     temp=temp->next;
    }
    for(int j=0;j<count;j++){
        for(int k=j+1;k<count;k++){
            if(!strcmp(name2[j],name2[k])){
                error15(name2[j],line);break;e15_flag=1;
            }
        }
    }
}
struct arg* getarg(struct namelist* namelist){//namelist to arglist
    struct arg* arg=(struct arg*)malloc(sizeof(struct arg));
    struct arg* ttt=arg;
    struct namelist* t=namelist;
    while(t!=NULL)
    {
        struct arg* tt=(struct arg*)malloc(sizeof(struct arg));
        tt->name=(char*)malloc(sizeof(char));
        strcpy(tt->name,t->name);
        tt->typname=(char*)malloc(sizeof(char));
        tt->tempname=(char*)malloc(sizeof(char));
        strcpy(tt->typname,gettype(t->name));
        tt->functype=getfunctype(t->name);
        t=t->next;
        ttt->next=tt;
        ttt=ttt->next;
    }
    return arg->next;
}
int checkargs(char* name,struct namelist* namelist){
    int mark=0;
    int result=1;
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(!strcmp(name,sym->name)){
            mark=1;
            struct arg* arg=sym->arg;
            struct namelist* t=namelist;
            while(arg!=NULL&&t!=NULL){
                if(strcmp(arg->typname,t->name)){
                    result=0;//name not match
                    break;
                }
                arg=arg->next;
                t=t->next;
            }
            if(t!=NULL||arg!=NULL)//num not match
                result=0;
        }
        if(mark==1)//have found
            break;
        sym=sym->next;
    }
    return result;
}
void freesymlist()
{
    while(symbollist!=NULL)
    {
        free(symbollist->name);
        free(symbollist->typname);
        struct arg* arg=symbollist->arg;
        while(arg!=NULL)
        {
            free(arg->name);
            free(arg->typname);
            struct arg* tt=arg;
            arg=arg->next;
            free(tt);
        }
        struct symbol* t=symbollist;
        symbollist=symbollist->next;
        free(t);
    }
}
void modfunctype(char* name,char* typname){
    int mark=0;
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL)
    {
        if(!strcmp(name,sym->name))
        {
            strcpy(sym->typname,typname);
            break;
        }
        sym=sym->next;
    }
}
void initsymbollist(){
    symbollist=(struct symbol*)malloc(sizeof(struct symbol));
    symbollist->next=NULL;
    symbollist->name=(char*)malloc(sizeof(char));
    symbollist->typname=(char*)malloc(sizeof(char));
    //define function read
    insertsym2("read","int",3,NULL);
    //define function write
    struct arg* t=(struct arg*)malloc(sizeof(struct arg));
    t->name=(char*)malloc(sizeof(char));
    strcpy(t->name,"out");
    t->typname=(char*)malloc(sizeof(char));
    strcpy(t->typname,"int");
    t->functype=0;
    insertsym2("write","int",3,t);
    insertsym1("no","int",0);
}
void settempname(char* name,char* tempname){
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(!strcmp(name,sym->name)){
            sym->visited=1;
            sym->tempname=(char*)malloc(sizeof(char));
            strcpy(sym->tempname,tempname);
            break;
        }
        sym=sym->next;
    }
}
void setfunctempname(char* argname,char* tempname){
    struct symbol* sym=symbollist;
    sym=sym->next;
    int flag=0;
    while(sym!=NULL){
        if(sym->functype==3){
            struct arg* cur_arg=sym->arg;
            while(cur_arg!=NULL){
                if(!strcmp(argname,cur_arg->name)){
                    strcpy(cur_arg->tempname,tempname);
                    flag=1;
                    break;
                }
                cur_arg=cur_arg->next;
            }
            if(flag)
                break;
        }
        sym=sym->next;
    }
    
}
/*void setfunctempname(char* funcname,char* argname,char* tempname){
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(!strcmp(funcname,sym->name)){
            break;
        }
        sym=sym->next;
    }
    struct arg* cur_arg=sym->arg;
    while(cur_arg!=NULL){
        if(!strcmp(argname,cur_arg->name)){
            strcpy(cur_arg->tempname,tempname);
            break;
        }
    }
}*/
char* gettempname(char* name){
    char* tempname=(char*)malloc(sizeof(char));
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(!strcmp(name,sym->name)){
            strcpy(tempname,sym->tempname);
            return tempname;
            break;
        }
        sym=sym->next;
    }
}
char* getfunctempname(char* argname){
    struct symbol* sym=symbollist;
    sym=sym->next;
    while(sym!=NULL){
        if(sym->functype==3){
            struct arg* cur_arg=sym->arg;
            while(cur_arg!=NULL){
                if(!strcmp(argname,cur_arg->name)){
                    return cur_arg->tempname;
                    break;
                }
                cur_arg=cur_arg->next;
            }
        }
        sym=sym->next;
    }
}
int visited(char* name){
    struct symbol* sym=symbollist;
    sym=sym->next;
    int markarg=0;
    int markbasic=0;
    while(sym!=NULL){
        if(sym->functype==3){//is func
            struct arg* cur_arg=sym->arg;
            while(cur_arg!=NULL){
                if(!strcmp(name,cur_arg->name)){
                    markarg=1;
                    break;
                }
                cur_arg=cur_arg->next;
            }
        }
        else{//not func
            if(!strcmp(name,sym->name)){
                markbasic=1;
                
            }
        }
        sym=sym->next;
    }
    if(markarg)
        return 2;
    if(markbasic)
        return 1;
}
