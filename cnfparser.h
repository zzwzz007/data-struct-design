#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

// typedef struct Record
// {//记录
// 	int num;//数字
// 	struct Record *nextliteral;//下一个
// }Record;

typedef struct Literal
{
	//文字
	// int truth;//真值
	// int unassigned;//未定义
	// int unsatisfied;//不满足
	// int satisfied;//满足
	int data;
	struct Literal *nextliteral;//下一个文字
}Literal;

typedef struct Clause
{//子句
	// int unassigned;//未定义
	// int unsatisfied;//不满足
	// int satisfied;//满足
	struct Clause *nextclause;//下一个子句
	Literal *firstliteral;
	// Record *firstlit;//第一个文字
	// Record *lastlit;//最后一个文字
}Clause;

// typedef struct Formula
// {//公式
// 	Clause *firstclause;
// }Formula;

Clause *ReadToFormula(char *filename,int *litnum,int *clunum){
	//Formula *Fnew;
	//Fnew=(Formula *)malloc(sizeof(Formula));
	FILE *fp;
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("File open error\n ");
		exit(-1);
	}
	
	Clause *formula,*pre_c,*now_c;//all;pre;now
	formula=(Clause *)malloc(sizeof(Clause));
	pre_c=NULL;
	now_c=formula;
	Literal *newlit=NULL;
	
	char rec;
	int over=0,startread=0;
	
	while(1){
		if(over) break;
		if(!startread){
			//former info
			fscanf(fp,"%c",&rec);
			if(rec=='c'){
				while(1){
					rec=fgetc(fp);
					if(rec=='\n') break;
				}
			}//if
			else if(rec=='p'){
				for (int i = 0; i < 5; ++i)
					fgetc(fp);
				fscanf(fp,"%d %d\n",litnum,clunum);
				startread=1;
			}//else if
			else{
				printf("input error!\n");
				exit(-1);
			}//end if rec
		}//if startread
		else{
			//read clause
			int lit,flag=0;//lit:read literal;flag:if first lit
			
			now_c->firstliteral=(Literal *)malloc(sizeof(Literal));
			newlit=now_c->firstliteral;
			int status=1;
			
			while(1){
				
				status=fscanf(fp,"%d",&lit);
				if(status!=1){
					over=1;
					break;
				}
				if(!flag){
					//first literal
					newlit->data=lit;
					newlit->nextliteral=NULL;
					flag=1;
					continue;
				}//if
				if(lit){
					//no first literal and no 0
					newlit->nextliteral=(Literal *)malloc(sizeof(Literal));
					newlit->nextliteral->data=lit;
					newlit->nextliteral->nextliteral=NULL;
					newlit=newlit->nextliteral;
				}
				else{
					//read 0
					now_c->nextclause=(Clause *)malloc(sizeof(Clause));
					pre_c=now_c;
					now_c=now_c->nextclause;
					now_c->nextclause=NULL;
					break;
				}
			}//while
		}//end if startread
	}//while
	pre_c->nextclause=NULL;
	fclose(fp);
	printf("Read Over! Num of literal is %d, num of clause is %d",*litnum,*clunum);
	return formula;
}//ReadToFormula

int abs(int num) {
    return num > 0 ? num : -num;
}

int sign(int num) {
    return num > 0 ? 1 : -1;
}

Clause *copy_sub_L(Clause* L) {
    /*
    * get a copy of sub_Clause L.
    * L->N->N->N->...->N->NULL
    * L can be empty.
    */
    Clause * L_copy = (Clause *)malloc(sizeof(Clause));
    L_copy->nextclause = NULL;
    if (!L->firstliteral) {
        L_copy->firstliteral = NULL;
        return L_copy;
    }
    L_copy->firstliteral = (Literal *)malloc(sizeof(Literal));
    Literal * np = L->firstliteral, * np_new = L_copy->firstliteral;
    while(np) {
        np_new->data = np->data;
        if (!np->nextliteral) {
            np_new->nextliteral = NULL;
            break;
        }
        np_new->nextliteral = (Literal *)malloc(sizeof(Literal));
        np_new = np_new->nextliteral;
        np = np->nextliteral;
    }
    return L_copy;
}

Clause* copy_L(Clause * L) {
    // echo(L);
    Clause * L_copy = copy_sub_L(L);
    Clause *lp = L->nextclause, *lp_new = L_copy;
    while (lp) {
        lp_new->nextclause = copy_sub_L(lp);
        if (!lp->nextclause)
            break;
        lp_new = lp_new->nextclause;
        lp = lp->nextclause;
    }
    return L_copy;
}