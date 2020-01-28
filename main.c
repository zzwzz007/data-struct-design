#include "BinaryPuzzle.h"

int main() {
	PrintMenu(0);
	int n;
	scanf("%d",&n);
	PrintMenu(n);
	char filename[80];
	clock_t start,finish;
	int result;
	Formula *F;
	scanf("%s",filename);
	int last;
	for (last = 0; last < 80; last++)
		if(filename[last]=='\0') break;
	Readcnf(filename,last);
	int num=6;
	switch(n){
		case 1:
			F = ReadToFormula(filename);
			start = clock();
			result = dpll(F);
			finish = clock();
			break;
		case 2:
			F = ReadToSudoku(num,filename);
			start = clock();
			result = dpll(F);
			finish = clock();
			break;
	}
	double t=Timeout(start,finish);
	Writeres(filename, last);
	ResultPrint(result, F, filename, t, n, num);
	printf("Spent time:%lf s (CLOCKS_PER_SEC=%d)\n", t ,CLOCKS_PER_SEC);
	return 0;
}
