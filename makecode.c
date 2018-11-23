#include<stdio.h>
#include<stdlib.h>
#include "autocode.h"

void write_outer(FILE*, int*, int);
void write_inner(FILE*, int*, int);

char *sum = "tuvwpqrs";
char *former = "01234567";
char *latter = "89abcdef";


int main(int argc, char** argv){
	
	FILE *fp;

	if((fp = fopen("opt_mul.c", "w")) == NULL){
		puts("fopen Error");
		exit(1);
	}

	int arg[4];

	if(argc != 4){
		arg[1] = 8;
		arg[2] = 1;
		arg[3] = 1;
	}
	else{
		arg[1] = (int)strtol(argv[1], NULL, 10);
		arg[2] = (int)strtol(argv[2], NULL, 10);
		arg[3] = (int)strtol(argv[3], NULL, 10);

		if(arg[1] * arg[2] * arg[3] != 8){
			puts("main : Invalid argument.");
			exit(1);
		}
	}



	fprintf(fp, CODE_INIT);

	int i;
	
	for(i=0; i<8; i+=(8/arg[1]))
		write_outer(fp, arg, i);
	

	fprintf(fp, CODE_END);

	fclose(fp);

	return 0;
}


void write_outer(FILE *fp, int* arg, int i){

	int lp = 8/arg[1];

	//fprintf(fp, "\tfor(index_outer = 0; index_outer < DIGITS; index_outer += 16){\n\n");

	//LOAD_B
	
	int j;
	for(j=0; j<lp; j++){
		if(i+j != 0) { INCREMENT(former[i+j], former[i+j-1], latter[i+j], latter[i+j-1]) }
		PERM_B0(former[i+j], latter[i+j])
		PERM_B1(former[i+j], latter[i+j])
	}

	for(j=0; j<lp; j++){ CALC_1(sum[i+j], former[i+j]) }
	for(j=0; j<lp; j++){ CALC_2(sum[i+j], former[i+j], latter[i+j]) }
	for(j=0; j<lp; j++){ CALC_3(sum[i+j], former[i+j], latter[i+j]) }
	for(j=0; j<lp; j++){ CALC_4(sum[i+j], former[i+j], latter[i+j]) }
	for(j=0; j<lp; j++){ CALC_5(sum[i+j], former[i+j], latter[i+j]) }
	for(j=0; j<lp; j++){ CALC_6(sum[i+j], former[i+j], latter[i+j]) }
	for(j=0; j<lp; j++){ CALC_7(sum[i+j], latter[i+j]) }
/*
	for(j=i; j<i+(8/arg[1]); j+=arg[3]){
		if(j != i) LOAD_A_INIT
		write_inner(fp, arg, j);
	}
*/
	//fprintf(fp, "\t\trdi = (index_outer*2) + 16 * (((DIGITS - 1) / 8) + 1);\n\n");

	//fprintf(fp, "\t}\n");
}


void write_inner(FILE *fp, int* arg, int i){

	fprintf(fp, "\t\tfor(index_inner = 8; index_inner < DIGITS; index_inner += 8){\n\n");
	fprintf(fp, "\t\t\tindex = (index_inner * 2) + (index_outer * 2); //offset + padding\n");

	int j;
	for(j=0; j<arg[3]; j++)
		MUL_ADD_TMP(former[i+j], latter[i+j], sum[i+j])

	for(j=0; j<arg[3]; j++)
		ADD_TMP(sum[i+j])

	for(j=0; j<arg[3]; j++)
		STORE_INNER(sum[i+j])

	LOAD_A_INNER

	for(j=0; j<arg[3]; j++)
		LOAD_INNER(sum[i+j])

	fprintf(fp, "\t\t}\n");
}
