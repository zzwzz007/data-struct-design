#include "cnfparser.h"

#define SATISFIED 1
#define SHRUNK 0

int Abs(int num) {
	return num > 0 ? num : -num;
}
int log2(int num) {
	int res;
	while(num>1) {
		num/=2;
		res++;
	}
	return res;
}
status IsEmptyClause(Clause *C) {
	if(C->firstliteral==NULL)
		return YES;
	else
		return NO;
}//IsEmptyClause
status IsUnitClause(Clause *C) {
	if(C->firstliteral && !C->firstliteral->nextliteral)
		return YES;
	else
		return NO;
}//IsUnitClause
Clause *CopyClause(Clause *C) {
	Clause *ccopy = (Clause *)malloc(sizeof(Clause));
	ccopy->nextclause = NULL;
	ccopy->is_satisfied = C->is_satisfied;
	if (IsEmptyClause(C)) {
		//empty clause
		ccopy->firstliteral = NULL;
		return ccopy;
	}//if
	ccopy->firstliteral = (Literal *)malloc(sizeof(Literal));
	Literal *lit = C->firstliteral, *litnew = ccopy->firstliteral;
	while(lit) {
		litnew->data = lit->data;
		if (!lit->nextliteral) {
			//end current clause
			litnew->nextliteral = NULL;
			break;
		}//if
		litnew->nextliteral = (Literal *)malloc(sizeof(Literal));
		litnew = litnew->nextliteral;
		lit = lit->nextliteral;
	}//while
	return ccopy;
}//CopyClause
Clause *CopyFormula(Clause *F) {
	Clause *Fcopy = CopyClause(F);
	Clause *cla = F->nextclause, *clanew = Fcopy;
	while (cla) {
		clanew->nextclause = CopyClause(cla);
		if (!cla->nextclause) break;//end formula
		clanew = clanew->nextclause;
		cla = cla->nextclause;
	}//while
	return Fcopy;
}//CopyFormula
status Removeliteral(Clause **C,Literal **prelit,Literal **lit) {
	if((*lit)==(*C)->firstliteral) {
		//first lit
		(*C)->firstliteral = (*C)->firstliteral->nextliteral;
		free((*lit));
		if (IsEmptyClause((*C))) {
			//fail
			return ERROR;
		}//if empty
		(*lit) = (*C)->firstliteral;
	} else {
		//latter lit
		(*prelit)->nextliteral = (*lit)->nextliteral;
		free((*lit));
		(*lit) = (*prelit)->nextliteral;
	}//if lit
	return YES;
}//Removeliteral
status DestroyClause(Clause **F,Clause **precla,Clause **cla) {
	Literal *firstlit = (*cla)->firstliteral;
	//moveclause
	if((*cla)==*F) {
		//first clause
		*F = (*F)->nextclause;
		free((*cla));
		(*cla) = (*F);
		if (*F == NULL) return FINISHED;// finish dpll.
	} else {
		//latter lit
		(*precla)->nextclause = (*cla)->nextclause;
		free((*cla));
		(*cla) = (*precla)->nextclause;
	} //if
	Literal * lit = NULL;
	//move literal
	while(firstlit) {
		lit = firstlit;
		firstlit = firstlit->nextliteral;
		free(lit);
	}//while
	return NO;//not finished
}//DestroyClause
status SortFormula(Clause **F, int num) {
	Clause *cla = *F, *precla = *F;
	Literal *lit, *prelit;
	int flag;
	while (cla) {
		if (IsEmptyClause(cla))
			return ERROR;
		if(cla->is_satisfied == YES) {
			cla = cla->nextclause;
			continue;
		}
		flag = 0;//if del cla
		lit = cla->firstliteral;
		prelit = cla->firstliteral;
		while(lit) {
			if (lit->data == -num) {
				//del lit
				if (Removeliteral(&cla,&prelit,&lit)==ERROR)
					return ERROR;
				continue;
			} else if (lit->data == num) {
				//cla ok, del cla
				cla->is_satisfied = YES;
				if(DestroyClause(F,&precla,&cla)==FINISHED)
					return FINISHED;
				flag = 1;//del
				break;
			}
			prelit = lit;
			lit = lit->nextliteral;
		}
		if (!flag) {
			//del cla
			precla = cla;
			cla = cla->nextclause;
		}
	}
	return 0;
}//SortFormula
void DestroyFormula(Clause * F) {
	Clause *cla = F, *precla = NULL;
	Literal *lit = NULL;
	while (cla) {
		//des cla
		Literal *pre = NULL;
		lit = cla->firstliteral;
		while(lit) {
			//des lit
			pre = lit;
			lit = lit->nextliteral;
			free(pre);
		}
		precla = cla;
		cla = cla->nextclause;
		free(precla);
	}
}//DestroyFormula
void AddClause(Clause ** F, int data) {
	//add at the head
	Clause *c = (Clause *)malloc(sizeof(Clause));
	c->firstliteral = (Literal *)malloc(sizeof(Literal));
	c->firstliteral->data = data;
	c->firstliteral->nextliteral = NULL;
	c->nextclause = (*F);
	*F = c;
}//AddClause
int Nextliteral(Clause * F, int litnum) {
	//get next lit
	int max = 0, next = 0;
	Clause *cla;
	Literal *lit;
	for (int i = 1; i <= litnum; i++) {
		//find the most
		int posnum = 0, negnum = 0;
		cla = F;
		lit = NULL;
		while (cla) {
			lit = cla->firstliteral;
			while(lit) {
				if (lit->data == i) {
					posnum ++;
				} else if (lit->data == -i) {
					negnum ++;
				}
				lit = lit->nextliteral;
			}
			cla = cla->nextclause;
		}
		if (negnum + posnum > max) {
			next = posnum > negnum ? i : -i;
			max = negnum + posnum;
		}
	}
	return next;
}//Nextliteral

