#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define NEG -1
#define UNKNOWN -1
#define YES 1
#define NO 0
#define SAT 1
#define UNSAT 0
#define IMPLIED 1
#define NONE 0
#define BRANCHED -1
#define UNASSIGNED -1

typedef int status;
typedef struct Literal {
	int assign;
	int occur;
	int *in_clauses;//出现在的子句列表
	int *in_locs;//对应位置
	int is_unit;//成为了子句中唯一一个未赋值的 则设置为yes
	int unit_loc;//单文字子句位置
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
	int type;//记录所赋的值类型
	int depth;//记录所处层数
	int decision;//记录赋值原因
} assign_info;
typedef struct Formula{
	Literal **lit;
	Clause *cla;
	int litnum;
	int clanum;
	int cur_clanum;

	Changes_info *changes;
	int changes_index;

	assign_info *assign;

	int is_unit_unsat;
	int unsat_ucl;

	int *gucl_stack;//global-unit-clause-literal
	int n_gucl;//stack size

	int depth;
	int backtrack_level;

	int *study_stack;//stack of error clause
	int n_study;//stack size

	int original_lit;
	int current_lit;

	int **n_changes;
}Formula;

int Abs(int num) {
	return num > 0 ? num : -num;
}
int Symbol(int num) {
	if (num > 0) return 1;
	else return 0;
}
void Readcnf(char filename[], int last){
	filename[last] = '.';
	filename[last+1] = 'c';
	filename[last+2] = 'n';
	filename[last+3] = 'f';
	filename[last+4] = '\0';
}
void Writeres(char filename[], int last){
	filename[last] = '.';
	filename[last+1] = 'r';
	filename[last+2] = 'e';
	filename[last+3] = 's';
	filename[last+4] = '\0';
}
void Addlit(Formula *F, int lit, int claloc, int litloc){
	F->cla[claloc].literals = (int *)realloc(F->cla[claloc].literals,sizeof(int)*(litloc+1));
	F->cla[claloc].literals[litloc] = lit;
	int p=Abs(lit),q=Symbol(lit);
	F->lit[p][q].occur++;
	F->lit[p][q].in_clauses = (int *)realloc(F->lit[p][q].in_clauses,sizeof(int)*(F->lit[p][q].occur));
	F->lit[p][q].in_clauses[F->lit[p][q].occur-1] = claloc;
	F->lit[p][q].in_locs = (int *)realloc(F->lit[p][q].in_locs,sizeof(int)*(F->lit[p][q].occur));
	F->lit[p][q].in_locs[F->lit[p][q].occur-1] = litloc;
}
void Finish_addlit(Formula *F, int i, int k){
	F->cla[i].current_length = k;
	F->cla[i].original_length = k;
	F->cla[i].is_satisfied = NO;
	F->cla[i].binary_code = (((1<<(k-1))-1)<<1) + 1;
	F->cla[i].current_ucl = 0;
}
Formula *InitFormula(int litnum, int clanum){
	Formula *F;
	F = (Formula *)malloc(sizeof(Formula));
	F->cla = (Clause *)malloc(sizeof(Clause)*(clanum+1));
	F->lit = (Literal **)malloc(sizeof(Literal)*(litnum+1)*2);
	for (int i = 0; i <= litnum; i++)
		F->lit[i] = (Literal *)malloc(sizeof(Literal)*2);//+-
	F->clanum = clanum;
	F->litnum = litnum;
	F->cur_clanum = clanum;

	F->changes = (Changes_info *)malloc(sizeof(Changes_info)*clanum*litnum);
	F->changes_index=0;

	F->assign = (assign_info *)malloc(sizeof(assign_info)*litnum);
	
	F->is_unit_unsat = 0;
	F->unsat_ucl = 0;

	F->gucl_stack = (int *)malloc(sizeof(int)*litnum);
	F->n_gucl = 0;

	F->depth = 0;
	F->backtrack_level = 0;

	F->study_stack = (int *)malloc(sizeof(int)*clanum);
	F->n_study = 0;

	F->original_lit = 0;
	F->current_lit=1;

	F->n_changes = (int **)malloc(sizeof(int)*clanum*litnum);
	for (int i = 0; i < litnum*litnum; i++)
		F->n_changes[i] = (int *)malloc(sizeof(int)*2);
	for (int i = 0; i < litnum; i++)
		for(int j=0;j<2;j++)
			F->n_changes[i][j]=0;
	for(int i = 1; i <= litnum; i++){
		F->assign[i].decision = NONE;
		F->assign[i].type = UNASSIGNED;
		F->assign[i].depth = -1;
		for (int j = 0; j < 2; j++)
		{
			F->lit[i][j].assign = NO;
			F->lit[i][j].occur = 0;
			F->lit[i][j].in_clauses = (int *)malloc(sizeof(int));
			F->lit[i][j].in_locs = (int *)malloc(sizeof(int));
			F->lit[i][j].is_unit = NO;
			F->lit[i][j].unit_loc = 0;
		}
	}
	return F;
}
Formula *ReadToFormula(char *filename) {
	FILE *fp;
	int litnum,clanum;
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("File open error\n ");
		exit(-1);
	}
	char rec;
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
			for (int i = 0; i < 5; i++) fgetc(fp);//read cnf
			fscanf(fp,"%d %d\n",&litnum,&clanum);
			break;
		}
		else {
			//error
			printf("formal error !\n");
			exit(-1);
		} //if rec
	}//while
	Formula *F = InitFormula(litnum, clanum);
	//read clause
	int lit = 0;
	int k;//k:record the loc of current lit
	for(int i = 1;i <= clanum;i++){
		k = 0;
		F->cla[i].literals = (int *)malloc(sizeof(int));
		while(1){
			fscanf(fp,"%d", &lit);
			if(lit){
				//read no 0
				k++;
				Addlit(F, lit, i, k);
			}
			else{
				//read 0
				Finish_addlit(F, i, k);
				break;
			}//if
		}//while
	}//for
	fclose(fp);
	printf("Reading Successfully!\nThe num of literal is %d, num of clause is %d\n",litnum,clanum);
	return F;
} //ReadToFormula
