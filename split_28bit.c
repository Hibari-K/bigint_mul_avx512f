#include<stdio.h>

#include "zmm_mul.h"

void split_28bit(unsigned int* data, unsigned int* result, int SPLITDIGITS){

	long rdi;
	long rsi;

    __asm__ volatile(
	
        // initialize
	    "movq $0xfffffff, %%rax;"
	    "movq %%rax, %%xmm1;"
	    "movdqa %%xmm1, %%xmm2;"
	    "psllq $28, %%xmm2;"
	    "movq %%xmm1, %%xmm3;"
	    "pslldq $7, %%xmm3;"
	    "movq %%xmm2, %%xmm4;"
	    "pslldq $7, %%xmm4;"
		::: "%rax"
		);
	    
	    /*
	      in inner,
	      xmm9 : tmp
	      xmm10: result
	    */
		for(rdi=0, rsi=0; rsi<=SPLITDIGITS; rdi+=14, rsi+=2){
		//for(rdi=0, rsi=0; rsi<=digits; rdi+=14, rsi+=2){

			__asm__ volatile(
			    "movdqu (%0, %1), %%xmm0;"
		    	// [0]
			    "movdqa %%xmm0, %%xmm10;"
		    	"pand %%xmm1, %%xmm10;"
		    	// [1]
			    "movdqa %%xmm0, %%xmm9;"
			    "pand %%xmm2, %%xmm9;"
	    		"psllq $4, %%xmm9;"
			    "pxor %%xmm9, %%xmm10;"
			    // [2]
	    		"movdqa %%xmm0, %%xmm9;"
		    	"pand %%xmm3, %%xmm9;"
			    "pslldq $1, %%xmm9;"
		    	"pxor %%xmm9, %%xmm10;"
			    //[3]
		    	"movdqa %%xmm0, %%xmm9;"
		    	"pand %%xmm4, %%xmm9;"
			    "psllq $12, %%xmm9;"
			    "pxor %%xmm9, %%xmm10;"
	    
	    		"movdqa %%xmm10, (%2, %3, 8);"

				::"r"(data), "r"(rdi), "r"(result), "r"(rsi)
			);
		}
}
