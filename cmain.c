#include<stdio.h>
#include<stdlib.h>

#include<hbwmalloc.h>
#include "zmm_mul.h"


void multiply(zmm_t a, zmm_t b, zmm_t t);

int main(int argc, char** argv){

	if(argc != 2){
		printf("Usage : %s num\n", argv[0]);
		exit(0);
	}
	
	int lp = atoi(argv[1]);

	zmm_t a, b, t;

	// initialize
	PTR(a) = hbw_calloc((896), sizeof(long));
	PTR(b) = hbw_calloc((896), sizeof(long));
	PTR(t) = hbw_calloc((1792),sizeof(long));

	
    if(!(PTR(a) && PTR(b) && PTR(t))){
		puts("malloc error");
		exit(1);
    }

    int i,j;

    // initialize
	srand(time(NULL));

    for(j=0; j<lp; j++){
		PTR(a)[j] = rand();
		PTR(b)[j] = rand(); 
    }
	SIZ(a) = lp;
	SIZ(b) = lp;
	SIZ(t) = 0;


    // optimized multiply
	multiply(a, b, t);


	// print multiply result to file
	FILE *result = fopen("result_t.txt", "w");

	fprintf(result, "%x", PTR(t)[2*lp-1]);
	for(i=2*lp-2; i>=0; i--)
		fprintf(result, "%08x", PTR(t)[i]);
	
	fprintf(result, "\n");

	fclose(result);


	hbw_free(PTR(a));
	hbw_free(PTR(b));
	hbw_free(PTR(t));
    free(A);
    free(B);
    free(T);
	free(str1);
	free(str2);
    
    return 0;
}
