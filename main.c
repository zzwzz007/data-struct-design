
#include "solver.h"

int main() {
	PrintMenu(1);
	PrintMenu(2);
	char filename[30];
	scanf("%s",filename);
	int litnum,clunum;
	Clause  *F=ReadToFormula(filename,&litnum,&clunum);
	int *model = (int *)malloc(litnum * sizeof(int));
	for (int i = 0; i < litnum; i++)
		model[i] = UNKNOWN;
	int res1 = DPLL(F, litnum, model, 1);
	if (res1 == 1) {
		printf("> INFO: Found solution.\n");
		ResultPrint(model, litnum);
	} else printf("> INFO: No solution found.\n");
	return 0;
}
