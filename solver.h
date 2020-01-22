#include "display.h"
#define TRUE 1
#define FALSE 0

int logto2(int num) {
	int res=1;
	while(num>1) {
		num/=2;
		res++;
	}
	return res;
}
int GetNextlit(Formula *F){
	int max=0,rec,more;
	for(int i=1;i<=F->litnum;i++){
		if(F->assign[i].decision == ASSIGN_NONE){
			rec=F->lit[i][0].occur + F->lit[i][1].occur;
			if(rec>max){
				max=rec;
				if(F->lit[i][0].occur > F->lit[i][1].occur)//more -
					more=-i;
				else more=i;
			}
		}
	}
	return more;
}

void SetVar(Formula *F,int v) {
	int i;
	int p = Abs(v), q = Symbol(v);
	for(i = 0; i < F->lit[p][q].occur; i++) {
		//遍历所有含v的子句
		int j = F->lit[p][q].in_clauses[i];//遍历
		if(F->cla[j].is_satisfied == YES) continue;
		F->cla[j].is_satisfied = YES;
		--F->r_clunum;
		F->changes[F->changes_index].clause_index = j;
		F->changes_index++;
		F->n_changes[F->depth][SATISFIED]++;
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
		F->n_changes[F->depth][SHRUNK]++;
		if(F->cla[j].current_length == 1) {
			int loc = logto2(F->cla[j].binary_code);
			int w = F->cla[j].literals[loc];
			int s = Abs(w), t = Symbol(w);
			F->lit[s][t].antecedent_clause = j;
			if(F->lit[s][(!t)].is_unit == YES) {
				F->contradictory_unit_clauses = TRUE;
				F->conflicting_literal = w;
			} else if(F->lit[s][t].is_unit == NO) {
				F->gucl_stack[F->n_gucl] = w;
				F->cla[j].current_ucl = w;
				F->lit[s][t].is_unit = YES;
				++F->n_gucl;
			}
		}
	}
	if(F->depth && F->backtrack_level == F->depth-1)
		++F->backtrack_level;
	++F->depth;
	F->lit[p][q].assign = YES;
	F->lit[p][!q].assign = YES;
}

void UnSetVar(Formula *F, int v) {
	int i;
	int p = abs(v), q = Symbol(v);
	--F->depth;
	if(F->depth && F->backtrack_level == F->depth)
		--F->backtrack_level;
	while(F->n_changes[F->depth][SHRUNK]) {
		--F->n_changes[F->depth][SHRUNK];
		int j = F->changes[--F->changes_index].clause_index;
		int k = F->changes[F->changes_index].literal_index;
		++F->cla[j].current_length;
		if(F->cla[j].current_length == 2) {
			int s = abs(F->cla[j].current_ucl);
			int t = Symbol(F->cla[j].current_ucl > 0);
			F->lit[s][t].is_unit = NO;
			F->cla[j].current_ucl = 0;
		}
		F->cla[j].binary_code += ((1 << (k-1)));
	}
	while(F->n_changes[F->depth][SATISFIED]) {
		--F->n_changes[F->depth][SATISFIED];
		int j = F->changes[--F->changes_index].clause_index;
		F->cla[j].is_satisfied = NO;
		++F->r_clunum;
	}
	F->lit[p][SATISFIED].assign = NO;
	F->lit[p][SHRUNK].assign = NO;
}

int dpll(Formula *F) {
	int * lucl_stack ;
	lucl_stack = (int *)malloc(sizeof(int));
	unsigned int n_lucl = 0;
	while(1) {
		if(F->contradictory_unit_clauses) {
			F->n_study = 0;
			int cl = Abs(F->conflicting_literal);
			F->study_stack[F->n_study++] = F->lit[cl][SATISFIED].antecedent_clause;
			F->study_stack[F->n_study++] = F->lit[cl][SHRUNK].antecedent_clause;
			F->assign[cl].decision = ASSIGN_NONE;
			F->assign[cl].type = UNASSIGNED;
			while(n_lucl) {
				UnSetVar(F,lucl_stack[--n_lucl]);
				int s = Abs(lucl_stack[n_lucl]);
				int t = lucl_stack[n_lucl]>0 ? TRUE : FALSE;
				F->study_stack[F->n_study++] = F->lit[s][t].antecedent_clause;
				F->assign[s].type = UNASSIGNED;
				F->assign[s].decision = ASSIGN_NONE;
			}
			F->contradictory_unit_clauses = FALSE;
			free(lucl_stack);
			F->n_gucl = 0;
			return UNSAT;
		} else if (F->n_gucl) {
			lucl_stack = (int*)realloc(lucl_stack,(n_lucl+1)*sizeof(int));
			int implied_lit = F->gucl_stack[--F->n_gucl];
			lucl_stack[n_lucl++] = implied_lit;
			F->assign[Abs(implied_lit)].type = implied_lit>0 ? TRUE : FALSE;
			F->assign[Abs(implied_lit)].decision = ASSIGN_IMPLIED;
			SetVar(F,implied_lit);
		} 
		else break;
	}
	//branch rule
	if(!F->r_clunum) return SAT;
	int v = GetNextlit(F);
	F->assign[Abs(v)].type = v > 0 ? TRUE : FALSE;
	F->assign[Abs(v)].depth = F->depth;
	F->assign[Abs(v)].decision = ASSIGN_BRANCHED;
	SetVar(F,v);
	if(dpll(F)) return SAT;
	//error
	UnSetVar(F,v);
	// F->assign[Abs(v)].type = UNASSIGNED;
	
	int max_depth = 0, i, j, k, m, left = FALSE;
	if(F->n_study) {
		while(F->n_study) {
			F->n_study--;
			i = F->study_stack[F->n_study];
			k = F->cla[i].original_length;
			for(j=1; j <= k; ++j) {
				m = Abs(F->cla[i].literals[j]);
				if((F->assign[m].decision == ASSIGN_BRANCHED) && (F->assign[m].depth > max_depth))
					max_depth = F->assign[m].depth;
			}
		}
		left = TRUE;
		}
	F->assign[Abs(v)].decision = ASSIGN_NONE;
	// ++n_backtracks;
	if(F->backtrack_level >= F->depth-1) {
		F->assign[Abs(v)].type = !F->assign[Abs(v)].type;
		F->assign[Abs(v)].decision = ASSIGN_IMPLIED;
		SetVar(F,-v);
		if(dpll(F)) return SAT;
		UnSetVar(F,-v);
		F->assign[Abs(v)].type = UNASSIGNED;
		F->assign[Abs(v)].decision = ASSIGN_NONE;
		if(left && F->n_study) {
			while(F->n_study) {
				F->n_study--;
				i = F->study_stack[F->n_study];
				k = F->cla[i].original_length;
				for(j=1; j <= k; ++j) {
					m = Abs(F->cla[i].literals[j]);
					if(F->assign[m].decision == ASSIGN_BRANCHED &&
					        F->assign[m].depth > max_depth)
						max_depth = F->assign[m].depth;
				}
			}
			if(max_depth < F->backtrack_level)
				F->backtrack_level = max_depth;

		}
	}
	F->n_study = 0;
	while(n_lucl) {
		n_lucl--;
		int z = lucl_stack[n_lucl];
		UnSetVar(F,z);
		F->assign[Abs(z)].type = UNASSIGNED;
		F->assign[Abs(z)].decision = ASSIGN_NONE;
	}
	free(lucl_stack);
	F->contradictory_unit_clauses = FALSE;
	return UNSAT;
}

