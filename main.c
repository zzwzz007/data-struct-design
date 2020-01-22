#include "solver.h"

int main() {
//	PrintMenu(1);
//	PrintMenu(2);
	char filename[30];
	scanf("%s",filename);
	int last;
	for (last = 0; last < 30; last++)
	{
		if(filename[last]=='\0') break;
	}
	filename[last]='.';
	filename[last+1]='c';
	filename[last+2]='n';
	filename[last+3]='f';
	filename[last+4]='\0';//rename
	int litnum,clunum;
	Formula *F = ReadToFormula(filename,&litnum,&clunum);
	clock_t start,finish;
	start = clock();
	int result = dpll(F);
	finish = clock();
	double t=Timeout(start,finish);
	filename[last]='.';
	filename[last+1]='r';
	filename[last+2]='e';
	filename[last+3]='s';
	filename[last+4]='\0';//rename
	ResultPrint(result, F, filename, t);
	printf("Spent time:%lf s (CLOCKS_PER_SEC=%ld)", t ,CLOCKS_PER_SEC);

//	int result = DPLL_Pre(F, litnum, clunum, model);
//	
//	IsAllAssign(model, &result, litnum);
//	
//	filename[last]='.';
//	filename[last+1]='r';
//	filename[last+2]='e';
//	filename[last+3]='s';
//	filename[last+4]='\0';//rename
//	
	return 0;
}
