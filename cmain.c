#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>

#include<hbwmalloc.h>

#include "zmm_mul.h"

//#define DEBUG
#define LOOP 30

void combine_29bit(unsigned int* data, unsigned int* result);
void split_29bit(unsigned int* data, unsigned int* result, int digits);

void combine_28bit(unsigned int* data, unsigned int* result, int digits);
void split_28bit(unsigned int* data, unsigned int* result, int digits);

void multiply(unsigned int* a, unsigned int* b, unsigned int* t);
void hbw_multiply(unsigned int* a, unsigned int* b, unsigned int* t);
void BigMultiply(unsigned int* A, unsigned int* B, unsigned int* T);
void gmp_mul();

int main(int argc, char** argv){

	if(argc != 2){
		printf("Usage : %s num\n", argv[0]);
		exit(0);
	}

    unsigned int* data_a_hbw = hbw_calloc((2*N), sizeof(int));
    unsigned int* data_b_hbw = hbw_calloc((2*N), sizeof(int));
    unsigned int* t_hbw = hbw_calloc((4*M),sizeof(long));

    unsigned int* data_a = calloc((2*N), sizeof(int));
    unsigned int* data_b = calloc((2*N), sizeof(int));
    unsigned int* A = calloc((2*N), sizeof(int));
    unsigned int* B = calloc((2*N), sizeof(int));
    unsigned int* T = calloc((4*N), sizeof(int));
	
    unsigned int* t = calloc((4*M),sizeof(long));
    

    if(!(data_a_hbw && data_b_hbw && t_hbw && data_a && data_b && t && A && B && T)){
		puts("malloc error");
		exit(1);
    }

    int i,j;

    // initialize
    for(j=0; j<N; j++){
		data_a_hbw[j] = data_a[j] = A[j] = 0xffffffff;
		data_b_hbw[j] = data_b[j] = B[j] = 0xffffffff;
    }
    
    struct timeval s, e;
    double total = 0.0;
    double time = 0.0;

    // optimized multiply
    for(i=0; i<LOOP; i++){
	
	    for(j=0; j<2*(2*M+1); j++) t_hbw[j] = 0;

    	gettimeofday(&s, NULL);
	    hbw_multiply(data_a_hbw, data_b_hbw, t_hbw);
	    gettimeofday(&e, NULL);
	    
		total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;

    }
    
    time = (total / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);
    

	total = 0.0;

    for(i=0; i<LOOP; i++){
	
	    for(j=0; j<2*(2*M+1); j++) t[j] = 0;

    	gettimeofday(&s, NULL);
	    multiply(data_a, data_b, t);
	    gettimeofday(&e, NULL);
	    
		total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;

    }

    time = (total / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);

	// use GMP
	gmp_mul(argv[1]);


    // normal multiply
    for(i=0; i<30; i++){
      for(j=0; j<(2*N+1); j++) T[j] = 0;

      gettimeofday(&s, NULL);
      BigMultiply(A, B, T);
      gettimeofday(&e, NULL);

      total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;
    }
    time = (total / (1.0 * LOOP)) * 1000 * 1000;
    //printf("%lf\n", time);
	
	FILE *result = fopen("result_t.txt", "w");
	
	for(i=2*N-1; i>=0; i--)
		fprintf(result, "%08x", t[i]);
	
	fprintf(result, "\n");

	fclose(result);

#ifdef DEBUG
	// Error check
    int flag = 0;
    for(i=2*N-1; i>=0; i--){
		if(t[i] != T[i]){
	    	printf("%d\n%x %x\n", i, t_hbw[i], T[i]);
			flag = 1;
		}
	
    }
    if(!flag) puts("-------- No Error --------");
#endif

	hbw_free(data_a_hbw);
	hbw_free(data_b_hbw);
	hbw_free(t_hbw);
    free(t);
    free(A);
    free(B);
    free(T);
    free(data_a);
    free(data_b);
    
    return 0;
}
