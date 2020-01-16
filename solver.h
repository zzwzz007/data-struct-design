#include "cnfparser.h"
int Abs(int num) {
	return num > 0 ? num : -num;
}

int Sign(int num) {
	return num > 0 ? 1 : -1;
}

int IsEmptyClause(Clause *C) {
	if(C->firstliteral==NULL) {
		return 1;
	} else {
		return 0;
	}
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

Clause* CopuFormula(Clause * L) {
	// echo(L);
	Clause * L_copy = copy_sub_L(L);
	Clause *lp = L->nextclause, *lp_new = L_copy;
	while (lp) {
		lp_new->nextclause = copy_sub_L(lp);
		if (!lp->nextclause) {
			break;
		}
		lp_new = lp_new->nextclause;
		lp = lp->nextclause;
	}
	return L_copy;
}

int RemoveClause(Clause ** L,Clause **pre_lp,Clause **lp) {
	Literal *head = (*lp)->firstliteral;
	if((*lp)==*L) {
		//first clause
		*L = (*L)->nextclause;
		free((*lp));
		(*lp) = (*L);
		if (*L == NULL) {
			return 1;    // finish dpll.
		}
	} //if
	else {
		(*pre_lp)->nextclause = (*lp)->nextclause;
		free((*lp));
		(*lp) = (*pre_lp)->nextclause;
	} //else
	Literal * bak = NULL;
	while(head) {
		bak = head;
		head = head->nextliteral;
		free(bak);
	}
	return 0;//not finished
}

int Removeliteral(Literal **np,Literal **pre_np,Clause **lp) {
	if((*np)==(*lp)->firstliteral) {
		(*lp)->firstliteral = (*lp)->firstliteral->nextliteral;
		free((*np));
		if (IsEmptyClause((*lp))) {
			return 1;
		}
		(*np) = (*lp)->firstliteral;
	} else {
		(*pre_np)->nextliteral = (*np)->nextliteral;
		free((*np));
		(*np) = (*pre_np)->nextliteral;
	}
	return 0;
}

int SortFormula(Clause ** L, int v) {
	Clause *lp = *L, *pre_lp = *L;
	while (lp) {
		if (IsEmptyClause(lp)) {
			return -1;
		}
		int remove_clause = 0;
		Literal *np = lp->firstliteral, *pre_np = lp->firstliteral;
		while(np) {
			if (np->data == -v) {

				if (Removeliteral(&np,&pre_np,&lp)) {
					return -1;
				}
				continue;
			} else if (np->data == v) {
				if(RemoveClause( L,&pre_lp,&lp)) {
					return 1;
				}
				remove_clause = 1;
				break;      // np was freed, skip the loop;
			}
			pre_np = np;
			np = np->nextliteral;
		}
		if (!remove_clause) {
			pre_lp = lp;
			lp = lp->nextclause;
		}
	}
	return 0;   // normal status.
}

int DestroyFormula(Clause * L) {
	Clause *lp = L, * pre_l = NULL;
	Literal *np = NULL;
	int i = 0;
	while (lp) {
		Literal *pre = NULL;
		np = lp->firstliteral;
		while(np) {
			pre = np;
			np = np->nextliteral;
			free(pre);
		}
		pre_l = lp;
		lp = lp->nextclause;
		free(pre_l);
		i++;
	}
}

int AddClause(Clause ** F, int data) {
	Clause *c = (Clause *)malloc(sizeof(Clause));
	// p->len = 1;
	c->firstliteral = (Literal *)malloc(sizeof(Literal));
	c->firstliteral->data = data;
	c->firstliteral->nextliteral = NULL;
	c->nextclause = (*F);
	*F = c;
}

int Nextliteral(Clause * F, int litnum) {
	int max = 0, next = 0;
	Clause *cla;
	Literal *lit;
	for (int i = 1; i <= litnum; i++) {
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
}

int DPLL(Clause * L, int litnum, int model[], int chooselit) {
	Clause * new_L = CopuFormula(L);
	// to find if there is single clause:
	while (1) {
		Clause *lp = new_L;
		Literal *np = NULL;
		int have_single = 0;
		while (lp) {
			np = lp->firstliteral;
			while (np) {
				if (lp->firstliteral && !lp->firstliteral->nextliteral) {
					int value = np->data > 0 ? 1 : NEG;
					model[Abs(np->data) - 1] = value;
					have_single = 1;
					int res = SortFormula(&new_L, np->data);
					if (res != 0) {
						return res;
					}
					break;
				} else if (lp->firstliteral == NULL) {
					DestroyFormula(new_L);
					return -1;
				}
				np = np->nextliteral;
			}
			if (have_single) {
				break;
			}
			lp = lp->nextclause;
		}
		if (!have_single) {
			break;
		}
	}
	// echo(L);
	if (new_L == NULL) {  // empty, succes.
		return 1;
	}
	// choose v and asSign:
	int next_v;
	if (chooselit) {
		next_v = Nextliteral(new_L, litnum);
	} else {
		next_v = new_L->firstliteral->data;
	}
	model[Abs(next_v) - 1] = next_v > 0 ? 1 : NEG;
	AddClause(&new_L, next_v);
	if(DPLL(new_L, litnum, model, chooselit) == 1) {
		DestroyFormula(new_L);
		return 1;
	} else {
		Clause *bak = new_L;
		new_L = new_L->nextclause;
		free(bak->firstliteral);
		free(bak);
		AddClause(&new_L, -next_v);
		return DPLL(new_L, litnum, model, chooselit);
	}
}
