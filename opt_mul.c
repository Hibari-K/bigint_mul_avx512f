#include<stdio.h>
#include<stdlib.h>

#include "zmm_mul.h"


void multiply_outer(unsigned int* a, unsigned int* b, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s);
long multiply_inner(unsigned int* a, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, long index_outer);
void calc_carry(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s);
void cloop(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s);


//multiply(a, b, t, DIGITS, DIGITSTIMESTWO);

void multiply(unsigned int* a, unsigned int* b, unsigned int* t){

   /*
	 It is not desirable to use scalar operation,
	 since it can introduce bottleneck in using not only SSE but AVX-512.

	  Algorithm ( Concept )

	                             0x f e d c b a 9 8 7 6 5 4 3 2 1 0 ... (i)
	  *                          0x f'e'd'c'b'a'9'8'7'6'5'4'3'2'1'0'... (ii)
	  -------------------------------------------------------------
	                                A A A A A A A A H H H H H H H H     
	                              B B B B B B B B G G G G G G G G       
	                            C C C C C C C C F F F F F F F F        
	                          E E E E E E E E D D D D D D D D          
	                        a a a a a a a a h h h h h h h h
	                      b b b b b b b b g g g g g g g g
	                    c c c c c c c c f f f f f f f f
	                  d d d d d d d d e e e e e e e e
                    P P P P P P P P I I I I I I I I
		          O O O O O O O O J J J J J J J J
	            N N N N N N N N K K K K K K K K
	          M M M M M M M M L L L L L L L L 
		    p p p p p p p p i i i i i i i i
		  o o o o o o o o j j j j j j j j
	    n n n n n n n n k k k k k k k k
	  m m m m m m m m l l l l l l l l
	  ------------------------------------------------------------


	  Capital alphabet commented out in following code (e.g., //A)
	  corresponds to the alphabet shown in above figure.
	  Steplike number (e.g., 03020100) and repeated number (e.g., 01010101) 
	  indicates multiplicand and multiplier, respectively;
	  that is, the former is (i) and the latter is (ii).
	  
	 */

	unsigned int* u = calloc((2*M+8),sizeof(long));
	unsigned int* v = calloc((2*M+8),sizeof(long));
	unsigned int* w = calloc((2*M+8),sizeof(long));

	unsigned int* p = calloc((2*M+8),sizeof(long));
	unsigned int* q = calloc((2*M+8),sizeof(long));
	unsigned int* r = calloc((2*M+8),sizeof(long));
	unsigned int* s = calloc((2*M+8),sizeof(long));

	if(!(u && v && w && p && q && r && s)){
		puts("malloc error");
		exit(1);
	}

	long step1[8], step2[8];
	long repeat1[8], repeat2[8], repeat3[8], repeat4[8], repeat5[8];

	int i;
	for(i=0; i<8; i++)	step1[i] = i;
	for(i=0; i<8; i++)	step2[i] = i+8;
	
	for(i=0; i<8; i++)	repeat1[i] = 1;
	for(i=0; i<8; i++)	repeat2[i] = 2;
	for(i=0; i<8; i++)	repeat3[i] = 4;
	for(i=0; i<8; i++)	repeat4[i] = 8;
	for(i=0; i<8; i++)	repeat5[i] = 13;

	// make zmm20 = 0706050403020100 and,
	// make zmm21 = 0f0e0d0c0b0a0908
	__asm__ volatile(
		"vmovdqu64 %0, %%zmm20;"
		"vmovdqu64 %1, %%zmm21;"
		::"m"(step1), "m"(step2)
	);

	// make repeat number
	__asm__ volatile(
		"vmovdqu64 %0, %%zmm22;"
		"vmovdqu64 %1, %%zmm23;"
		"vmovdqu64 %2, %%zmm24;"
		"vmovdqu64 %3, %%zmm25;"
		"vmovdqu64 %4, %%zmm26;"
		::"m"(repeat1), "m"(repeat2), "m"(repeat3), "m"(repeat4), "m"(repeat5)
	);

	
	multiply_outer(a, b, t, u, v, w, p, q, r, s);

	//Finally, we do the 29bit carry calculation
	//and add the two result arrays

	calc_carry(t, u, v, w, p, q, r, s);
    

	free(u);
	free(v);
	free(w);
	free(p);
	free(q);
	free(r);
	free(s);
}

