#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>

#include<hbwmalloc.h>

#include "zmm_mul.h"

#define DEBUG
//#define LOOP 30.0

//void multiply(unsigned int* a, unsigned int* b, unsigned int* t);
void multiply(zmm_t a, zmm_t b, zmm_t t);
void BigMultiply(unsigned int* A, unsigned int* B, unsigned int* T, int loop);
void gmp_mul();

double total = 0.0;
double total_k = 0.0;
double total_s = 0.0;
double total_c = 0.0;
double total_a = 0.0;
double total_o = 0.0;

int main(int argc, char** argv){

	if(argc != 2){
		printf("Usage : %s num\n", argv[0]);
		exit(0);
	}
	
	int lp = atoi(argv[1]);

	zmm_t a, b, t;


	//unsigned int* restrict 
	PTR(a) = hbw_calloc((896), sizeof(long));
	//unsigned int* restrict 
	PTR(b) = hbw_calloc((896), sizeof(long));
	//unsigned int* restrict 
	PTR(t) = hbw_calloc((1792),sizeof(long));

    //unsigned int* restrict data_a __attribute__((aligned(64))) = calloc((896), sizeof(long));
    //unsigned int* restrict data_b __attribute__((aligned(64))) = calloc((896), sizeof(long));
    unsigned int* restrict A __attribute__((aligned(64))) = calloc((896), sizeof(int));
    unsigned int* restrict B __attribute__((aligned(64))) = calloc((896), sizeof(int));
    unsigned int* restrict T __attribute__((aligned(64))) = calloc((1792), sizeof(int));
	
    //unsigned int* restrict t __attribute__((aligned(64))) = calloc((1792),sizeof(long));
    
	// used in gmp_mul
	char* str1 = malloc(2048);
	char* str2 = malloc(2048);

    if(!(str1 && str2 && PTR(a) && PTR(b) && PTR(t) && A && B && T)){
		puts("malloc error");
		exit(1);
    }

    int i,j;

    // initialize
	srand(time(NULL));

    for(j=0; j<lp; j++){
		PTR(a)[j] = A[j] = rand();
		PTR(b)[j] = B[j] = rand(); 
    }
	SIZ(a) = lp;
	SIZ(b) = lp;
	SIZ(t) = 0;

	/*
	for(i=0; i<lp*8; i+=8)
		for(j=0; j<8; j++)
			str[i+j] = 'f';
	*/
	//str[i*8] = '\0';

	char *tmpa = str1;
	char *tmpb = str2;

	for(j=lp-1; j>=0; j--){
		snprintf(tmpa, 9, "%08x", A[j]);
		snprintf(tmpb, 9, "%08x", B[j]);
		tmpa += 8;
		tmpb += 8;
	}

/*
	for(j=lp-1; j>=0; j--)
		printf("%08x", data_a_hbw[j]);
	puts("");
	for(j=lp-1; j>=0; j--)
		printf("%08x", A[j]);
	puts("");
	printf("%s\n", str1);
*/

	struct timeval s;
	struct timeval e;
	double time = 0.0;

	// dummy multiply (for warming up)
	
    for(i=0; i<LOOP; i++){
	
	    for(j=0; j<(2*lp); j++) PTR(t)[j] = 0;

    	//gettimeofday(&s, NULL);
	    multiply(a, b, t);
	    //gettimeofday(&e, NULL);
	    
		//total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;

    }
    
    printf("0,");

	total = total_a = total_s = total_k = total_c = 0.0;
    // optimized multiply
    for(i=0; i<LOOP; i++){
	
	    for(j=0; j<(2*lp); j++) PTR(t)[j] = 0;

    	gettimeofday(&s, NULL);
	    multiply(a, b, t);
	    gettimeofday(&e, NULL);
	    
		total += (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;

    }
    
    time = (total_k / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);
    time = (total_s / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);
    time = (total_c / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);
    time = (total_a / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);
    time = (total_o / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);
    
	time = (total / (1.0 * LOOP)) * 1000 * 1000;
    printf("%lf,", time);
    
	total = 0.0;

	
	// use GMP
	gmp_mul(str1, str2);

	gmp_mul(str1, str2);
	puts("");

    // normal multiply

      BigMultiply(A, B, T, lp);

    //printf("%lf\n", time);
	
	FILE *result = fopen("result_t.txt", "w");
	

	fprintf(result, "%x", PTR(t)[2*lp-1]);
	for(i=2*lp-2; i>=0; i--)
		fprintf(result, "%08x", PTR(t)[i]);
	
	fprintf(result, "\n");

	fclose(result);

#ifdef DEBUG
	// Error check
    int flag = 0;
	/*
	printf("t_hbw digits = %d\n", 2*lp-1);
	for(i=2*lp-1; i>=0; i--)
		printf("%08x", t_hbw[i]);
	puts("");
	for(i=2*lp-1; i>=0; i--)
		printf("%08x", T[i]);
	puts("");
    */
	for(i=2*lp-1; i>=0; i--){
		if(PTR(t)[i] != T[i]){
	    	printf("%d\n%x %x\n", i, PTR(t)[i], T[i]);
			flag = 1;
		}
	
    }
    //if(!flag) puts("-------- No Error --------");
    if(!flag) fprintf(stderr, "-------- No Error --------\n"); 
#endif

	hbw_free(PTR(a));
	hbw_free(PTR(b));
	hbw_free(PTR(t));
    //free(t);
    free(A);
    free(B);
    free(T);
    //free(data_a);
    //free(data_b);
	free(str1);
	free(str2);
    
    return 0;
}
