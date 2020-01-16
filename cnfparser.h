#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define NEG -1
#define UNKNOWN 0

// typedef struct Record
// {//记录
// 	int num;//数字
// 	struct Record *nextliteral;//下一个
// }Record;

typedef struct Literal
{
	//文字
	// int truth;//真值
	// int unasSigned;//未定义
	// int unsatisfied;//不满足
	// int satisfied;//满足
	int data;
	struct Literal *nextliteral;//下一个文字
}Literal;

typedef struct Clause
{//子句
	// int unasSigned;//未定义
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

int Abs(int num) {
    return num > 0 ? num : -num;
}

int Sign(int num) {
    return num > 0 ? 1 : -1;
}

int IsEmptyClause(Clause *C){
	if(C->firstliteral==NULL)
		return 1;
	else
		return 0;
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

int RemoveClause(Clause ** L,Clause **pre_lp,Clause **lp){
    Literal *head = (*lp)->firstliteral;
    if((*lp)==*L){
        //first clause
        *L = (*L)->nextclause;
        free((*lp));
        (*lp) = (*L);
        if (*L == NULL)
            return 1;   // finish dpll.
    } //if
    else {
        (*pre_lp)->nextclause = (*lp)->nextclause;
        free((*lp));
        (*lp) = (*pre_lp)->nextclause;
    }//else
    Literal * bak = NULL;
    while(head) {
        bak = head;
        head = head->nextliteral;
        free(bak);
    }
    return 0;//not finished
}

int Removeliteral(Literal **np,Literal **pre_np,Clause **lp){
    if((*np)==(*lp)->firstliteral){
        (*lp)->firstliteral = (*lp)->firstliteral->nextliteral;
        free((*np));
        if (IsEmptyClause((*lp)))
            return 1;
        (*np) = (*lp)->firstliteral;
    }
    else{
        (*pre_np)->nextliteral = (*np)->nextliteral;
        free((*np));
        (*np) = (*pre_np)->nextliteral;
    }
}

int Remove_v(Clause ** L, int v) {
    Clause *lp = *L, *pre_lp = *L;
    while (lp) {
        if (IsEmptyClause(lp))
            return -1;
        int remove_clause = 0;
        Literal *np = lp->firstliteral, *pre_np = lp->firstliteral;
        while(np) {
            if (np->data == -v) {
                if (Removeliteral(&np,&pre_np,&lp))
                    return -1;
                continue;
            } 
            else if (np->data == v) {
                if(RemoveClause( L,&pre_lp,&lp))
                	return 1;
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

int delete_L(Clause * L) {
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

int add_v(Clause ** L, int v_data) {
    Clause *p = (Clause *)malloc(sizeof(Clause));
    p->firstliteral = (Literal *)malloc(sizeof(Literal));
    p->firstliteral->data = v_data;
    p->firstliteral->nextliteral = NULL;
    p->nextclause = (*L);
    *L = p;
}

int get_next_v(Clause * L, int nv) {
    int max_num = 0, next_v = 0;
    for (int i = 0; i < nv; i++) {
        int now_pos_num = 0, now_neg_num = 0;
        Clause *lp = L;
        Literal *np = NULL;
        while (lp) {
            np = lp->firstliteral;
            while(np) {
                if (np->data == i + 1)
                    now_pos_num ++;
                else if (np->data == -i - 1)
                    now_neg_num ++;
                np = np->nextliteral;
            }
            lp = lp->nextclause;
        }
        if (now_neg_num + now_pos_num > max_num) {
            next_v = now_pos_num > now_neg_num ? i + 1 : -i - 1;
            max_num = now_neg_num + now_pos_num;
        }
    }
    return next_v;
}

int dpll_valilla(Clause * L, int nv, int model[], int choose_v) {
    Clause * new_L = copy_L(L);
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
                    // printf("> INFO: AsSign clause [%d].\n", np->data);
                    int res = Remove_v(&new_L, np->data);
                    // echo(new_L);
                    if (res != 0)
                        return res;
                    break;
                } else if (lp->firstliteral == NULL) {
                    delete_L(new_L);
                    return -1;
                }
                np = np->nextliteral;
            }
            if (have_single)
                break;
            lp = lp->nextclause;
        }
        if (!have_single)
            break;
    }
    // echo(L);
    if (new_L == NULL)    // empty, succes.
        return 1;
    // choose v and asSign:
    int next_v;
    if (choose_v)
        next_v = get_next_v(new_L, nv);
    else
        next_v = new_L->firstliteral->data;
    model[Abs(next_v) - 1] = next_v > 0 ? 1 : NEG;
		add_v(&new_L, next_v);
    if(dpll_valilla(new_L, nv, model, choose_v) == 1) {
        delete_L(new_L);
        return 1;
    } else {
        Clause *bak = new_L;
        new_L = new_L->nextclause;
        free(bak->firstliteral);
        free(bak);
        add_v(&new_L, -next_v);
        return dpll_valilla(new_L, nv, model, choose_v);
    }
}




int echo_model(int model[], int nv) {
    printf("> INFO: Now asSignment model:\n");
    for(int i = 0; i < nv; i++)
        printf("---");
    printf("--------\nVarible:");
    for(int i = 0; i < nv; i++) {
        printf("%2d ", i + 1);
    }
    printf("\nAsSign: ");
    for(int i = 0; i < nv; i++) {
        if(model[i] != UNKNOWN)
            printf("%2d ", (model[i] + 1) / 2);
        else
            printf("%2c ", 'N');
    }
    printf("\n");
    for(int i = 0; i < nv; i++)
        printf("---");
    printf("--------\n");
}