void multiply_outer(unsigned int* a, unsigned int* b, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s){


	long index_outer, index_inner, rdi;

	for(index_outer = 0; index_outer < DIGITS; index_outer += 16){

		__asm__ volatile(
			// load b[i] ... b[i+7]
			"vmovdqu32 (%0, %1, 4), %%zmm1;"
	
			// load t[8-15], u[8-15], u[0-7], t[0-7], respectively
			"vmovdqu64 64(%2, %1, 8), %%zmm4;" //latter t
			"vmovdqu64 64(%3, %1, 8), %%zmm5;" //latter u
			"vmovdqu64 64(%4, %1, 8), %%zmm6;" //latter v
			"vmovdqu64 64(%5, %1, 8), %%zmm7;" //latter w
			"vmovdqu64 64(%6, %1, 8), %%zmm8;" //latter p
			"vmovdqu64 64(%7, %1, 8), %%zmm9;" //latter q
			"vmovdqu64 64(%8, %1, 8), %%zmm10;" //latter r
			"vmovdqu64 64(%9, %1, 8), %%zmm11;" //latter s

			"vmovdqu64 (%2, %1, 8), %%zmm12;" //former t
			"vmovdqu64 (%3, %1, 8), %%zmm13;" //former u
			"vmovdqu64 (%4, %1, 8), %%zmm14;" //former v
			"vmovdqu64 (%5, %1, 8), %%zmm15;" //former w
			"vmovdqu64 (%6, %1, 8), %%zmm16;" //former p
			"vmovdqu64 (%7, %1, 8), %%zmm17;" //former q
			"vmovdqu64 (%8, %1, 8), %%zmm18;" //former r
			"vmovdqu64 (%9, %1, 8), %%zmm19;" //former s

			::"r"(b), "r"(index_outer), "r"(t), "r"(u), "r"(v), "r"(w), "r"(p), "r"(q), "r"(r), "r"(s)
		);


		index_inner = multiply_inner(a, t, u, v, w, p, q, r, s, index_outer);

		rdi = index_inner + index_outer;

		__asm__ volatile(
			"vmovdqu64 %%zmm12, 128(%1, %0, 8);"
			"vmovdqu64 %%zmm13, 128(%2, %0, 8);"	
			"vmovdqu64 %%zmm14, 128(%3, %0, 8);"
			"vmovdqu64 %%zmm15, 128(%4, %0, 8);"
			"vmovdqu64 %%zmm16, 128(%5, %0, 8);"
			"vmovdqu64 %%zmm17, 128(%6, %0, 8);"	
			"vmovdqu64 %%zmm18, 128(%7, %0, 8);"
			"vmovdqu64 %%zmm19, 128(%8, %0, 8);"
			::"r"(rdi), "r"(t), "r"(u), "r"(v), "r"(w), "r"(p), "r"(q), "r"(r), "r"(s)
		);
	}
}



