#include "solver.h"

int main() {
//	PrintMenu(1);
//	PrintMenu(2);
	char filename[80];
	scanf("%s",filename);
	int last;
	for (last = 0; last < 80; last++)
		if(filename[last]=='\0') break;
	Readcnf(filename,last);
	Formula *F = ReadToFormula(filename);
	clock_t start,finish;
	start = clock();
	int result = dpll(F);
	finish = clock();
	double t=Timeout(start,finish);
	Writeres(filename, last);
	ResultPrint(result, F, filename, t);
	printf("Spent time:%lf s (CLOCKS_PER_SEC=%d)\n", t ,CLOCKS_PER_SEC);
	return 0;
}
