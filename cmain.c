#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>

#include "zmm_mul.h"

//#define DEBUG

void combine_29bit(unsigned int* data, unsigned int* result);
void split_29bit(unsigned int* data, unsigned int* result, int digits);

void combine_28bit(unsigned int* data, unsigned int* result, int digits);
void split_28bit(unsigned int* data, unsigned int* result, int digits);

void multiply(unsigned int* a, unsigned int* b, unsigned int* t);
void BigMultiply(unsigned int* A, unsigned int* B, unsigned int* T);
void gmp_mul();

int main(int argc, char** argv){

	if(argc != 2){
		printf("Usage : %s num\n", argv[0]);
		exit(0);
	}

    unsigned int* data_a = calloc((2*N), sizeof(int));
    unsigned int* data_b = calloc((2*N), sizeof(int));
    unsigned int* A = calloc((2*N), sizeof(int));
    unsigned int* B = calloc((2*N), sizeof(int));
    unsigned int* T = calloc((4*N), sizeof(int));
    unsigned int* result_t = calloc((4*N), sizeof(int));
	
    unsigned int* a = calloc((4*M),sizeof(int));
    unsigned int* b = calloc((4*M),sizeof(int));
    unsigned int* t = calloc((4*M),sizeof(long));
    

    if(!(result_t && data_a && data_b && a && b && t && A && B && T)){
		puts("malloc error");
		exit(1);
    }

    int i,j;

    // initialize
    for(j=0; j<N-1; j++){
		data_a[j] = A[j] = 0x11111111;
		data_b[j] = B[j] = 0x11111111;
    }
    
    struct timeval s, e;
    double total = 0.0;
    double time = 0.0;

    // optimized multiply
    for(i=0; i<30; i++){
	
	    for(j=0; j<2*(2*M+1); j++) t[j] = 0;

    	gettimeofday(&s, NULL);
	    split_28bit(data_a, a, SPLITDIGITS);
    	split_28bit(data_b, b, SPLITDIGITS);

	    multiply(a, b, t);

    	combine_28bit(t, result_t, COMDIGITS);
	    gettimeofday(&e, NULL);
	    
		total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;

    }
    
    time = (total / 30.0) * 1000 * 1000;
    printf("%lf,", time);
    //printf("\nOptimized\t: Average time = %lf [us]\n", time);
    
	total = 0.0;
    
    // normal multiply 
    for(i=0; i<30; i++){
        for(j=0; j<(2*N+1); j++) T[j] = 0;

    	gettimeofday(&s, NULL);
	    BigMultiply(A, B, T);
    	gettimeofday(&e, NULL);

		total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;
    }

    time = (total / 30.0) * 1000 * 1000;
    printf("%lf,", time);
    //printf("Normal\t\t: Average time = %lf [us]\n", time);

	// use GMP
	gmp_mul(argv[1]);


#ifdef DEBUG
	// Error check
    int flag = 0;
    for(i=2*N; i>=0; i--){
		if(result_t[i] != T[i]){
	    	printf("%d\n%x %x\n", i, result_t[i], T[i]);
			flag = 1;
		}
    }
    if(!flag) puts("-------- No Error --------");
#endif

    free(a);
    free(b);
    free(t);
    free(A);
    free(B);
    free(T);
    free(data_a);
    free(data_b);
    free(result_t);
    
    return 0;
}