long multiply_inner(unsigned int* a, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, long index_outer){


	long index_inner;
	for(index_inner = 0; index_inner < DIGITS; index_inner += 8){

	long index = index_inner + index_outer;

	__asm__ volatile(
	// load a[j] ... a[j+7]
	"vmovdqu (%0, %5, 4), %%ymm0;"

	// calculate multiply
	"vpxor %%ymm15, %%ymm15, %%ymm15;"
	"vpermd %%ymm1, %%ymm15, %%ymm3;" //00000000
	"vpermd %%ymm0, %%ymm12, %%ymm2;" //07060504
	"vpmuludq %%ymm3, %%ymm2, %%ymm3;" //A
	"vpaddq %%ymm3, %%ymm7, %%ymm7;"
	
	"vpermd %%ymm1, %%ymm14, %%ymm3;" //01010101
	"vpmuludq %%ymm3, %%ymm2, %%ymm2;" //B
	"vpaddq %%ymm2, %%ymm6, %%ymm6;"

    "vpsubq %%ymm13, %%ymm12, %%ymm12;" // ymm12(07060504) - ymm13(04040404)
	"vpermd %%ymm0, %%ymm12, %%ymm2;" //03020100
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //C
	"vpaddq %%ymm3, %%ymm10, %%ymm10;"

	"vmovdqu %%ymm10, (%2, %6, 8);"
	"vmovdqu 64(%2, %6, 8), %%ymm10;"

	"vpermd %%ymm1, %%ymm15, %%ymm3;" //00000000
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //D
	"vpaddq %%ymm3, %%ymm11, %%ymm11;"

	"vmovdqu %%ymm11, (%1, %6, 8);"
	"vmovdqu 64(%1, %6, 8), %%ymm11;"

	"vpermd %%ymm1, %%ymm13, %%ymm3;" //04040404
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //E
	"vpaddq %%ymm3, %%ymm7, %%ymm7;"

	"vmovdqu %%ymm7, 32(%1, %6, 8);"
	"vmovdqu 96(%1, %6, 8), %%ymm7;"

    "vpaddq %%ymm13, %%ymm14, %%ymm15;"
	"vpermd %%ymm1, %%ymm15, %%ymm3;" //05050505
	"vpmuludq %%ymm3, %%ymm2, %%ymm2;" //F
	"vpaddq %%ymm2, %%ymm6, %%ymm6;"

	"vmovdqu %%ymm6, 32(%2, %6, 8);"
	"vmovdqu 96(%2, %6, 8), %%ymm6;"

    "vpaddq %%ymm13, %%ymm12, %%ymm12;"
	"vpermd %%ymm0, %%ymm12, %%ymm2;" //07060504
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //G
	"vpaddq %%ymm3, %%ymm10, %%ymm10;"

	"vpermd %%ymm1, %%ymm13, %%ymm3;" //04040404
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //H
	"vpaddq %%ymm3, %%ymm11, %%ymm11;"

    "vpaddq %%ymm14, %%ymm14, %%ymm15;"
	"vpermd %%ymm1, %%ymm15, %%ymm3;" //02020202
	"vpmuludq %%ymm3, %%ymm2, %%ymm3;" //I
	"vpaddq %%ymm3, %%ymm4, %%ymm4;"

    "vpaddq %%ymm14, %%ymm15, %%ymm15;"
	"vpermd %%ymm1, %%ymm15, %%ymm3;" //03030303
	"vpmuludq %%ymm3, %%ymm2, %%ymm2;" //J
	"vpaddq %%ymm2, %%ymm5, %%ymm5;"

    "vpsubq %%ymm13, %%ymm12, %%ymm12;"
	"vpermd %%ymm0, %%ymm12, %%ymm2;" //03020100
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //K
	"vpaddq %%ymm3, %%ymm9, %%ymm9;"

	"vmovdqu %%ymm9, (%4, %6, 8);"
	"vmovdqu 64(%4, %6, 8), %%ymm9;"

    "vpsubq %%ymm14, %%ymm15, %%ymm15;"
	"vpermd %%ymm1, %%ymm15, %%ymm3;" //02020202
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //L
	"vpaddq %%ymm3, %%ymm8, %%ymm8;"

	"vmovdqu %%ymm8, (%3, %6, 8);"
	"vmovdqu 64(%3, %6, 8), %%ymm8;"

    "vpaddq %%ymm13, %%ymm15, %%ymm15;"
    "vpermd %%ymm1, %%ymm15, %%ymm3;" //06060606
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //O
	"vpaddq %%ymm3, %%ymm4, %%ymm4;"

	"vmovdqu %%ymm4, 32(%3, %6, 8);"
	"vmovdqu 96(%3, %6, 8), %%ymm4;"
    
	"vpaddq %%ymm15, %%ymm14, %%ymm15;"
    "vpermd %%ymm1, %%ymm15, %%ymm3;" //07070707
	"vpmuludq %%ymm3, %%ymm2, %%ymm2;" //P
	"vpaddq %%ymm2, %%ymm5, %%ymm5;"

	"vmovdqu %%ymm5, 32(%4, %6, 8);"
	"vmovdqu 96(%4, %6, 8), %%ymm5;"

    "vpaddq %%ymm13, %%ymm12, %%ymm12;"
	"vpermd %%ymm0, %%ymm12, %%ymm2;" //07060504
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //Q
	"vpaddq %%ymm3, %%ymm9, %%ymm9;"
    
	"vpsubq %%ymm14, %%ymm15, %%ymm15;"
    "vpermd %%ymm1, %%ymm15, %%ymm3;" //06060606
	"vpmuludq %%ymm2, %%ymm3, %%ymm3;" //R
	"vpaddq %%ymm3, %%ymm8, %%ymm8;"

	::"r"(a), "r"(t), "r"(u), "r"(v), "r"(w), "r"(index_inner), "r"(index)
	);
	}

	// subtract extra 8 which is added in "for" expression. 
	return index_inner - 8;
}


