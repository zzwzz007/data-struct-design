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
#define SATISFIED 1
#define SHRUNK 0
#define PLUS 1
#define MINUS 0
#define UNSAT 0
#define SAT 1
#define ASSIGN_IMPLIED 1
#define ASSIGN_NONE 0
#define ASSIGN_BRANCHED -1

typedef int status;

typedef struct Literal {
	int assign;
	int occur;
	int *in_clauses;//出现在的子句列表
	int *in_locs;//对应位置
	int is_unit;//成为了子句中唯一一个未赋值的 则设置为yes
	int antecedent_clause;//单文字子句位置
} Literal;
typedef struct Clause {
	int *literals;//记录子句里的所有文字
	int current_length;//当前长度
	int original_length;//初始长度
	int is_satisfied;//该子句是否满足
	int binary_code;//记录取值
	int current_ucl;//有单子句文字
} Clause;
typedef struct Changes_info {
	int clause_index;
	int literal_index;
} Changes_info;
typedef struct assign_info {
	int type;//记录所赋的值
	int depth;//记录所处层数
	int decision;//记录赋值原因
} assign_info;
typedef struct Formula{
	Literal **lit;
	Clause *cla;
	int litnum;
	int clunum;
	int r_clunum;

	Changes_info *changes;
	int changes_index;

	assign_info *assign;

	int contradictory_unit_clauses;
	int conflicting_literal;

	int *gucl_stack;//global-unit-clause-literal
	int n_gucl;//stack size

	int depth;
	int backtrack_level;
	int max_depth;

	int *impl_clauses;//stack of error clause
	int icl_cnt;//stack size

	int **n_changes;
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
	F->lit = (Literal **)malloc(sizeof(Literal)*(*litnum+1)*2);
	for (int i = 0; i < (*litnum+1); ++i)
		F->lit[i] = (Literal *)malloc(sizeof(Literal)*2);//+-
	F->clunum = *clunum;
	F->litnum = *litnum;
	F->r_clunum = *clunum;

	F->changes = (Changes_info *)malloc(sizeof(Changes_info)*(*clunum+1)*(*litnum+1));
	F->changes_index=0;

	F->assign = (assign_info *)malloc(sizeof(assign_info)*(*litnum+1));
	
	F->contradictory_unit_clauses = 0;
	F->conflicting_literal = 0;

	F->gucl_stack = (int *)malloc(sizeof(int)*(*litnum)*2);
	F->n_gucl = 0;

	F->depth = 0;
	F->backtrack_level = 0;
	F->max_depth = 0;

	F->impl_clauses = (int *)malloc(sizeof(int)*(*clunum));
	F->icl_cnt = 0;

	F->n_changes = (int **)malloc(sizeof(int)*(*litnum)*2);
	for (int i = 0; i < (*litnum)*(*litnum); ++i)
		F->n_changes[i] = (int *)malloc(sizeof(int)*2);
	for (int i = 0; i < (*litnum); ++i)
		for(int j=0;j<2;j++)
			F->n_changes[i][j]=0;
	for(int i = 1; i <= (*litnum); i++){
		for (int j = 0; j < 2; j++)
		{
			F->lit[i][j].assign = NO;
			F->lit[i][j].occur = 0;
			F->lit[i][j].in_clauses = (int *)malloc(sizeof(int));
			F->lit[i][j].in_locs = (int *)malloc(sizeof(int));
			F->lit[i][j].is_unit = NO;
			F->lit[i][j].antecedent_clause = 0;
		}
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
				F->lit[p][q].occur++;
				F->lit[p][q].in_clauses = (int *)realloc(F->lit[p][q].in_clauses,sizeof(int)*(F->lit[p][q].occur));
				F->lit[p][q].in_clauses[F->lit[p][q].occur-1] = i;
				F->lit[p][q].in_locs = (int *)realloc(F->lit[p][q].in_locs,sizeof(int)*(F->lit[p][q].occur));
				F->lit[p][q].in_locs[F->lit[p][q].occur-1] = k;
			}
			else{
				//read 0
				n--;
				F->cla[i].current_length = n;
				F->cla[i].original_length = n;
				F->cla[i].is_satisfied = UNKNOWN;
				F->cla[i].binary_code = (((1<<(n-1))-1)<<1) + 1;
				F->cla[i].current_ucl = 0;
				break;
			}//if
		}//while
	}//for
	fclose(fp);
	printf("Reading Successfully!\nThe num of literal is %d, num of clause is %d\n",*litnum,*clunum);
	return F;
} //ReadToFormula
