#include<stdio.h>

#include "zmm_mul.h"

void combine_28bit(unsigned int* data, unsigned int* result, int digits){


	long rdi;
	long rsi;


    __asm__ volatile(
	// initialize
	"movq $0xfffffff, %%rax;"
	"movq %%rax, %%xmm1;"
	"vpslldq $4, %%xmm1, %%xmm2;"
	"vpslldq $8, %%xmm1, %%xmm3;"
	"vpslldq $12, %%xmm1, %%xmm4;"

	:::"%rax"
	);

	for(rdi=0, rsi=0; rdi<=digits; rdi+=2, rsi+=14){

		__asm__ volatile(
    		//"combine:"
			"movdqa (%0, %1, 8), %%xmm0;"
			// [0]
			"movdqa %%xmm0, %%xmm6;"
			"pand %%xmm1, %%xmm6;"
			// [1]
			"movdqa %%xmm0, %%xmm5;"
			"pand %%xmm2, %%xmm5;"
			"psrlq $4, %%xmm5;"
			"pxor %%xmm5, %%xmm6;"
			// [2]
			"movdqa %%xmm0, %%xmm5;"
			"pand %%xmm3, %%xmm5;"
			"psrldq $1, %%xmm5;"
			"pxor %%xmm5, %%xmm6;"
			// [3]
			"movdqa %%xmm0, %%xmm5;"
			"pand %%xmm4, %%xmm5;"
			"psrlq $12, %%xmm5;"
			"pxor %%xmm5, %%xmm6;"

			"movdqu %%xmm6, (%2, %3);"

			::"r"(data), "r"(rdi), "r"(result), "r"(rsi)
		);
	}

}

