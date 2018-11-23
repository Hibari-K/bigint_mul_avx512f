#include<stdio.h>
#include<gmp.h>
#include<stdlib.h>
#include<sys/time.h>

#include "zmm_mul.h"

//#define LOOP 30.0
#define BASE 16

void gmp_mul(char* arg1, char* arg2){
	mpz_t a, b, res;

/*
	mpz_init2(a, 8192);
	mpz_init2(b, 8192);
	mpz_init2(res, 16384);
*/
	
	mpz_init(a);
	mpz_init(b);
	mpz_init(res);
	

	int i;
	
	struct timeval s, e;
	double total = 0.0;
	double time = 0.0;
	
	
	mpz_set_str(a,arg1,BASE);
	mpz_set_str(b,arg2,BASE);

	//mpz_out_str(stdout, 16, b);
	//mpz_set(b, a);

	for(i=0; i<LOOP; i++){

		gettimeofday(&s, NULL);
		mpz_mul(res, a, b);
		gettimeofday(&e, NULL);

		total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;
	}

	time = (total / LOOP*1.0)*1000*1000;
	//printf("GMP\t\t: Average time = %lf [us]\n", time);
    printf("%lf,", time);


	FILE *result = fopen("result_gmp.txt", "w");

	mpz_out_str(result, BASE, res);
	fprintf(result, "\n");

	fclose(result);

	//mpz_out_str(stdout, 16, b);
	//printf("\n");

	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(res);

	//free(str_b);

}
