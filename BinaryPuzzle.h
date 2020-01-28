#include "solver.h"

int Getlitnum(int n){
	int num;
	num = (3*n+1);//{(1..k1->1..n1,1..k0->1..n0,1..k)*n,+1.. }=3n+1
	num *= n*(n-1);//row and column
	num += n*n;//num of squares
	return num;
}
int Getclanum(int n,int origin){
	int num;
	num = origin;
	//rule 1:consecutive ways in each row is (n-2),each ways has 2 clauses,for rows and column
	num += (n-2) * 2 * n * 2;
	//rule 2:equal ones and zeros,C(n/2+1,n) ways in each row
	int p = n / 2 + 1, q = n;
	int numerator = q,denominator = p;
	for (int i = 1; i <= p - 1; ++i)
		numerator *= q - i;
	for (int i = 1; i <= p-1; ++i)
		denominator *= p - i;
	num += (numerator / denominator) * 2 * n * 2;
	//rule 3:each 1..k1/1..k create 3 clauses,each 1.. create (n+1) clauses,for row and column
	num += (10 * n + 1) * n * (n-1);
	return num;
}
void add_rule_1(Formula *F, int n, int *begin){
	//add row
	for (int i = 0; i < n; ++i)
	{
		for (int j = i * n + 1; j <= (i + 1) * n - 2; ++j)
		{
			(*begin)++;
			F->cla[*begin].literals = (int *)malloc(sizeof(int));
			Addlit(F, j, *begin, 1);
			Addlit(F, j + 1, *begin, 2);
			Addlit(F, j + 2, *begin, 3);
			Finish_addlit(F, *begin, 3);
			(*begin)++;
			F->cla[*begin].literals = (int *)malloc(sizeof(int));
			Addlit(F, -j, *begin, 1);
			Addlit(F, -(j + 1), *begin, 2);
			Addlit(F, -(j + 2), *begin, 3);
			Finish_addlit(F, *begin, 3);
		}
	}
	//add column
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n - 2; ++j)
		{
			(*begin)++;
			int j_loc,k_loc,m_loc;
			j_loc = (j - 1) * n + i;
			k_loc = j_loc + n;
			m_loc = k_loc + n;
			F->cla[*begin].literals = (int *)malloc(sizeof(int));
			Addlit(F, j_loc, *begin, 1);
			Addlit(F, k_loc, *begin, 2);
			Addlit(F, m_loc, *begin, 3);
			Finish_addlit(F, *begin, 3);
			(*begin)++;
			F->cla[*begin].literals = (int *)malloc(sizeof(int));
			Addlit(F, -j_loc, *begin, 1);
			Addlit(F, -k_loc, *begin, 2);
			Addlit(F, -m_loc, *begin, 3);
			Finish_addlit(F, *begin, 3);
		}
	}
}
void add_rule_2(Formula *F, int n, int *begin){
	//add row
	for (int i = 0; i < n; ++i)
	{
		for (int j = i * n + 1; j <= (i + 1) * n - 3; ++j)
		{
			for (int k = j + 1; k <= (i + 1) * n - 2; ++k)
			{
				for (int m = k + 1; m <= (i + 1) * n - 1; ++m)
				{
					for (int p = m + 1; p <= (i + 1) * n; ++p){
						(*begin)++;
						F->cla[*begin].literals = (int *)malloc(sizeof(int));
						Addlit(F, j, *begin, 1);
						Addlit(F, k, *begin, 2);
						Addlit(F, m, *begin, 3);
						Addlit(F, p, *begin, 4);
						Finish_addlit(F, *begin, 4);
						(*begin)++;
						F->cla[*begin].literals = (int *)malloc(sizeof(int));
						Addlit(F, -j, *begin, 1);
						Addlit(F, -k, *begin, 2);
						Addlit(F, -m, *begin, 3);
						Addlit(F, -p, *begin, 4);
						Finish_addlit(F, *begin, 4);
					}
				}
			}
		}
	}
	//add column
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n - 3; ++j)
		{
			for (int k = j + 1; k <= n - 2; ++k)
			{
				for (int m = k + 1; m <= n - 1; ++m)
				{
					for (int p = m + 1; p <= n; ++p){
						(*begin)++;
						F->cla[*begin].literals = (int *)malloc(sizeof(int));
						int j_loc,k_loc,m_loc,p_loc;
						j_loc = (j - 1) * n + i;
						k_loc = (k - 1) * n + i;
						m_loc = (m - 1) * n + i;
						p_loc = (p - 1) * n + i;
						Addlit(F, j_loc, *begin, 1);
						Addlit(F, k_loc, *begin, 2);
						Addlit(F, m_loc, *begin, 3);
						Addlit(F, p_loc, *begin, 4);
						Finish_addlit(F, *begin, 4);
						(*begin)++;
						F->cla[*begin].literals = (int *)malloc(sizeof(int));
						Addlit(F, -j_loc, *begin, 1);
						Addlit(F, -k_loc, *begin, 2);
						Addlit(F, -m_loc, *begin, 3);
						Addlit(F, -p_loc, *begin, 4);
						Finish_addlit(F, *begin, 4);
					}
				}
			}
		}
	}
}
void add_rule_3(Formula *F, int n, int *begin){
	int p, q, t,k = 0,x = n*n;//p:groups num;q:formmer num loc;t:latter num loc;k:rec the round times;x:cur loc
	int num1=x;
	int x1,x2;
	//row
	while(1){
		t=0;
		for (int i = 1; i < n - k; ++i)
		{
			t++;
			for(int j = 0;j<n;j++){
				x++;
				p = (x-num1) / 3;
				q = p % n;
				q++;
				x1=q+k*n ;
				x2=x1+t*n;

				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x1, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x2, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -x1, *begin, 1);
				Addlit(F, -x2, *begin, 2);
				Addlit(F, x, *begin, 3);
				Finish_addlit(F, *begin, 3);

				x++;
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -x1, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -x2, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x1, *begin, 1);
				Addlit(F, x2, *begin, 2);
				Addlit(F, x, *begin, 3);
				Finish_addlit(F, *begin, 3);

				x++;
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -(x-1), *begin, 1);
				Addlit(F, x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -(x-2), *begin, 1);
				Addlit(F, x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x-1, *begin, 1);
				Addlit(F, x-2, *begin, 2);
				Addlit(F, -x, *begin, 3);
				Finish_addlit(F, *begin, 3);


			}
		}
		k++;
		if(k==n-1) break;
	}
	k=0;
	for(int j=0;j<n*(n-1)/2;j++){
		x++;
		(*begin)++;
		F->cla[*begin].literals = (int *)malloc(sizeof(int));
		Addlit(F, -x, *begin, 1);
		for(int i=1;i<=n;i++){
			F->cla[(*begin)+i].literals = (int *)malloc(sizeof(int));
			Addlit(F, x, (*begin)+i, 1);
			Addlit(F, num1+3*(i+k*n), (*begin)+i, 2);
			Finish_addlit(F, (*begin)+i, 2);
			Addlit(F, -(num1+3*(i+k*n)), *begin, i+1);
		}
		Finish_addlit(F, *begin, n+1);
		(*begin)+=n;
		k++;
	}
	//column
	k=1;
	int num2=x;
	while(1){
		t=0;
		for (int i = 0; i < n - k; ++i)
		{
			t++;
			for(int j = 0;j<n;j++){
				x++;
				p = (x-num2) / 3;
				q = p % n;
				x1=q*n+k;
				x2=x1+t;

				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x1, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x2, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -x1, *begin, 1);
				Addlit(F, -x2, *begin, 2);
				Addlit(F, x, *begin, 3);
				Finish_addlit(F, *begin, 3);

				x++;
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -x1, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -x2, *begin, 1);
				Addlit(F, -x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x1, *begin, 1);
				Addlit(F, x2, *begin, 2);
				Addlit(F, x, *begin, 3);
				Finish_addlit(F, *begin, 3);

				x++;
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -(x-1), *begin, 1);
				Addlit(F, x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, -(x-2), *begin, 1);
				Addlit(F, x, *begin, 2);
				Finish_addlit(F, *begin, 2);
				(*begin)++;
				F->cla[*begin].literals = (int *)malloc(sizeof(int));
				Addlit(F, x-1, *begin, 1);
				Addlit(F, x-2, *begin, 2);
				Addlit(F, -x, *begin, 3);
				Finish_addlit(F, *begin, 3);
			}
		}
		k++;
		if(k==n) break;
	}
	k=0;
	for(int j=0;j<n*(n-1)/2;j++){
		x++;
		(*begin)++;
		F->cla[*begin].literals = (int *)malloc(sizeof(int));
		Addlit(F, -x, *begin, 1);
		for(int i=1;i<=n;i++){
			F->cla[(*begin)+i].literals = (int *)malloc(sizeof(int));
			Addlit(F, x, (*begin)+i, 1);
			Addlit(F, num2+3*(i+k*n), (*begin)+i, 2);
			Finish_addlit(F, (*begin)+i, 2);
			Addlit(F, -(num1+3*(i+k*n)), *begin, i+1);
		}
		Finish_addlit(F, *begin, n+1);
		(*begin)+=n;
		k++;
	}
}
void add_origin(Formula *F, char *filename, int n, int *begin){
	FILE *fp;
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("File open error\n ");
		exit(-1);
	}
	int lit;
	int num1,num2;
	int p,q;
	while(!feof(fp)){
		fscanf(fp,"%d",&lit);
		(*begin)++;
		F->cla[*begin].literals = (int *)malloc(sizeof(int));
		p = Abs(lit);
		q = Symbol(lit);
		num1 = p / 10;
		num2 = p % 10;
		lit = (num1 - 1) * n + num2;
		if(!q) lit *= -1;
		Addlit(F, lit, *begin, 1);
		Finish_addlit(F, *begin, 1);
	}
	fclose(fp);
}
int GetOrigin(char *filename){
	FILE *fp;
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("File open error\n ");
		exit(-1);
	}
	int x,n=0;
	while(!feof(fp)){
		fscanf(fp,"%d",&x);
		if(x) n++;
	}
	return n;
}
Formula *ReadToSudoku(int n,char *filename){
	int clanum,litnum;
	litnum = Getlitnum(n);
	int origin_lit_num=GetOrigin(filename);
	clanum = Getclanum(n,origin_lit_num);
	Formula *F = InitFormula(litnum, clanum);
	F->original_lit = origin_lit_num;
	int begin = 0;
	add_origin(F,filename,n,&begin);
	add_rule_1(F,n,&begin);
	add_rule_2(F,n,&begin);
	add_rule_3(F,n,&begin);
	return F;
}