void SetVar(Formula F,int v) {
	int i;
	int p = Abs(v), q = (v>0) ? SATISFIED : SHRUNK;
	F->lit[p].pos_occur++;
	for(i = 0; i < F->lit[p].pos_occur; i++) {
		//遍历所有含v的子句
		int j = F->lit[p].pos_in_clauses[i];//遍历
		if(F->cla[j].is_satisfied) continue;
		F->cla[j].is_satisfied = YES;
		F->changes[changes_index++].clause_index = j;
//		n_changes[depth][SATISFIED]++;
	}
	for(i = 0; i < F->lit[p].neg_occur; i++) {
		int j = F->lit[p].neg_in_clauses[i];
		if(F->cla[j].is_satisfied) continue;
		int k = F->lit[p].neg_in_locs[i];
		F->cla[j].current_length--;
		clauses[j].binary_code -= ((1 << k));
		F->changes[changes_index].clause_index = j;
		F->changes[changes_index++].literal_index = k;
//		n_changes[depth][SHRUNK]++;
		if(F->cla[j].current_length == 1) {
			int loc = int(log2(F->cla[j].binary_code));
			int w = F->cla[j].literals[loc];
			int s = Abs(w), t = (w>0) ? SATISFIED : SHRUNK;
			linfo[s][t].antecedent_clause = j;
			if(linfo[s][(!t)].is_unit == YES) {
				contradictory_unit_clauses = TRUE;
				conflicting_literal = w;
			} else if(linfo[s][t].is_unit == NO) {
				gucl_stack[n_gucl] = clauses[j].current_ucl = w;
				linfo[s][t].is_unit = YES;
				++n_gucl;
			}
		}
	}
	if(depth && backtrack_level == depth-1)
		++backtrack_level;
	++depth;
	linfo[p][SATISFIED].is_assigned = YES;
	linfo[p][SHRUNK].is_assigned = YES;
}

status DPLL(Clause * F, int litnum, int model[], changes_info *changes, int *change_index, int *pos_change, int *neg_change) {
	//dpll algorithm
	Clause * Fornew = CopyFormula(F);//copy the former formula
	Clause *cla;
	Literal *lit;
	while (1) {
		//unit rule
		cla = Fornew;
		lit = NULL;
		int flag = 0;//record if unit
		while (cla) {
			//not finished
			if (IsEmptyClause(cla)) {
				//empty clause,error
				DestroyFormula(Fornew);
				return ERROR;
			}
			lit = cla->firstliteral;
			if (IsUnitClause(cla)) {
				//unit clause
				int value = lit->data > 0 ? 1 : 0;//judge pos or neg
				model[Abs(lit->data) - 1] = value;//assign literal
				flag = 1;//is unit
				int result = SortFormula(&Fornew, lit->data);
				if (result) return result;//FINISHED or ERROR
				break;
			} //if
			cla = cla->nextclause;
		}//while
		if (!flag) break;//not exist unit
	}
	if (!Fornew) return FINISHED;// empty, success.
	int litnext = Nextliteral(Fornew, litnum);// choose next lit
	AddClause(&Fornew, litnext);
	if(DPLL(Fornew, litnum, model, changes, change_index, pos_change,neg_change) == 1) {
		//ok
		DestroyFormula(Fornew);
		return FINISHED;
	} else {
		//error,change the assign
		Clause *bak = Fornew;
		Fornew = Fornew->nextclause;
		free(bak->firstliteral);
		free(bak);//delete the former assign
		AddClause(&Fornew, -litnext);
		return DPLL(Fornew, litnum, model, changes, change_index, pos_change,neg_change);
	}
}//DPLL
