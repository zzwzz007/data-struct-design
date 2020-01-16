#include "display.h"

typedef struct Literal {
	//文字
	int data;
	struct Literal *nextliteral;//下一个文字
} Literal;

typedef struct Clause {
	//子句
	struct Clause *nextclause;//下一个子句
	Literal *firstliteral;
} Clause;

Clause *ReadToFormula(char *filename,int *litnum,int *clunum) {
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

	while(1) {
		if(over) {
			break;
		}
		if(!startread) {
			//former info
			fscanf(fp,"%c",&rec);
			if(rec=='c') {
				while(1) {
					rec=fgetc(fp);
					if(rec=='\n') {
						break;
					}
				}
			} //if
			else if(rec=='p') {
				for (int i = 0; i < 5; ++i) {
					fgetc(fp);
				}
				fscanf(fp,"%d %d\n",litnum,clunum);
				startread=1;
			} //else if
			else {
				printf("input error!\n");
				exit(-1);
			} //end if rec
		}//if startread
		else {
			//read clause
			int lit,flag=0;//lit:read literal;flag:if first lit

			now_c->firstliteral=(Literal *)malloc(sizeof(Literal));
			newlit=now_c->firstliteral;
			int status=1;

			while(1) {

				status=fscanf(fp,"%d",&lit);
				if(status!=1) {
					over=1;
					break;
				}
				if(!flag) {
					//first literal
					newlit->data=lit;
					newlit->nextliteral=NULL;
					flag=1;
					continue;
				} //if
				if(lit) {
					//no first literal and no 0
					newlit->nextliteral=(Literal *)malloc(sizeof(Literal));
					newlit->nextliteral->data=lit;
					newlit->nextliteral->nextliteral=NULL;
					newlit=newlit->nextliteral;
				} else {
					//read 0
					now_c->nextclause=(Clause *)malloc(sizeof(Clause));
					pre_c=now_c;
					now_c=now_c->nextclause;
					now_c->nextclause=NULL;
					break;
				}
			} //while
		}//end if startread
	}//while
	pre_c->nextclause=NULL;
	fclose(fp);
	printf("Read Over! Num of literal is %d, num of clause is %d",*litnum,*clunum);
	return formula;
} //ReadToFormula