void calc_carry(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s){


/*
	  in this routine,
	  mm7 : 0x1fffffff
	  mm6 : carry
	  mm0 : t
	  mm1 : u
	  mm2 : v
	  mm3 : w
	 */

	 __asm__ volatile(
		"pxor %%mm6, %%mm6;"
		"mov $0x1fffffff, %%eax;"
		"movd %%eax, %%mm7;"
		//[0]
		"movq (%0), %%mm0;"
		"movq %%mm0, %%mm6;"
		"pand %%mm7, %%mm0;"
		"movd %%mm0, (%0);"
		"psrlq $29, %%mm6;"
		
		"mov $1, %%rbx;"
		//[1]
		"movq (%0, %%rbx, 8), %%mm0;"
		"movq -8(%1, %%rbx, 8), %%mm1;"
		"paddq %%mm1, %%mm0;"
		"paddq %%mm6, %%mm0;"
		"movq %%mm0, %%mm6;"
		"pand %%mm7, %%mm6;"
		"movd %%mm6, (%0, %%rbx, 4);"
		"psrlq $29, %%mm0;"
	
		"inc %%ebx;"
		//[2]
		"movq (%0, %%rbx, 8), %%mm6;"
		"movq -8(%1, %%rbx, 8), %%mm1;"
		"movq -16(%2, %%rbx, 8), %%mm2;"
		"paddq %%mm1, %%mm6;"
		"paddq %%mm2, %%mm6;"
		"paddq %%mm0, %%mm6;"
		"movq %%mm6, %%mm0;"
		"pand %%mm7, %%mm0;"
		"movd %%mm0, (%0, %%rbx, 4);"
		"psrlq $29, %%mm6;"

		::"r"(t), "r"(u), "r"(v)
		:"%rax", "%rbx"
	);

	
	cloop(t, u, v, w, p, q, r, s);
	

	long rbx = DIGITSTIMESTWO - 1;

	__asm__ volatile(
		"movd %%mm6, (%0, %1, 4);"
		::"r"(t), "r"(rbx)
	);
}



void cloop(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s){

	long i;
	for(i=3; i<=DIGITSTIMESTWO; i+=2){

		__asm__ volatile(
			"movq (%0, %4, 8), %%mm0;"
			"movq -8(%1, %4, 8), %%mm1;"
			"movq -16(%2, %4, 8), %%mm2;"
			"movq -24(%3, %4, 8), %%mm3;"
			"paddq %%mm1, %%mm0;"
			"paddq %%mm2, %%mm0;"
			"paddq %%mm3, %%mm0;"
			"paddq %%mm6, %%mm0;"
			"movq %%mm0, %%mm6;"
			"pand %%mm7, %%mm6;"
			"movd %%mm6, (%0, %4, 4);"
			"psrlq $29, %%mm0;"


			"movq 8(%0, %4, 8), %%mm6;"
			"movq (%1, %4, 8), %%mm1;"
			"movq -8(%2, %4, 8), %%mm2;"
			"movq -16(%3, %4, 8), %%mm3;"
			"paddq %%mm1, %%mm6;"
			"paddq %%mm2, %%mm6;"
			"paddq %%mm3, %%mm6;"
			"paddq %%mm0, %%mm6;"
			"movq %%mm6, %%mm0;"
			"pand %%mm7, %%mm0;"
			"movd %%mm0, 4(%0, %4, 4);"
			"psrlq $29, %%mm6;"

			::"r"(t), "r"(u), "r"(v), "r"(w), "r"(i)
		);
	}
}
