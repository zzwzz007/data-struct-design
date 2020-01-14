#include "cnfparser.h"

int main(){
	char filename[30];
	printf("input the name of file:");
	scanf("%s",filename);
	int litnum,clunum;
	Clause *F=ReadToFormula(filename,&litnum,&clunum);
	
	return 0;
}
