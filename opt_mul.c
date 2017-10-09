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

	unsigned int* u = calloc((4*M),sizeof(long));
	unsigned int* v = calloc((4*M),sizeof(long));
	unsigned int* w = calloc((4*M),sizeof(long));

	unsigned int* p = calloc((4*M),sizeof(long));
	unsigned int* q = calloc((4*M),sizeof(long));
	unsigned int* r = calloc((4*M),sizeof(long));
	unsigned int* s = calloc((4*M),sizeof(long));

	if(!(u && v && w && p && q && r && s)){
		puts("malloc error");
		exit(1);
	}

	long step1[8]; //, step2[8];
	long repeat1[8]; //, repeat2[8], repeat3[8], repeat4[8], repeat5[8];

	int i;
	for(i=0; i<8; i++)	step1[i] = i;
	//for(i=0; i<8; i++)	step2[i] = i+8;
	
	for(i=0; i<8; i++)	repeat1[i] = 1;
	/*
	for(i=0; i<8; i++)	repeat2[i] = 2;
	for(i=0; i<8; i++)	repeat3[i] = 4;
	for(i=0; i<8; i++)	repeat4[i] = 8;
	for(i=0; i<8; i++)	repeat5[i] = 13;
*/

	//new style
	// make zmm24 = 0706050403020100
	__asm__ volatile(
		"vmovdqu64 %0, %%zmm26;"
		::"m"(step1)
	);	

	// make zmm26 = 0101010101010101
	__asm__ volatile(
		"vmovdqu64 %0, %%zmm27;"
		::"m"(repeat1)
	);

	__asm__ volatile(

		"vpaddq %zmm27, %zmm27, %zmm28;" //0202020202020202
		"vpaddq %zmm28, %zmm28, %zmm29;" //0404040404040404
		"vpaddq %zmm29, %zmm29, %zmm30;" //0808080808080808
	);

	// make zmm24 = 0706050403020100 and,
	// make zmm25 = 0f0e0d0c0b0a0908
	// old style
	/*
	__asm__ volatile(
		"vmovdqu64 %0, %%zmm26;"
		"vmovdqu64 %1, %%zmmyy;"
		::"m"(step1), "m"(step2)
	);

	// make repeat number
	__asm__ volatile(
		"vmovdqu64 %0, %%zmm27;"
		"vmovdqu64 %1, %%zmm28;"
		"vmovdqu64 %2, %%zmm29;"
		"vmovdqu64 %3, %%zmm30;"
		"vmovdqu64 %4, %%zmmxx;"
		::"m"(repeat1), "m"(repeat2), "m"(repeat3), "m"(repeat4), "m"(repeat5)
	);
	*/
	
	

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

	u = v = w = p = q = r = s = NULL;
}

