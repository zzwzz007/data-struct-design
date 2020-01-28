#include "display.h"

int logto2(int num) {
	int res = 1;
	while(num > 1) {
		num /= 2;
		res++;
	}
	return res;
}
int GetNextlit(Formula *F){
	int max = 0,rec,more;
	for(int i = 1;i <= F->litnum;i++){
		if(F->assign[i].decision == NONE){
			rec = F->lit[i][0].occur + F->lit[i][1].occur;
			if(rec>max){
				max = rec;
				if(F->lit[i][0].occur > F->lit[i][1].occur)//more -
					more = -i;
				else more = i;
			}
		}
	}
	return more;
}
void UpdateTrackback(Formula *F, int *max_depth){
	int i,j,m;
	while(F->n_study) {
		F->n_study--;
		i = F->study_stack[F->n_study];
		j = F->cla[i].original_length;
		for(int k=1; k <= j; ++k) {
			m = Abs(F->cla[i].literals[k]);
			if(F->assign[m].decision == BRANCHED &&
			        F->assign[m].depth > *max_depth)
				*max_depth = F->assign[m].depth;
		}
	}
}
void SetVar(Formula *F,int v) {
	int i;
	int p = Abs(v), q = Symbol(v);
	for(i = 0; i < F->lit[p][q].occur; i++) {
		//遍历所有含v的子句
		int j = F->lit[p][q].in_clauses[i];//遍历
		if(F->cla[j].is_satisfied == YES) continue;
		F->cla[j].is_satisfied = YES;
		F->cur_clanum--;
		F->changes[F->changes_index].clause_index = j;
		F->changes_index++;
		F->n_changes[F->depth][SAT]++;
	}
	q = !q;
	for(i = 0; i < F->lit[p][q].occur; i++) {
		int j = F->lit[p][q].in_clauses[i];
		if(F->cla[j].is_satisfied == YES ) continue;
		int k = F->lit[p][q].in_locs[i];
		F->cla[j].current_length--;
		F->cla[j].binary_code -= ((1 << (k-1)));
		F->changes[F->changes_index].clause_index = j;
		F->changes[F->changes_index++].literal_index = k;
		F->n_changes[F->depth][UNSAT]++;
		if(F->cla[j].current_length == 1) {
			int loc = logto2(F->cla[j].binary_code);
			int w = F->cla[j].literals[loc];
			int s = Abs(w), t = Symbol(w);
			F->lit[s][t].unit_loc = j;
			if(F->lit[s][(!t)].is_unit == YES) {
				F->is_unit_unsat = YES;
				F->unsat_ucl = w;
			} else if(F->lit[s][t].is_unit == NO) {
				F->gucl_stack[F->n_gucl] = w;
				F->cla[j].current_ucl = w;
				F->lit[s][t].is_unit = YES;
				F->n_gucl++;
			}
		}
	}
	if(F->depth && F->backtrack_level == F->depth-1)
		F->backtrack_level++;
	F->depth++;
	F->lit[p][q].assign = YES;
	F->lit[p][!q].assign = YES;
}
void UnSetVar(Formula *F, int v) {
	int p = abs(v);
	F->depth--;
	if(F->depth && F->backtrack_level == F->depth)
		F->backtrack_level--;
	while(F->n_changes[F->depth][UNSAT]) {
		F->n_changes[F->depth][UNSAT]--;
		F->changes_index--;
		int j = F->changes[F->changes_index].clause_index;
		int k = F->changes[F->changes_index].literal_index;
		F->cla[j].current_length++;
		if(F->cla[j].current_length == 2) {
			int s = abs(F->cla[j].current_ucl);
			int t = Symbol(F->cla[j].current_ucl);
			F->lit[s][t].is_unit = NO;
			F->cla[j].current_ucl = 0;
		}
		F->cla[j].binary_code += ((1 << (k-1)));
	}
	while(F->n_changes[F->depth][SAT]) {
		F->n_changes[F->depth][SAT]--;
		F->changes_index--;
		F->cur_clanum++;
		int j = F->changes[F->changes_index].clause_index;
		F->cla[j].is_satisfied = NO;
	}
	F->lit[p][SAT].assign = NO;
	F->lit[p][UNSAT].assign = NO;
}
int dpll(Formula *F) {
	int *lucl_stack ;
	lucl_stack = (int *)malloc(sizeof(int));
	int n_lucl = 0;
	while(1) {
		if(F->is_unit_unsat) {
			F->n_study = 0;
			int cl = Abs(F->unsat_ucl);
			F->study_stack[F->n_study++] = F->lit[cl][SAT].unit_loc;
			F->study_stack[F->n_study++] = F->lit[cl][UNSAT].unit_loc;
			F->assign[cl].decision = NONE;
			F->assign[cl].type = UNASSIGNED;
			while(n_lucl) {
				UnSetVar(F,lucl_stack[--n_lucl]);
				int s = Abs(lucl_stack[n_lucl]);
				int t = Symbol(lucl_stack[n_lucl]);
				F->study_stack[F->n_study++] = F->lit[s][t].unit_loc;
				F->assign[s].type = UNASSIGNED;
				F->assign[s].decision = NONE;
			}
			F->is_unit_unsat = NO;
			free(lucl_stack);
			F->n_gucl = 0;
			return UNSAT;
		} else if (F->n_gucl) {
			lucl_stack = (int*)realloc(lucl_stack,(n_lucl+1)*sizeof(int));
			int nextlit = F->gucl_stack[--F->n_gucl];
			lucl_stack[n_lucl++] = nextlit;
			F->assign[Abs(nextlit)].type = Symbol(nextlit);
			F->assign[Abs(nextlit)].decision = IMPLIED;
			SetVar(F,nextlit);
		} 
		else break;
	}
	//branch rule
	if(!F->cur_clanum) return SAT;
	int v ;
	if(F->original_lit==0)
		v= GetNextlit(F);
	else{
		v=F->cla[F->current_lit].literals[1];
		F->current_lit++;
		F->original_lit--;
	}
	if(F->assign[Abs(v)].type!=UNASSIGNED){
		for(int i=1;i<F->clanum;i++){
			if(F->cla[i].is_satisfied==NO)
				printf("%d  ",i);
		}
		printf("!!!\n%d\n",F->cur_clanum);
		for(int j=1;j<=36;j++){
			printf("%2d  ", F->assign[j].type);
			if(j%6==0) printf("\n");
		}
		getchar();
	}
	F->assign[Abs(v)].type = v > 0 ? YES : NO;
	F->assign[Abs(v)].depth = F->depth;
	F->assign[Abs(v)].decision = BRANCHED;
	SetVar(F,v);
	if(dpll(F)) return SAT;
	//error
	UnSetVar(F,v);
	// F->assign[Abs(v)].type = UNASSIGNED;
	int max_depth = 0,  left = NO;
	if(F->n_study) {
		UpdateTrackback(F, &max_depth);
		left = YES;
	}
	F->assign[Abs(v)].decision = NONE;
	// ++n_backtracks;
	if(F->backtrack_level >= F->depth-1) {
		F->assign[Abs(v)].type = !F->assign[Abs(v)].type;
		F->assign[Abs(v)].decision = IMPLIED;
		SetVar(F,-v);
		if(dpll(F)) return SAT;
		UnSetVar(F,-v);
		F->assign[Abs(v)].type = UNASSIGNED;
		F->assign[Abs(v)].decision = NONE;
		if(left && F->n_study) {
			UpdateTrackback(F, &max_depth);
			if(max_depth < F->backtrack_level)
				F->backtrack_level = max_depth;
		}
	}
	F->n_study = 0;
	while(n_lucl) {
		n_lucl--;
		int v = lucl_stack[n_lucl];
		UnSetVar(F,v);
		F->assign[Abs(v)].type = UNASSIGNED;
		F->assign[Abs(v)].decision = NONE;
	}
	free(lucl_stack);
	F->is_unit_unsat = NO;
	return UNSAT;
}

