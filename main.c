#include "cnfparser.h"

int main(){
	char filename[30];
	printf("input the name of file:");
	scanf("%s",filename);
	int litnum,clunum;
	Clause  *F=ReadToFormula(filename,&litnum,&clunum);
	int *model = (int *)malloc(litnum * sizeof(int));
        for (int i = 0; i < litnum; i++)
            model[i] = UNKNOWN;
    int res1 = dpll_valilla(F, litnum, model, 1);
    if (res1 == 1) {
        printf("> INFO: Found solution.\n");
        ResultPrint(model, litnum);
    } 
    else printf("> INFO: No solution found.\n");
	return 0;
}
