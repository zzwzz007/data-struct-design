#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define NEG -1
#define UNKNOWN 0

void PrintMenu(int n){
	switch(n) {
		case 1:
			printf("Choose Function: \n1.CNF solver\n");
		case 2:
			printf("input the name of file:");
	}
}

int ResultPrint(int model[], int litnum) {
	printf("> INFO: Now asSignment model:\n");
	for(int i = 0; i < litnum; i++) {
		printf("---");
	}
	printf("--------\n Varible:");
	for(int i = 0; i < litnum; i++) {
		printf("%2d ", i + 1);
	}
	printf("\nAsSign: ");
	for(int i = 0; i < litnum; i++) {
		if(model[i] != UNKNOWN) {
			printf("%2d ", (model[i] + 1) / 2);
		} else {
			printf("%2c ", 'N');
		}
	}
	printf("\n");
	for(int i = 0; i < litnum; i++) {
		printf("---");
	}
	printf("--------\n");
}
