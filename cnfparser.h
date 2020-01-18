#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define NEG -1
#define UNKNOWN -1
#define YES 1
#define NO 0
#define ERROR -1
#define FINISHED 1

typedef int status;

typedef struct Literal {
	int assign;
	int pos_occur;
	int neg_occur;
	int *pos_in_clauses;//出现在的子句列表
	int *pos_in_locs;//对应位置
	int *neg_in_clauses;//出现在的子句列表
	int *neg_in_locs;//对应位置
	// int is_unit;//成为了子句中唯一一个未赋值的 则设置为yes
	// int antecedent_clause;//单文字子句位置
} Literal;
typedef struct Clause {
	int *literals;//记录子句里的所有文字
	int current_length;//当前长度
	int original_length;//初始长度
	int is_satisfied;//该子句是否满足
	int binary_code;//记录取值
	// int current_ucl;//有单子句文字
} Clause;
typedef struct Changes_info {
	int clause_index;
	int literal_index;
} Changes_info;
typedef struct Formula{
	Literal *lit;
	Clause *cla;
	Changes_info *changes;
	int changes_index;
	int litnum;
	int clunum;
}Formula;
int Abs(int num) {
	return num > 0 ? num : -num;
}
int Symbol(int num) {
	if (num > 0) return 1;
	else return 0;
}
Formula *ReadToFormula(char *filename,int *litnum,int *clunum) {
	FILE *fp;
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("File open error\n ");
		exit(-1);
	}
	char rec;
	int over=0,startread=0;
	while(1) {
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
			break;
		}
		else {
			//error
			printf("formal error !\n");
			exit(-1);
		} //if rec
	}//while
	Formula *F;
	F = (Formula *)malloc(sizeof(Formula));
	F->cla = (Clause *)malloc(sizeof(Clause)*(*clunum+1));
	F->lit = (Literal *)malloc(sizeof(Literal)*(*litnum+1));
	F->changes = (Changes_info *)malloc(sizeof(Changes_info)*(*clunum));
	F->changes_index=0;
	F->clunum = *clunum;
	F->litnum = *litnum;
	for(int i = 1; i <= (*litnum); i++){
		F->lit[i].assign = UNKNOWN;
		F->lit[i].pos_occur = 0;
		F->lit[i].neg_occur = 0;
		F->lit[i].pos_in_clauses = (int *)malloc(sizeof(int));
		F->lit[i].neg_in_clauses = (int *)malloc(sizeof(int));
		F->lit[i].neg_in_locs = (int *)malloc(sizeof(int));
		F->lit[i].pos_in_locs = (int *)malloc(sizeof(int));

	}
	//read clause
	int lit,flag = 0;
	int n,k;//n:record the num of lit  k:record the loc of current lit
	for(int i = 1;i <= *clunum;i++){
		n = 1;
		k = 0;
		F->cla[i].literals = (int *)malloc(sizeof(int));
		while(1){
			fscanf(fp,"%d", &lit);
			if(lit){
				//read no 0
				k++;
				F->cla[i].literals = (int *)realloc(F->cla[i].literals,sizeof(int)*(++n));
				F->cla[i].literals[n-1] = lit;
				int p=Abs(lit),q=Symbol(lit);
				if(q){
					//lit>0
					F->lit[p].pos_occur++;
					F->lit[p].pos_in_clauses = (int *)realloc(F->lit[p].pos_in_clauses,sizeof(int)*(F->lit[p].pos_occur));
					F->lit[p].pos_in_clauses[F->lit[p].pos_occur-1] = i;
					F->lit[p].pos_in_locs = (int *)realloc(F->lit[p].pos_in_locs,sizeof(int)*(F->lit[p].pos_occur));
					F->lit[p].pos_in_locs[F->lit[p].pos_occur-1] = k;
				}
				else{
					//lit<0
					F->lit[p].neg_occur++;
					F->lit[p].neg_in_clauses = (int *)realloc(F->lit[p].neg_in_clauses,sizeof(int)*(F->lit[p].neg_occur));
					F->lit[p].neg_in_clauses[F->lit[p].neg_occur-1] = i;
					F->lit[p].neg_in_locs = (int *)realloc(F->lit[p].neg_in_locs,sizeof(int)*(F->lit[p].neg_occur));
					F->lit[p].neg_in_locs[F->lit[p].neg_occur-1] = k;
				}
			}
			else{
				//read 0
				n--;
				F->cla[i].current_length = n;
				F->cla[i].original_length = n;
				F->cla[i].is_satisfied = UNKNOWN;
				F->cla[i].binary_code = (((1<<(n-1))-1)<<1) + 1;
				break;
			}//if
		}//while
	}//for
	fclose(fp);
	printf("Reading Successfully!\nThe num of literal is %d, num of clause is %d\n",*litnum,*clunum);
	return F;
} //ReadToFormula
