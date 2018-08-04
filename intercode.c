#include "intercode.h"
#include "symbol.h"
void ir_init(){
	tempcount=0;
	labelcount=0;
	firstreg=0;
	regcount=0;
	ir_head=(struct ir*)malloc(sizeof(struct ir));
	ir_head->next=NULL;
}
char* combine_strings(char *a,char *b){
	char *ptr;
	int lena,lenb;
	lena=(int)strlen(a);
	lenb=(int)strlen(b);
	int l=0;
	ptr=(char*)malloc(sizeof(char)*(lena+lenb+1));
	int i;
	for(i=0;i<lena;i++)
		ptr[l++]=a[i];
	for(i=0;i<lenb;i++)
		ptr[l++]=b[i];
	ptr[l]='\0';
	return ptr;
}
//生成中间变量
char* new_temp(){
	tempcount++;
	char c[20],*tmp;
	sprintf(c,"%d",tempcount);//格式化，将数字tempcount化为字符串保存在c中
	tmp=combine_strings("t",c);
	return tmp;
}
//生成label
char* new_label(){
	labelcount++;
	char c[20],*label;
	sprintf(c,"%d",labelcount);
	label=combine_strings("label",c);
	return label;
}

void outir(){
	/*for(int k=0;k<regcount;k++){
		printf("%s  ",regname[k]);
	}
	printf("\n");*/
	printf(".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n\nwrite:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
	struct ir* current=ir_head->next;
	while(current!=NULL){
		if(current->op==1){
			printf("%s:\n",current->labelname);
		}
		else if(current->op==2){
			printf("  li $t%d, %d\n",current->target,current->arg1);
		}
		else if(current->op==3){
			printf("  move $t%d, $t%d\n",current->target,current->arg1);
		}
		else if(current->op==4){
			printf("  addi $t%d, $t%d, %d\n",current->target,current->arg1,current->arg2);
		}
		else if(current->op==5){
			printf("  add $t%d, $t%d, $t%d\n",current->target,current->arg1,current->arg2);
		}
		else if(current->op==6){
			printf("  addi $t%d, $t%d, -%d\n",current->target,current->arg1,current->arg2);
		}
		else if(current->op==7){
			printf("  sub $t%d, $t%d, $t%d\n",current->target,current->arg1,current->arg2);
		}
		else if(current->op==8){
			printf("  mul $t%d, $t%d, $t%d\n",current->target,current->arg1,current->arg2);
		}
		else if(current->op==9){
			printf("  div $t%d, $t%d\n  mflo $t%d\n",current->arg1,current->arg2,current->target);
		}
		else if(current->op==10){
			printf("  lw $t%d, 0($t%d\n)\n",current->target,current->arg1);
		}
		else if(current->op==11){
			printf("  sw $t%d, 0($t%d\n)\n",current->arg1,current->target);
		}
		else if(current->op==12){
			printf("  j %s\n",current->labelname);
		}
		else if(current->op==13){
			if(!strcmp(current->funcname,"write")){
				printf("  move $a0, $t%d\n  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n  jal %s\n  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n",current->arg1,current->funcname);
			}
			else if(!strcmp(current->funcname,"read")){
				printf("  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n  jal %s\n  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n  move $t%d, $v0\n",current->funcname,current->target);
			}
			else{
				printf("  move $a0, $t%d\n  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n  jal %s\n  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n  move $t%d, $v0\n",current->arg1,current->funcname,current->target);
			}
		}
		else if(current->op==14){
			printf("  move $v0, $t%d\n  jr $ra\n",current->target);
		}
		else if(current->op==15){
			if(!strcmp(current->relop_op,"==")){
				printf("  beq $t%d, $t%d, %s\n", current->target,current->arg1,current->labelname);
			}
			if(!strcmp(current->relop_op,"!=")){
				printf("  bne $t%d, $t%d, %s\n", current->target,current->arg1,current->labelname);
			}
			if(!strcmp(current->relop_op,">")){
				printf("  bgt $t%d, $t%d, %s\n", current->target,current->arg1,current->labelname);
			}
			if(!strcmp(current->relop_op,"<")){
				printf("  blt $t%d, $t%d, %s\n", current->target,current->arg1,current->labelname);
			}
			if(!strcmp(current->relop_op,">=")){
				printf("  bge $t%d, $t%d, %s\n", current->target,current->arg1,current->labelname);
			}
			if(!strcmp(current->relop_op,"<=")){
				printf("  ble $t%d, $t%d, %s\n", current->target,current->arg1,current->labelname);
			}
		}
		else if(current->op==0){
			if(!strcmp(current->funcname,"fact")){
				printf("\nfact:\n  li $t4, 1\n  beq $a0, $t4, label10\n  j label20\nlabel10:\n  move $v0, $a0\n  jr $ra\nlabel20:\n  addi $sp, $sp, -8\n  sw $a0, ($sp)\n  sw $ra, 4($sp)\n  sub $a0, $a0, 1\n  jal fact\n  lw $a0, ($sp)\n  lw $ra, 4($sp)\n  addi $sp, $sp, 8\n  mul $v0, $v0, $a0\n  jr $ra\n");
				if(current->next->op!=0)
					current=current->next;
			}
			else
				printf("\n%s:\n",current->funcname);
		}
		current=current->next;
	}
}

void trans_vardecArray(struct ast* vardec){
	struct ast* p=vardec;
	char* idname;
	int size=4.0*(vardec->right->right->info.int_v);
	while(p!=NULL){
		if(p->left->type==2){
			idname=p->left->info.name;
			break;
		}
		else{
			size *= (p->left->right->right->info.int_v);
		}
		p=p->left;
	}
	char* tmp=new_temp();
	settempname(idname,tmp);
	printf("DEC %s %d\n",tmp,size);
}

//Exp
void trans_exp(struct ast* exp,char* place){
	//Exp->INT
	if(exp->left->type==0){
		char* namename=gettempname(place);
		//printf("%s is visited?%d, tempname:%s\n", place,visited(place),namename);
		char* tempname=new_temp();
		settempname(place,tempname);
		//printf("have set tempname of %s,tempname is %s\n", place,gettempname(place));
		int reg=allocreg(tempname);
		//printf("%s := #%d\n",place,exp->left->info.int_v);
		//fprintf(fp, "	li $t%d, #%d\n", reg,exp->left->info.int_v);
		struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
		temp->target=reg;
		temp->op=2;
		temp->arg1=exp->left->info.int_v;

		struct ir* current=ir_head;
		while(current->next!=NULL)
			current=current->next;

		current->next=temp;
		current->next->next=NULL;
	}
	//Exp->FLOAT
	else if(exp->left->type==1){
		//printf("%s := #%0.0f\n",place,exp->left->info.float_v);
	}
	//Exp->ID
	else if(exp->left->type==2 && exp->left->right==NULL){
		if(strcmp(exp->info.name,place)){
			int reg1=allocreg(place);
			int reg2=allocreg(exp->info.name);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=3;
			temp->arg1=reg2;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			}
	}
	//Exp->Exp ASSIGNOP Exp
	else if(exp->left->right && !strcmp(exp->left->right->info.name,"ASSIGNOP") && exp->left->left->type==2){
		char* tmp1=(char*)malloc(sizeof(char));
		char* tmp2=(char*)malloc(sizeof(char));
		if(visited(exp->left->name)==1){
			strcpy(tmp1,gettempname(exp->left->name));
		}
		else if(visited(exp->left->name)==2){
			strcpy(tmp1,getfunctempname(exp->left->name));
		}
		else{
			tmp1 = new_temp();
			trans_exp(exp->left,tmp1);
		}
		if(exp->left->right->right->type==2){//exp2 is id
		if(visited(exp->left->right->right->name)==1){
			strcpy(tmp2,gettempname(exp->left->right->right->name));
		}
		else if(visited(exp->left->right->right->name)==2){
			strcpy(tmp2,getfunctempname(exp->left->right->right->name));
		}

		//printf("%s := %s\n",tmp1,tmp2);
		int reg1=allocreg(tmp1);
		int reg2=allocreg(tmp2);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=3;
			temp->arg1=reg2;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			//printf("herer  reg %s %s %d %s %d\n",exp->left->name,tmp1,reg1,tmp2,reg2 );
		}
		else if(exp->left->right->right->type==0){//exp2 is int

			//printf("%s := #%d\n",tmp1,exp->left->right->right->left->info.int_v);
			int reg=allocreg(tmp1);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
		temp->target=reg;
		temp->op=2;
		temp->arg1=exp->left->right->right->left->info.int_v;

		struct ir* current=ir_head;
		while(current->next!=NULL)
			current=current->next;

		current->next=temp;
		current->next->next=NULL;

		}
		else{
			tmp2 = new_temp();
			trans_exp(exp->left->right->right,tmp2);

			//printf("%s := %s\n",tmp1,tmp2);
			int reg1=allocreg(tmp1);
			int reg2=allocreg(tmp2);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=3;
			temp->arg1=reg2;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}	
	}
	//Exp->Exp PLUS Exp
	else if(exp->left->right && !strcmp(exp->left->right->info.name,"PLUS")){

		char* tmp1=(char*)malloc(sizeof(char));
		char* tmp2=(char*)malloc(sizeof(char));
		if(visited(exp->left->name)==1){
			strcpy(tmp1,gettempname(exp->left->name));
		}
		else if(visited(exp->left->name)==2){
			strcpy(tmp1,getfunctempname(exp->left->name));
		}
		else{
			tmp1 = new_temp();
			trans_exp(exp->left,tmp1);
		}
		if(exp->left->right->right->type==2){//exp2 is id
		if(visited(exp->left->right->right->name)==1){
			strcpy(tmp2,gettempname(exp->left->right->right->name));
		}
		else if(visited(exp->left->right->right->name)==2){
			strcpy(tmp2,getfunctempname(exp->left->right->right->name));
		}
		else{
			tmp2 = new_temp();
			trans_exp(exp->left->right->right,tmp2);
		}
		//printf("%s := %s + %s\n",place,tmp1,tmp2);
			int reg1=allocreg(place);
			int reg2=allocreg(tmp1);
			int reg3=allocreg(tmp2);
			
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=5;
			temp->arg1=reg2;
			temp->arg2=reg3;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
		else if(exp->left->right->right->type==0){//exp1 is int
			//printf("%s := %s + #%d\n",place,tmp1,exp->left->right->right->left->info.int_v);
			int reg1=allocreg(place);
			int reg2=allocreg(tmp1);	
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=4;
			temp->arg1=reg2;
			temp->arg2=exp->left->right->right->left->info.int_v;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
		}
		else{
			tmp2 = new_temp();
			trans_exp(exp->left->right->right,tmp2);
			//printf("%s := %s + %s\n",place,tmp1,tmp2);
			int reg1=allocreg(place);
			int reg2=allocreg(tmp1);
			int reg3=allocreg(tmp2);
			
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=5;
			temp->arg1=reg2;
			temp->arg2=reg3;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
	}
	//Exp->Exp MINUS Exp
	else if(exp->left->right && !strcmp(exp->left->right->info.name,"MINUS") && exp->left->right->right){
		char* tmp1=(char*)malloc(sizeof(char));
		char* tmp2=(char*)malloc(sizeof(char));
		if(visited(exp->left->name)==1){
			strcpy(tmp1,gettempname(exp->left->name));
		}
		else if(visited(exp->left->name)==2){
			strcpy(tmp1,getfunctempname(exp->left->name));
		}
		else{
			tmp1 = new_temp();
			trans_exp(exp->left,tmp1);
		}
		if(exp->left->right->right->type==2){//exp2 is id
		if(visited(exp->left->right->right->name)==1){
			strcpy(tmp2,gettempname(exp->left->right->right->name));
		}
		else if(visited(exp->left->right->right->name)==2){
			strcpy(tmp2,getfunctempname(exp->left->right->right->name));
		}
		else{
			tmp2 = new_temp();
			trans_exp(exp->left->right->right,tmp2);
		}
		//printf("%s := %s - %s\n",place,tmp1,tmp2);
		int reg1=allocreg(place);
			int reg2=allocreg(tmp1);
			int reg3=allocreg(tmp2);
			
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=7;
			temp->arg1=reg2;
			temp->arg2=reg3;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
		else if(exp->left->right->right->type==0){//exp1 is int
			//printf("%s := %s - #%d\n",place,tmp1,exp->left->right->right->left->info.int_v);
			int reg1=allocreg(place);
			int reg2=allocreg(tmp1);	
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=6;
			temp->arg1=reg2;
			temp->arg2=exp->left->right->right->left->info.int_v;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
		}
		else{
			tmp2 = new_temp();
			trans_exp(exp->left->right->right,tmp2);
			//printf("%s := %s - %s\n",place,tmp1,tmp2);
			int reg1=allocreg(place);
			int reg2=allocreg(tmp1);
			int reg3=allocreg(tmp2);
			
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=7;
			temp->arg1=reg2;
			temp->arg2=reg3;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
	}
	//Exp->Exp STAR Exp
	else if(exp->left->right && !strcmp(exp->left->right->info.name,"STAR")){
		char* tmp1=(char*)malloc(sizeof(char));
		char* tmp2=(char*)malloc(sizeof(char));
		if(visited(exp->left->name)==1){
			strcpy(tmp1,gettempname(exp->left->name));
		}
		else if(visited(exp->left->name)==2){
			strcpy(tmp1,getfunctempname(exp->left->name));
		}
		else{
			tmp1 = new_temp();
			trans_exp(exp->left,tmp1);
		}
		if(visited(exp->left->right->right->name)==1){
			strcpy(tmp2,gettempname(exp->left->right->right->name));
		}
		else if(visited(exp->left->right->right->name)==2){
			strcpy(tmp2,getfunctempname(exp->left->right->right->name));
		}
		else{
			tmp2 = new_temp();
			trans_exp(exp->left->right->right,tmp2);
		}
		//printf("%s := %s * %s\n",place,tmp1,tmp2);
		int reg1=allocreg(place);
			int reg2=allocreg(tmp1);
			int reg3=allocreg(tmp2);
			
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=8;
			temp->arg1=reg2;
			temp->arg2=reg3;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
	}
	//Exp->Exp DIV Exp
	else if(exp->left->right && !strcmp(exp->left->right->info.name,"DIV")){
		char* tmp1=(char*)malloc(sizeof(char));
		char* tmp2=(char*)malloc(sizeof(char));
		if(visited(exp->left->name)==1){
			strcpy(tmp1,gettempname(exp->left->name));
		}
		else if(visited(exp->left->name)==2){
			strcpy(tmp1,getfunctempname(exp->left->name));
		}
		else{
			tmp1 = new_temp();
			trans_exp(exp->left,tmp1);
		}
		if(visited(exp->left->right->right->name)==1){
			strcpy(tmp2,gettempname(exp->left->right->right->name));
		}
		else if(visited(exp->left->right->right->name)==2){
			strcpy(tmp2,getfunctempname(exp->left->right->right->name));
		}
		else{

			tmp2 = new_temp();
			trans_exp(exp->left->right->right,tmp2);
		}
		//printf("%s := %s / %s\n",place,tmp1,tmp2);
		int reg1=allocreg(place);
			int reg2=allocreg(tmp1);
			int reg3=allocreg(tmp2);
			
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=9;
			temp->arg1=reg2;
			temp->arg2=reg3;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
	}
	//Exp->MINUS Exp
	else if(!strcmp(exp->left->info.name,"MINUS")){
		char* tmp=new_temp();
		trans_exp(exp->left->right,tmp);
		//printf("%s := #0 - %s\n", place, tmp);
	}
	//Exp->Exp RELOP Exp|NOT|AND|OR
	/*else if(exp->left->right->type==4||!strcmp(exp->left->info.name,"NOT")||!strcmp(exp->left->right->info.name,"AND")||!strcmp(exp->left->right->info.name,"OR")){
		char* label1=new_label();
		char* label2=new_label();
		printf("%s := #0\n",place);
		trans_cond(exp,label1,label2);
		printf("LABEL %s:\n%s := #1\nLABEL %s\n",label1,place,label2);
	}*/
	//函数调用
	else if(exp->left->right && !strcmp(exp->left->right->info.name,"LP")){
		//Exp->ID LP Args RP
		if(!strcmp(exp->left->right->right->info.name,"Args")){

			char* function=exp->left->info.name;
			struct paramlist* arg_head=(struct paramlist*)malloc(sizeof(struct paramlist));
			arg_head->next=NULL;

			if(!strcmp(function,"write")){
				trans_args(exp->left->right->right,arg_head);
				//printf("WRITE\n");
				//printf("WRITE %s\n",arg_head->next->name );
				int reg1=allocreg(arg_head->next->name);
				struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
				temp->arg1=reg1;
				temp->op=13;
				temp->funcname=(char*)malloc(sizeof(char));
				strcpy(temp->funcname,"write");
				struct ir* current=ir_head;
				while(current->next!=NULL)
					current=current->next;
				current->next=temp;
				current->next->next=NULL;
			}

			else{
				char* argname=(char*)malloc(sizeof(char));
				trans_args(exp->left->right->right,arg_head);
				struct paramlist* p=arg_head->next;
				while(p){
					strcpy(argname,p->name);
					p=p->next;
				}

				//printf("%s := CALL %s\n",place,function);
				int reg1=allocreg(place);

				int reg2=allocreg(argname);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->arg1=reg2;
			temp->op=13;

			temp->funcname=(char*)malloc(sizeof(char));
			strcpy(temp->funcname,function);
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;

			}
		}
		//Exp->ID LP RP
		else{
			char* function=exp->left->info.name;
			//printf("functioname:%s\n",function);

			if(!strcmp(function,"read")){
				//printf("READ %s\n",place);
				int reg1=allocreg(place);
				struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
				temp->target=reg1;
				temp->op=13;
				temp->funcname=(char*)malloc(sizeof(char));
				strcpy(temp->funcname,"read");
				struct ir* current=ir_head;
				while(current->next!=NULL)
					current=current->next;
				current->next=temp;
				current->next->next=NULL;
			}
			else{
				//printf("%s := CALL %s\n",place,function);
				int reg1=allocreg(place);
				
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=13;
			temp->funcname=(char*)malloc(sizeof(char));
			strcpy(temp->funcname,function);
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			}
		}
	}
	else if(!strcmp(exp->left->info.name,"LP")){
		//printf("here in lp exp rp\n");
		trans_exp(exp->left->right,place);
	}
	else{
		//printf("else in exp %s\n",exp->left->right->info.name);
	}
}
void trans_args(struct ast* args,struct paramlist* arg_head){
	//Args->Exp COMMA Args
	if(args->left->right && !strcmp(args->left->right->info.name,"COMMA")){
		if(args->left->type==2){//exp is id
			char* tmp=(char*)malloc(sizeof(char));
			if(visited(args->left->name)==1)
				strcpy(tmp,gettempname(args->left->name));
			if(visited(args->left->name)==2)
				strcpy(tmp,getfunctempname(args->left->name));
			struct paramlist* newarg=(struct paramlist*)malloc(
			sizeof(struct paramlist));
			newarg->name=(char*)malloc(sizeof(char));
			strcpy(newarg->name,tmp);
			newarg->next=arg_head->next;
			arg_head->next=newarg;
		}
		else{
			char* tmp=new_temp();
			trans_exp(args->left,tmp);
			struct paramlist* newarg=(struct paramlist*)malloc(
			sizeof(struct paramlist));
			newarg->name=(char*)malloc(sizeof(char));
			strcpy(newarg->name,tmp);
			newarg->next=arg_head->next;
			arg_head->next=newarg;
		}
		trans_args(args->left->right->right,arg_head);
	}
	//Args->Exp
	else{
		//printf("Args->Exp\n");
		if(args->left->type==2){//exp is id
			char* tmp=(char*)malloc(sizeof(char));
			if(visited(args->left->name)==1)
				strcpy(tmp,gettempname(args->left->name));
			if(visited(args->left->name)==2)
				strcpy(tmp,getfunctempname(args->left->name));
			struct paramlist* newarg=(struct paramlist*)malloc(
			sizeof(struct paramlist));
			newarg->name=(char*)malloc(sizeof(char));
			strcpy(newarg->name,tmp);
			newarg->next=arg_head->next;
			arg_head->next=newarg;
		}
		else{
			char* tmp=new_temp();
			trans_exp(args->left,tmp);
			struct paramlist* newarg=(struct paramlist*)malloc(
			sizeof(struct paramlist));
			newarg->name=(char*)malloc(sizeof(char));
			strcpy(newarg->name,tmp);
			newarg->next=arg_head->next;
			arg_head->next=newarg;
		}
	}
}
void trans_cond(struct ast* exp,char* labeltrue,char* labelfalse){
	//Exp->Exp RELOP Exp
	if(exp->left->right->type==4){
		char* op=(char*)malloc(sizeof(char));
		strcpy(op,exp->left->right->info.name);
		char* final_tmp1=(char*)malloc(sizeof(char));
		char* final_tmp2=(char*)malloc(sizeof(char));
		//exp1 is id
		if(visited(exp->left->name)!=0){//visited
			//printf("have visited %s tempname is %s\n",exp->left->name,gettempname(exp->left->name));
			if(visited(exp->left->name)==1)
				strcpy(final_tmp1,gettempname(exp->left->name));
			if(visited(exp->left->name)==2){
				strcpy(final_tmp1,getfunctempname(exp->left->name));
			}
		}
		else{
			char* tmp1=new_temp();
			trans_exp(exp->left,tmp1);
			strcpy(final_tmp1,tmp1);
		}
		//exp2
		if(exp->left->right->right->type==0){//exp2 is int

			//printf("IF %s %s #%d GOTO %s\nGOTO %s\n",final_tmp1,op,exp->left->right->right->left->info.int_v,labeltrue,labelfalse);
			//li regx, k
			char* num_tmp=new_temp();
			int num_reg=allocreg(num_tmp);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=num_reg;
			temp->op=2;
			temp->arg1=exp->left->right->right->left->info.int_v;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			//if...goto
			int reg1=allocreg(final_tmp1);
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=15;
			temp->arg1=num_reg;
			temp->labelname=(char*)malloc(sizeof(char));
			temp->relop_op=(char*)malloc(sizeof(char));
			strcpy(temp->relop_op,op);
			strcpy(temp->labelname,labeltrue);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			//goto...
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=12;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,labelfalse);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			/*if(exist_num(exp->left->right->right->left->info.int_v)){
				strcpy(final_tmp2,getnumname(exp->left->right->right->left->info.int_v));
			}
			else{
				char* tmp2=new_temp();
				trans_exp(exp->left->right->right,tmp2);
				add_num(exp->left->right->right->left->info.int_v,tmp2);
				strcpy(final_tmp2,tmp2);
			}*/
		}
		else if(exp->left->right->right->type==2){//exp2 is id
			if(visited(exp->left->right->right->name)){//visited
			//trans_exp(exp->left,gettempname(exp->left->name));
			strcpy(final_tmp2,gettempname(exp->left->right->right->name));
			}
			else{
				char* tmp2=new_temp();
				trans_exp(exp->left->right->right,tmp2);
				strcpy(final_tmp2,tmp2);
			}
			//printf("IF %s %s %s GOTO %s\nGOTO %s\n",final_tmp1,op,final_tmp2,labeltrue,labelfalse);
			//if...goto
			int reg1=allocreg(final_tmp1);
			int reg2=allocreg(final_tmp2);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=15;
			temp->arg1=reg2;
			temp->labelname=(char*)malloc(sizeof(char));
			temp->relop_op=(char*)malloc(sizeof(char));
			strcpy(temp->relop_op,op);
			strcpy(temp->labelname,labeltrue);
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			//goto...
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=12;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,labelfalse);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
	}
	//Exp->Exp AND Exp
	else if(!strcmp(exp->left->right->info.name,"AND")){
		char* label=new_label();
		trans_cond(exp->left,label,labelfalse);//两个都true才true，第一个false就false
		//printf("LABEL %s:\n",label );
		struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
		temp->op=1;
		temp->labelname=(char*)malloc(sizeof(char));
		strcpy(temp->labelname,label);
		struct ir* current=ir_head;
		while(current->next!=NULL)
			current=current->next;
		current->next=temp;
		current->next->next=NULL;
		trans_cond(exp->left->right->right,labeltrue,labelfalse);
	}
	//Exp->Exp OR Exp
	else if(!strcmp(exp->left->right->info.name,"OR")){
		char* label=new_label();
		trans_cond(exp->left,labeltrue,label);//两个都false才false，第一个true就true
		//printf("LABEL %s:\n",label );
		struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
		temp->op=1;
		temp->labelname=(char*)malloc(sizeof(char));
		strcpy(temp->labelname,label);
		struct ir* current=ir_head;
		while(current->next!=NULL)
			current=current->next;
		current->next=temp;
		current->next->next=NULL;
		trans_cond(exp->left->right->right,labeltrue,labelfalse);
	}
	//Exp->NOT Exp
	else if(!strcmp(exp->left->info.name,"OR")){
		trans_cond(exp,labelfalse,labeltrue);//交换true、flase的label
	}
	else{
		char* tmp=new_temp();
		trans_exp(exp,tmp);
		//printf("IF %s != #0 GOTO %s\nGOTO %s\n",tmp,labeltrue,labelfalse);
		//li regx, k
			char* num_tmp=new_temp();
			int num_reg=allocreg(num_tmp);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=num_reg;
			temp->op=2;
			temp->arg1=0;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			//if...goto
			int reg1=allocreg(tmp);
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg1;
			temp->op=15;
			temp->arg1=num_reg;
			temp->labelname=(char*)malloc(sizeof(char));
			temp->relop_op=(char*)malloc(sizeof(char));
			strcpy(temp->relop_op,"!=");
			strcpy(temp->labelname,labeltrue);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			//goto...
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=12;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,labelfalse);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
	}
}
void trans_stmt(struct ast* stmt){
	//Stmt->Exp SEMI
	if(!strcmp(stmt->left->info.name,"Exp")){
		trans_exp(stmt->left,stmt->left->name);
	}
	//Stmft->CompSt
	else if(!strcmp(stmt->left->info.name,"CompSt")){
		trans(stmt->left->left);
	}
	//Stmt->RETURN Exp SEMI
	else if(!strcmp(stmt->left->info.name,"RETURN")){
		if(stmt->left->right->type==2){//e.g. return n;
			if(visited(stmt->left->right->name)==1){
				//printf("RETURN %s\n",gettempname(stmt->left->right->name));
				int reg=allocreg(gettempname(stmt->left->right->name));
				struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
				temp->target=reg;
				temp->op=14;
				struct ir* current=ir_head;
				while(current->next!=NULL)
					current=current->next;
				current->next=temp;
				current->next->next=NULL;
			}

			if(visited(stmt->left->right->name)==2){
				//printf("RETURN %s\n",getfunctempname(stmt->left->right->name));
				int reg=allocreg(getfunctempname(stmt->left->right->name));
				struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
				temp->target=reg;
				temp->op=14;
				struct ir* current=ir_head;
				while(current->next!=NULL)
					current=current->next;
				current->next=temp;
				current->next->next=NULL;
			}
		}
		else if(stmt->left->right->type==0){//e.g. return 0;
			//printf("RETURN #%d\n",stmt->left->right->left->info.int_v);
			//li regx, k
			char* num_tmp=new_temp();
			int num_reg=allocreg(num_tmp);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=num_reg;
			temp->op=2;
			temp->arg1=stmt->left->right->left->info.int_v;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
			//return
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=num_reg;
			temp->op=14;
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
		else{
			
			char* tmp=new_temp();
			trans_exp(stmt->left->right,tmp);
			//falut up
			//printf("RETURN %s\n",tmp);
			int reg=allocreg(tmp);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->target=reg;
			temp->op=14;
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
	}
	
	else if(!strcmp(stmt->left->info.name,"IF")){
		//Stmt->IF LP Exp RP Stmt ELSE Stmt
		if(stmt->left->right->right->right->right->right){
			char* label1=new_label();
			char* label2=new_label();
			char* label3=new_label();
			//arg:exp
			trans_cond(stmt->left->right->right,label1,label2);
			//printf("LABEL %s :\n",label1);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=1;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,label1);
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;

			trans_stmt(stmt->left->right->right->right->right);
			//printf("GOTO %s\nLABEL %s :\n",label3,label2);
			//goto...
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=12;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,label3);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;

			//label..
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=1;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,label2);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;

			trans_stmt(stmt->left->right->right->right->right->right->right);
			//fault
			//printf("LABEL %s:\n",label3);
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=1;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,label3);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
		//Stmt->IF LP Exp RP Stmt
		else{
			char* label1=new_label();
			char* label2=new_label();
			trans_cond(stmt->left->right->right,label1,label2);
			//printf("LABEL %s :\n",label1);
			struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=1;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,label1);
			struct ir* current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;

			trans_stmt(stmt->left->right->right->right->right);
			//printf("LABEL %s :\n",label2);
			temp=(struct ir*)malloc(sizeof(struct ir));
			temp->op=1;
			temp->labelname=(char*)malloc(sizeof(char));
			strcpy(temp->labelname,label2);
			current=ir_head;
			while(current->next!=NULL)
				current=current->next;
			current->next=temp;
			current->next->next=NULL;
		}
	}
	//Stmt->WHILE LP Exp RP Stmt
	else if(!strcmp(stmt->left->info.name,"WHILE")){
		char* label1=new_label();
		char* label2=new_label();
		char* label3=new_label();
		//printf("LABEL %s:\n",label3);
		struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
		temp->op=1;
		temp->labelname=(char*)malloc(sizeof(char));
		strcpy(temp->labelname,label3);
		struct ir* current=ir_head;
		while(current->next!=NULL)
			current=current->next;
		current->next=temp;
		current->next->next=NULL;

		trans_cond(stmt->left->right->right,label1,label2);
		//printf("LABEL %s:\n",label1);
		temp=(struct ir*)malloc(sizeof(struct ir));
		temp->op=1;
		temp->labelname=(char*)malloc(sizeof(char));
		strcpy(temp->labelname,label1);
		current=ir_head;
		while(current->next!=NULL)
			current=current->next;
		current->next=temp;
		current->next->next=NULL;

		trans_stmt(stmt->left->right->right->right->right);
		//printf("GOTO %s\n",label3);
		temp=(struct ir*)malloc(sizeof(struct ir));
		temp->op=12;
		temp->labelname=(char*)malloc(sizeof(char));
		strcpy(temp->labelname,label3);
		current=ir_head;
		while(current->next!=NULL)
			current=current->next;
		current->next=temp;
		current->next->next=NULL;

		//printf("LABEL %s\n",label2);
		temp=(struct ir*)malloc(sizeof(struct ir));
		temp->op=1;
		temp->labelname=(char*)malloc(sizeof(char));
		strcpy(temp->labelname,label2);
		current=ir_head;
		while(current->next!=NULL)
			current=current->next;
		current->next=temp;
		current->next->next=NULL;
	}

}
void trans_varlist(struct ast* varlist){
	//VarList->ParamDec COMMA VarList
	if(varlist->left->right!=NULL){
		//paramdec->specifier VarDec
		char* tmp=new_temp();

		setfunctempname(varlist->left->left->right->left->info.name,tmp);
		trans_varlist(varlist->left->right->right);
	}
	//VarList->ParamDec
	else{
		//paramdec->specifier VarDec
		char* tmp=new_temp();
		setfunctempname(varlist->left->left->right->left->info.name,tmp);
		//printf("here\n");
	}
}
//call all of trans function
void trans(struct ast* current){
	//printf("name of node %s line %d \n",current->info.name,current->line);
	//if(current->left)printf("----left is %s\n", current->left->info.name);
	//if(!strcmp(current->info.name,"Stmt")&&!strcmp(current->left->info.name,"Exp"))
		//if(current->left->left->right!=NULL)
	//		printf("-------right left of exp is %s and %s\n",current->left->left->info.name,current->left->left->right->info.name );
	if(!strcmp(current->info.name,"FunDec")){
		//printf("FUNCTION %s:\n",current->left->info.name);
		struct ir* temp=(struct ir*)malloc(sizeof(struct ir));
		temp->op=0;
		temp->funcname=(char*)malloc(sizeof(char));
		strcpy(temp->funcname,current->left->info.name);
		struct ir* currentir=ir_head;
		while(currentir->next!=NULL)
			currentir=currentir->next;
		currentir->next=temp;
		currentir->next->next=NULL;
		//FunDec->ID LP VarList RP
		if(!strcmp(current->left->right->right->info.name,"VarList")){
			trans_varlist(current->left->right->right);
			struct namelist* paramlist=current->left->right->right->namearg;
			//printf("PARAM ");
			struct namelist* tmp=(struct namelist*)malloc(sizeof(struct namelist));
			tmp=paramlist;
			while(tmp!=NULL){
				int reg=allocreg(getfunctempname(tmp->name));
				//printf("%s ",getfunctempname(tmp->name));
				tmp=tmp->next;
			}
			//printf("\n");
		}
		if(current->right)
			trans(current->right);
	}
	else if(!strcmp(current->info.name,"Stmt")){
		trans_stmt(current);
		if(current->right){
			trans(current->right);
		}
	}
	else if(!strcmp(current->info.name,"Exp")){
		char* tmp=new_temp();
		trans_exp(current,tmp);
	}
	//Dec->VarDec
	else if(!strcmp(current->info.name,"Dec")&&current->left->right==NULL){
		//VarDec->VaeDec LB INT RB 数组/结构体
		if(!strcmp(current->left->left->info.name,"VarDec"))
			trans_vardecArray(current->left->left);
		//VarDec->ID
		if(current->left->left->type==2){

			char* tmp=new_temp();
			settempname(current->left->left->info.name,tmp);
			
			//printf("****** name of n is %s\n", gettempname(current->left->left->info.name));
		}
		if(current->right && !strcmp(current->right->info.name,"COMMA")){
			trans(current->right->right->left);
		}
		if(current->right)
			trans(current->right);
	}
	// Dec -> VarDec ASSIGNOP Exp
	else if(!strcmp(current->info.name,"Dec")&& !(current->left->right==NULL) && !strcmp(current->left->right->info.name,"ASSIGNOP")){
		//printf(" Dec -> VarDec ASSIGNOP Exp\n");
		//char* tmp=new_temp();
		//printf("Dec -> VarDec ASSIGNOP Exp %s\n",current->left->name);
		trans_exp(current->left->right->right,current->left->name);
		//VarDec->ID
		if(current->left->left->type==2){
			//char* tmp=new_temp();
			//settempname(current->left->left->info.name,tmp);
		}
			//printf("%s := %s\n",current->left->left->info.name,tmp);
		// VarDec -> VarDec LB INT RB
		else
			trans_vardecArray(current->left->left);
		if(current->right)
			trans(current->right);
	}
	//ParamDec->Specifier VarDec
	/*else if(!strcmp(current->info.name,"ParamDec")){
		printf("into paramdec\n");
		trans_paramdec(current);
	}*/
	else{
		if(current->left!=NULL){
			trans(current->left);
			//printf("*******left is%s\n", current->left->info.name);
		}
		if(current->right!=NULL){
			trans(current->right);
			//printf("*******right is%s\n", current->right->info.name);

		}
	}
}
int allocreg(char* name){
	int i,flag=0;
	int count=0;
	for(i=0;i<regcount;i++){
		if(!strcmp(name,regname[i])){
			flag=1;
			break;
		}
	}
	if(flag){
		return i;
	}
	else{
		if(regcount==12){

			regname[firstreg]=(char*)malloc(sizeof(char));
			strcpy(regname[firstreg],name);
			count=firstreg;
			firstreg=(firstreg+1)%12;
		}
		else{
			regname[regcount]=(char*)malloc(sizeof(char));
			strcpy(regname[regcount],name);
			count=regcount;
			regcount++;
		}
		return count;
	}
}
