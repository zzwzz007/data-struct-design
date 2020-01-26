#include "cnfparser.h"

void PrintMenu(int n){
	switch(n) {
		case 1:
			printf("Choose Function: \n1.CNF solver\n");
		case 2:
			printf("input the name of file(not include .cnf):");
	}
}
void ResultPrint(int result, Formula *F, char *filename,double time) {
	FILE *fp;
	if ((fp=fopen(filename,"wt"))==NULL) {
		printf("File open error!\n ");
		exit(-1);
	}
	if(result==1){
		printf("Found solution!\n");
		printf("Assign model:\n");
		// printf("------------------------------------------------------------\n");
		printf("Varible:");
		for(int i = 1; i <= F->litnum; i++) {
			printf("%2d ", i);
		}
		printf("\nAssign: ");
		for(int i = 1; i <= F->litnum; i++) {
			printf("%2d ", F->assign[i].type);
		}
		printf("\n");
		// printf("------------------------------------------------------------\n");
		fprintf(fp, "s 1\nv ");
		for(int i = 1; i <= F->litnum; i++){
			if(F->assign[i].type==1) fprintf(fp, "%d ", i);
			else fprintf(fp, "-%d ", i);
		}
	}
	else if(result==-1){
		//no sollution
		printf("No solution was founded!\n");
		fprintf(fp, "s 0");
	}
	else{
		//have at least one unknown
		printf("Could not decide the solution!\n");
		fprintf(fp, "s -1");
	}
	fprintf(fp, "\nt %lf\n", time);
	fclose(fp);
}
double Timeout(clock_t start, clock_t finish){
	return ((double)(finish - start) / CLOCKS_PER_SEC);
}
