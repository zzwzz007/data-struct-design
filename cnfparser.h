#include "display.h"

typedef struct Literal {
	int data;
	struct Literal *nextliteral;//下一个文字
} Literal;
typedef struct Clause {
	struct Clause *nextclause;//下一个子句
	Literal *firstliteral;
} Clause;
Clause *ReadToFormula(char *filename,int *litnum,int *clunum) {
	FILE *fp;
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("File open error\n ");
		exit(-1);
	}
	Clause *formula,*pre_c,*now_c;
	formula=(Clause *)malloc(sizeof(Clause));
	pre_c=NULL;
	now_c=formula;
	Literal *newlit=NULL;
	char rec;
	int over=0,startread=0;
	while(!feof(fp)) {
		if(!startread) {
			//read former
			fscanf(fp,"%c",&rec);
			if(rec=='c') {
				//read info
				while(1) {
					rec=fgetc(fp);
					if(rec=='\n') {
						break;
					}//if
				}//while
			}
			else if(rec=='p') {
				//read the num
				for (int i = 0; i < 5; ++i) fgetc(fp);//read cnf
				fscanf(fp,"%d %d\n",litnum,clunum);
				startread=1;
			}
			else {
				//error
				printf("formal error !\n");
				exit(-1);
			} //if rec
		}//if startread
		else {
			//read clause
			int lit,flag=0;
			now_c->firstliteral=(Literal *)malloc(sizeof(Literal));
			newlit=now_c->firstliteral;
			int status=1;
			while(!feof(fp)) {
				fscanf(fp,"%d",&lit);
				if(!flag) {
					//first literal
					newlit->data=lit;
					newlit->nextliteral=NULL;
					flag=1;
					continue;
				} //if
				if(lit) {
					//no first literal and no 0, add lit
					newlit->nextliteral=(Literal *)malloc(sizeof(Literal));
					newlit->nextliteral->data=lit;
					newlit->nextliteral->nextliteral=NULL;
					newlit=newlit->nextliteral;
				} else {
					//read 0 ,prepare cla
					now_c->nextclause=(Clause *)malloc(sizeof(Clause));
					pre_c=now_c;
					now_c=now_c->nextclause;
					now_c->nextclause=NULL;
					break;
				}
			} //while feof
		}//end if else startread
	}//while feof
	pre_c->nextclause=NULL;
	fclose(fp);
	printf("Reading Successfully!\nThe num of literal is %d, num of clause is %d\n",*litnum,*clunum);
	return formula;
} //ReadToFormula