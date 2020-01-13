#include "cnfparser.h"

int main(){
	printf("1\n" );
	Formula *F;
	F=(Formula *)malloc(sizeof(Formula));
	printf("number of clause:\n");
	int i;
	scanf("%d",&i);
	ReadToFormula(F,i);
	return 0;
}
