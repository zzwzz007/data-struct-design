#include "cnfparser.h"

void PrintMenu(int n){
	switch(n) {
		case 0:
			printf("Choose Function: \n1.CNF solver\n2.Binary Puzzle\n");
			break;
		case 1:
			printf("input the name of file(not include .cnf):");
			break;
		case 2:
			printf("input the name of sudoku file:");
			break;

	}
}
void ResultPrint(int result, Formula *F, char *filename,double time,int mode,int num) {
	FILE *fp;
	if ((fp=fopen(filename,"wt"))==NULL) {
		printf("File open error!\n ");
		exit(-1);
	}
	if(mode==1){
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
	}
	else{
		if(result==1){
			printf("Found solution!\n");
			for (int i = 1; i <= num*num; ++i)
			{
				printf("%2d ", F->assign[i].type);
				fprintf(fp, "%d ", F->assign[i].type);
				if(i % num == 0) {
					printf("\n");
					fprintf(fp, "\n");
				}
			}
		}
		else {
			printf("Not Sollution!\n");
			fprintf(fp, "s 0");
		}
	}
	fprintf(fp, "\nt %lf\n", time);
	fclose(fp);
}
double Timeout(clock_t start, clock_t finish){
	return ((double)(finish - start) / CLOCKS_PER_SEC);
}
