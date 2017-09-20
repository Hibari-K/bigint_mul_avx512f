#include<stdio.h>

#include "zmm_mul.h"

void BigMultiply(unsigned int* A, unsigned int* B, unsigned int* T){

    unsigned int i, j, carry;
    unsigned long result;

    for(i=0; i<N; i++){
    	carry = 0;
	    for(j=0; j<N; j++){
	        result = (long)A[j] * B[i] + T[i+j] + carry;
	    
    	    T[i+j] = LoWord(result);
	        carry = HiWord(result);
    	}
	    T[i+j] = carry;
    }
}
