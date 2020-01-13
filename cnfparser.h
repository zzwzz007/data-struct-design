#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct Record
{//记录
	int num;//数字
	struct Record *nextliteral;//下一个
}Record;

typedef struct Literal
{
	int truth;//真值
	int unassigned;//未定义
	int unsatisfied;//不满足
	int satisfied;//满足
	Record *firstcla;//第一个子句
}Literal;

typedef struct Clause
{//子句
	int unassigned;//未定义
	int unsatisfied;//不满足
	int satisfied;//满足
	struct Clause *nextclause;//下一个子句
	Record *firstlit;//第一个文字
	Record *lastlit;//最后一个文字
}Clause;

typedef struct Formula
{//公式
	Clause *firstclause;
}Formula;

void ReadToClause(Clause *c,int x){
	Record *newadd;
	newadd=(Record *)malloc(sizeof(Record));//新建记录
	newadd->num=x;
	newadd->nextliteral=NULL;
	if(c->firstlit==NULL){
		//无第一文字
		c->firstlit=newadd;
		c->lastlit=newadd;
		c->unassigned++;
	}
	else{
		//有第一文字
		c->lastlit->nextliteral=newadd;
		c->lastlit=newadd;
		c->unassigned++;
	}
}

void ReadToFormula(Formula *Fnew,int num){
	//Formula *Fnew;
	//Fnew=(Formula *)malloc(sizeof(Formula));
	Clause *cla,*last,*first;
	Fnew->firstclause=NULL;
	last=(Clause *)malloc(sizeof(Clause));
	last=Fnew->firstclause;
	int i;//循环
	int x;//读入
	for (i = 0; i < num; i++)
	{
		//初始化子句
		cla=(Clause *)malloc(sizeof(Clause));
		cla->unassigned=0;
		cla->unsatisfied=0;
		cla->satisfied=0;
		cla->firstlit=NULL;
		cla->lastlit=NULL;
		cla->nextclause=NULL;
		
		//创造子句
		scanf("%d",&x);
		while(x!=0){
			ReadToClause(cla,x);
			scanf("%d",&x);
		}

		//子句插在序列尾
		if(Fnew->firstclause==NULL){
			//无第一点
			Fnew->firstclause=cla;
			last=cla;
		}
		else{
			//有第一点
			last->nextclause=cla;
			last=last->nextclause;
		}
		
	}
}