void multiply_outer(unsigned int* a, unsigned int* b, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s){


	long index_outer, index_inner, rdi;

	for(index_outer = 0; index_outer < DIGITS; index_outer += 16){

		__asm__ volatile(
			// load b[i] ... b[i+7]
			"vmovdqu32 (%0, %1, 4), %%zmm1;"
	
		
			// load t[8-15], u[8-15], u[0-7], t[0-7], respectively
			"vmovdqu64 64(%2, %1, 8), %%zmm10;" //latter t
			"vmovdqu64 64(%3, %1, 8), %%zmm11;" //latter u
			"vmovdqu64 64(%4, %1, 8), %%zmm12;" //latter v
			"vmovdqu64 64(%5, %1, 8), %%zmm13;" //latter w
			"vmovdqu64 64(%6, %1, 8), %%zmm14;" //latter p
			"vmovdqu64 64(%7, %1, 8), %%zmm15;" //latter q
			"vmovdqu64 64(%8, %1, 8), %%zmm16;" //latter r
			"vmovdqu64 64(%9, %1, 8), %%zmm17;" //latter s


			"vmovdqu64 (%2, %1, 8), %%zmm18;" //former t
			"vmovdqu64 (%3, %1, 8), %%zmm19;" //former u
			"vmovdqu64 (%4, %1, 8), %%zmm20;" //former v
			"vmovdqu64 (%5, %1, 8), %%zmm21;" //former w
			"vmovdqu64 (%6, %1, 8), %%zmm22;" //former p
			"vmovdqu64 (%7, %1, 8), %%zmm23;" //former q
			"vmovdqu64 (%8, %1, 8), %%zmm24;" //former r
			"vmovdqu64 (%9, %1, 8), %%zmm25;" //former s
		

			::"r"(b), "r"(index_outer), "r"(t), "r"(u), "r"(v), "r"(w), "r"(p), "r"(q), "r"(r), "r"(s)
		);


		index_inner = multiply_inner(a, t, u, v, w, p, q, r, s, index_outer);

		rdi = index_inner + index_outer;

		__asm__ volatile(
			"vmovdqu64 %%zmm18, 128(%1, %0, 8);"
			"vmovdqu64 %%zmm19, 128(%2, %0, 8);"	
			"vmovdqu64 %%zmm20, 128(%3, %0, 8);"
			"vmovdqu64 %%zmm21, 128(%4, %0, 8);"
			"vmovdqu64 %%zmm22, 128(%5, %0, 8);"
			"vmovdqu64 %%zmm23, 128(%6, %0, 8);"	
			"vmovdqu64 %%zmm24, 128(%7, %0, 8);"
			"vmovdqu64 %%zmm25, 128(%8, %0, 8);"
			::"r"(rdi), "r"(t), "r"(u), "r"(v), "r"(w), "r"(p), "r"(q), "r"(r), "r"(s)
		);
		
	}
}



long multiply_inner(unsigned int* a, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, long index_outer){


	long index_inner;
	for(index_inner = 0; index_inner < DIGITS; index_inner += 16){

		long index = index_inner + index_outer;

		// load a[j] ... a[j+15]
		__asm__ volatile(
			"vmovdqu32 (%0, %1, 4), %%zmm0;"
			::"r"(a), "r"(index_inner)
		);

	// zmm31 ... vperm index
	__asm__ volatile(

		"vpxorq %zmm31, %zmm31, %zmm31;"
		"vpermd %zmm1, %zmm31, %zmm3;" // 0000000000000000
		"vpaddq %zmm26, %zmm30, %zmm31;"
		"vpermd %zmm0, %zmm31, %zmm2;" // 0f0e0d0c0b0a0908
		
		"vpaddq %zmm27, %zmm28, %zmm31;"

		"vpermd %zmm1, %zmm27, %zmm4;" // 0101010101010101	
		"vpermd %zmm1, %zmm28, %zmm5;" // 0202020202020202
		"vpermd %zmm1, %zmm31, %zmm6;" // 0303030303030303
		
		"vpmuludq %zmm3, %zmm2, %zmm7;" //mul A
		"vpmuludq %zmm4, %zmm2, %zmm8;" //mul B
		"vpmuludq %zmm5, %zmm2, %zmm9;" //mul C	
		"vpmuludq %zmm6, %zmm2, %zmm2;" //mul D


		"vpaddq %zmm7, %zmm10, %zmm10;" //add A
		"vpaddq %zmm8, %zmm11, %zmm11;" //add B
		"vpaddq %zmm9, %zmm12, %zmm12;" //add C
		"vpaddq %zmm2, %zmm13, %zmm13;" //add D
	);

	__asm__ volatile(
		"vpermd %%zmm0, %%zmm26, %%zmm2;" // 0706050403020100
		"vpxorq %%zmm31, %%zmm31, %%zmm31;"
		"vpermd %%zmm1, %%zmm31, %%zmm3;" // 000000000000000

		"vpmuludq %%zmm6, %%zmm2, %%zmm7;" //mul E
		"vpmuludq %%zmm5, %%zmm2, %%zmm8;" //mul F
		"vpmuludq %%zmm4, %%zmm2, %%zmm9;" //mul G
		"vpmuludq %%zmm3, %%zmm2, %%zmm3;" //mul H

		"vpaddq %%zmm7, %%zmm21, %%zmm21;" //add E
		"vpaddq %%zmm8, %%zmm20, %%zmm20;" //add F
		"vpaddq %%zmm9, %%zmm19, %%zmm19;" //add G
		"vpaddq %%zmm3, %%zmm18, %%zmm18;" //add H

		"vmovdqu64 %%zmm21, (%1, %0, 8);"
		"vmovdqu64 %%zmm20, (%2, %0, 8);"
		"vmovdqu64 %%zmm19, (%3, %0, 8);"
		"vmovdqu64 %%zmm18, (%4, %0, 8);"
		
		"vmovdqu64 128(%1, %0, 8), %%zmm21;" //128 = sizeof(long)(i.e., 8) * 16
		"vmovdqu64 128(%2, %0, 8), %%zmm20;"
		"vmovdqu64 128(%3, %0, 8), %%zmm19;"
		"vmovdqu64 128(%4, %0, 8), %%zmm18;"

		::"r"(index), "r"(w), "r"(v), "r"(u), "r"(t)
	);

	__asm__ volatile(
		"vpermd %%zmm1, %%zmm30, %%zmm3;" // 0808080808080808
		"vpaddq %%zmm27, %%zmm30, %%zmm31;"
		
		"vpmuludq %%zmm3, %%zmm2, %%zmm7;" //mul I

		"vpermd %%zmm1, %%zmm31, %%zmm4;" // 0909090909090909
		"vpaddq %%zmm27, %%zmm31, %%zmm31;"
		
		"vpmuludq %%zmm4, %%zmm2, %%zmm8;" //mul J

		"vpermd %%zmm1, %%zmm31, %%zmm5;" // 0a0a0a0a0a0a0a0a
		"vpaddq %%zmm27, %%zmm31, %%zmm31;"
		
		"vpmuludq %%zmm5, %%zmm2, %%zmm9;" //mul K

		"vpermd %%zmm1, %%zmm31, %%zmm6;" // 0b0b0b0b0b0b0b0b

		"vpmuludq %%zmm6, %%zmm2, %%zmm2;" //mul L
		
		"vpaddq %%zmm7, %%zmm10, %%zmm10;" //add I
		"vpaddq %%zmm8, %%zmm11, %%zmm11;" //add J
		"vpaddq %%zmm9, %%zmm12, %%zmm12;" //add K
		"vpaddq %%zmm2, %%zmm13, %%zmm13;" //add L

		"vmovdqu64 %%zmm10, 64(%1, %0, 8);"
		"vmovdqu64 %%zmm11, 64(%2, %0, 8);"
		"vmovdqu64 %%zmm12, 64(%3, %0, 8);"
		"vmovdqu64 %%zmm13, 64(%4, %0, 8);"

		"vmovdqu64 192(%1, %0, 8), %%zmm10;"
		"vmovdqu64 192(%2, %0, 8), %%zmm11;"
		"vmovdqu64 192(%3, %0, 8), %%zmm12;"
		"vmovdqu64 192(%4, %0, 8), %%zmm13;"

		::"r"(index), "r"(t), "r"(u), "r"(v), "r"(w)
	);


	__asm__ volatile(	
		"vpaddq %zmm26, %zmm30, %zmm31;"
		"vpermd %zmm0, %zmm31, %zmm2;" // 0f0e0d0c0b0a0908

		"vpmuludq %zmm6, %zmm2, %zmm7;" //mul M
		"vpmuludq %zmm5, %zmm2, %zmm8;" //mul N
		"vpmuludq %zmm4, %zmm2, %zmm9;" //mul O
		"vpmuludq %zmm3, %zmm2, %zmm3;" //mul P

		"vpaddq %zmm7, %zmm21, %zmm21;" //add M
		"vpaddq %zmm8, %zmm20, %zmm20;" //add N
		"vpaddq %zmm9, %zmm19, %zmm19;" //add O
		"vpaddq %zmm3, %zmm18, %zmm18;" //add P
	);

	__asm__ volatile(
		"vpermd %zmm1, %zmm29, %zmm3;" //0404040404040404
		"vpaddq %zmm29, %zmm27, %zmm31;"

		"vpmuludq %zmm3, %zmm2, %zmm7;" //mul a

		"vpermd %zmm1, %zmm31, %zmm4;" //0505050505050505
		"vpaddq %zmm27, %zmm31, %zmm31;"

		"vpmuludq %zmm4, %zmm2, %zmm8;" //mul b

		"vpermd %zmm1, %zmm31, %zmm5;" //0606060606060606
		"vpaddq %zmm27, %zmm31, %zmm31;"

		"vpmuludq %zmm5, %zmm2, %zmm9;" //mul c

		"vpermd %zmm1, %zmm31, %zmm6;" //0707070707070707


		"vpaddq %zmm7, %zmm14, %zmm14;" //add a
		"vpmuludq %zmm6, %zmm2, %zmm2;" //mul d
		"vpaddq %zmm8, %zmm15, %zmm15;" //add b
		"vpaddq %zmm9, %zmm16, %zmm16;" //add c
		"vpaddq %zmm2, %zmm17, %zmm17;" //add d
	);
	
	__asm__ volatile(
		"vpermd %%zmm0, %%zmm26, %%zmm2;" //0706050403020100

		"vpmuludq %%zmm6, %%zmm2, %%zmm7;" //mul e
		"vpmuludq %%zmm5, %%zmm2, %%zmm8;" //mul f
		"vpmuludq %%zmm4, %%zmm2, %%zmm9;" //mul g
		"vpmuludq %%zmm2, %%zmm3, %%zmm3;" //mul h

		"vpaddq %%zmm7, %%zmm25, %%zmm25;" //add e
		"vpaddq %%zmm8, %%zmm24, %%zmm24;" //add f
		"vpaddq %%zmm9, %%zmm23, %%zmm23;" //add g
		"vpaddq %%zmm3, %%zmm22, %%zmm22;" //add h

		"vmovdqu64 %%zmm25, (%1, %0, 8);"
		"vmovdqu64 %%zmm24, (%2, %0, 8);"
		"vmovdqu64 %%zmm23, (%3, %0, 8);"
		"vmovdqu64 %%zmm22, (%4, %0, 8);"		
		
		"vmovdqu64 128(%1, %0, 8), %%zmm25;" //128 = sizeof(long)(i.e., 8) * 16
		"vmovdqu64 128(%2, %0, 8), %%zmm24;"
		"vmovdqu64 128(%3, %0, 8), %%zmm23;"
		"vmovdqu64 128(%4, %0, 8), %%zmm22;"

		::"r"(index), "r"(s), "r"(r), "r"(q), "r"(p)
	);

	__asm__ volatile(
		"vpaddq %%zmm30, %%zmm29, %%zmm31;"
		"vpermd %%zmm1, %%zmm31, %%zmm3;" // 0c0c0c0c0c0c0c0c
		"vpaddq %%zmm31, %%zmm27, %%zmm31;"
		
		"vpmuludq %%zmm3, %%zmm2, %%zmm7;" //mul i
		
		"vpermd %%zmm1, %%zmm31, %%zmm4;" // 0d0d0d0d0d0d0d0d
		"vpaddq %%zmm27, %%zmm31, %%zmm31;"
		
		"vpmuludq %%zmm4, %%zmm2, %%zmm8;" //mul j
		
		"vpermd %%zmm1, %%zmm31, %%zmm5;" // 0e0e0e0e0e0e0e0e
		"vpaddq %%zmm27, %%zmm31, %%zmm31;"
		
		"vpmuludq %%zmm5, %%zmm2, %%zmm9;" //mul k
		
		"vpermd %%zmm1, %%zmm31, %%zmm6;" // 0f0f0f0f0f0f0f0f

		"vpmuludq %%zmm6, %%zmm2, %%zmm2;" //mul l

		"vpaddq %%zmm7, %%zmm14, %%zmm14;" //add i
		"vpaddq %%zmm8, %%zmm15, %%zmm15;" //add j
		"vpaddq %%zmm9, %%zmm16, %%zmm16;" //add k
		"vpaddq %%zmm2, %%zmm17, %%zmm17;" //add l

		"vmovdqu64 %%zmm14, 64(%1, %0, 8);"
		"vmovdqu64 %%zmm15, 64(%2, %0, 8);"
		"vmovdqu64 %%zmm16, 64(%3, %0, 8);"
		"vmovdqu64 %%zmm17, 64(%4, %0, 8);"

		"vmovdqu64 192(%1, %0, 8), %%zmm14;"
		"vmovdqu64 192(%2, %0, 8), %%zmm15;"
		"vmovdqu64 192(%3, %0, 8), %%zmm16;"
		"vmovdqu64 192(%4, %0, 8), %%zmm17;"

		::"r"(index), "r"(p), "r"(q), "r"(r), "r"(s)
	);

	__asm__ volatile(	
		"vpaddq %zmm26, %zmm30, %zmm31;"
		"vpermd %zmm0, %zmm31, %zmm2;" // 0f0e0d0c0b0a0908
		
		"vpmuludq %zmm6, %zmm2, %zmm7;" //mul m
		"vpmuludq %zmm5, %zmm2, %zmm8;" //mul n
		"vpmuludq %zmm4, %zmm2, %zmm9;" //mul o
		"vpmuludq %zmm3, %zmm2, %zmm3;" //mul p

		"vpaddq %zmm7, %zmm25, %zmm25;" //add m
		"vpaddq %zmm8, %zmm24, %zmm24;" //add n
		"vpaddq %zmm9, %zmm23, %zmm23;" //add o
		"vpaddq %zmm3, %zmm22, %zmm22;" //add p
	);
	}

	// subtract extra 16 which is added in "for" expression. 
	return index_inner - 16;
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
		"psrlq $28, %%mm6;"
		
		"mov $1, %%rbx;"
		//[1]
		"movq (%0, %%rbx, 8), %%mm0;"
		"movq -8(%1, %%rbx, 8), %%mm1;"
		"paddq %%mm1, %%mm0;"
		"paddq %%mm6, %%mm0;"
		"movq %%mm0, %%mm6;"
		"pand %%mm7, %%mm6;"
		"movd %%mm6, (%0, %%rbx, 4);"
		"psrlq $28, %%mm0;"
	
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
		"psrlq $28, %%mm6;"

		"inc %%ebx;"
		//[3]
		"movq (%0, %%rbx, 8), %%mm0;"
		"movq -8(%1, %%rbx, 8), %%mm1;"
		"movq -16(%2, %%rbx, 8), %%mm2;"
		"movq -24(%3, %%rbx, 8), %%mm3;"
		"paddq %%mm6, %%mm0;"
		"paddq %%mm1, %%mm0;"
		"paddq %%mm2, %%mm0;"
		"paddq %%mm3, %%mm0;"
		"movq %%mm0, %%mm6;"
		"pand %%mm7, %%mm6;"
		"movd %%mm6, (%0, %%rbx, 4);"
		"psrlq $28, %%mm0;"

		"inc %%ebx;"
		//[4]
		"movq (%0, %%rbx, 8), %%mm6;"
		"movq -8(%1, %%rbx, 8), %%mm1;"
		"movq -16(%2, %%rbx, 8), %%mm2;"
		"movq -24(%3, %%rbx, 8), %%mm3;"
		"movq -32(%4, %%rbx, 8), %%mm4;"
		"paddq %%mm0, %%mm6;"
		"paddq %%mm1, %%mm6;"
		"paddq %%mm2, %%mm6;"
		"paddq %%mm3, %%mm6;"
		"paddq %%mm4, %%mm6;"
		"movq %%mm6, %%mm0;"
		"pand %%mm7, %%mm0;"
		"movd %%mm0, (%0, %%rbx, 4);"
		"psrlq $28, %%mm6;"

		"inc %%ebx;"
		//[5]
		"movq (%0, %%rbx, 8), %%mm0;"
		"movq -8(%1, %%rbx, 8), %%mm1;"
		"movq -16(%2, %%rbx, 8), %%mm2;"
		"movq -24(%3, %%rbx, 8), %%mm3;"
		"movq -32(%4, %%rbx, 8), %%mm4;"
		"movq -40(%5, %%rbx, 8), %%mm5;"
		"paddq %%mm6, %%mm0;"
		"paddq %%mm1, %%mm0;"
		"paddq %%mm2, %%mm0;"
		"paddq %%mm3, %%mm0;"
		"paddq %%mm4, %%mm0;"
		"paddq %%mm5, %%mm0;"
		"movq %%mm0, %%mm6;"
		"pand %%mm7, %%mm6;"
		"movd %%mm6, (%0, %%rbx, 4);"
		"psrlq $28, %%mm0;"

		"inc %%ebx;"
		//[6]
		"movq (%0, %%rbx, 8), %%mm6;"
		"movq -8(%1, %%rbx, 8), %%mm1;"
		"movq -16(%2, %%rbx, 8), %%mm2;"
		"movq -24(%3, %%rbx, 8), %%mm3;"
		"movq -32(%4, %%rbx, 8), %%mm4;"
		"movq -40(%5, %%rbx, 8), %%mm5;"
		"paddq %%mm0, %%mm6;"
		"paddq %%mm1, %%mm6;"
		"movq -48(%6, %%rbx, 8), %%mm1;" //r
		"paddq %%mm2, %%mm6;"
		"paddq %%mm3, %%mm6;"
		"paddq %%mm4, %%mm6;"
		"paddq %%mm5, %%mm6;"
		"paddq %%mm1, %%mm6;" //r
		"movq %%mm6, %%mm0;"
		"pand %%mm7, %%mm0;"
		"movd %%mm0, (%0, %%rbx, 4);"
		"psrlq $28, %%mm6;"

		::"r"(t), "r"(u), "r"(v), "r"(w), "r"(p), "r"(q), "r"(r)
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
	for(i=7; i<=DIGITSTIMESTWO; i+=2){

		__asm__ volatile(
			"movq (%1, %0, 8), %%mm0;"
			"movq -8(%2, %0, 8), %%mm1;"
			"movq -16(%3, %0, 8), %%mm2;"
			"movq -24(%4, %0, 8), %%mm3;"
			"movq -32(%5, %0, 8), %%mm4;"
			"movq -40(%6, %0, 8), %%mm5;"
			"paddq %%mm1, %%mm0;"
			"movq -48(%7, %0, 8), %%mm1;" //r
			"paddq %%mm2, %%mm0;"
			"movq -56(%8, %0, 8), %%mm2;" //s
			"paddq %%mm3, %%mm0;"
			"paddq %%mm4, %%mm0;"
			"paddq %%mm5, %%mm0;"
			"paddq %%mm1, %%mm0;" //r
			"paddq %%mm2, %%mm0;" //s
			"paddq %%mm6, %%mm0;"
			"movq %%mm0, %%mm6;"
			"pand %%mm7, %%mm6;"
			"movd %%mm6, (%1, %0, 4);"
			"psrlq $28, %%mm0;"


			"movq 8(%1, %0, 8), %%mm6;"
			"movq (%2, %0, 8), %%mm1;"
			"movq -8(%3, %0, 8), %%mm2;"
			"movq -16(%4, %0, 8), %%mm3;"
			"movq -24(%5, %0, 8), %%mm4;"
			"movq -32(%6, %0, 8), %%mm5;"
			"paddq %%mm1, %%mm6;"
			"movq -40(%7, %0, 8), %%mm1;" //r
			"paddq %%mm2, %%mm6;"
			"movq -48(%8, %0, 8), %%mm2;" //s
			"paddq %%mm3, %%mm6;"
			"paddq %%mm4, %%mm6;"
			"paddq %%mm5, %%mm6;"
			"paddq %%mm1, %%mm6;" //r
			"paddq %%mm2, %%mm6;" //s
			"paddq %%mm0, %%mm6;"
			"movq %%mm6, %%mm0;"
			"pand %%mm7, %%mm0;"
			"movd %%mm0, 4(%1, %0, 4);"
			"psrlq $28, %%mm6;"

			::"r"(i), "r"(t), "r"(u), "r"(v), "r"(w), "r"(p), "r"(q), "r"(r), "r"(s)
		);
	}
}
