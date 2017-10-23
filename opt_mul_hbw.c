#include<stdio.h>
#include<stdlib.h>

#include<hbwmalloc.h>

#include "zmm_mul.h"


void multiply_outer(unsigned int* a, unsigned int* b, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s);
long multiply_inner(unsigned int* a, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, long index_outer);
void calc_carry(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s);
void cloop(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s);

void combine_28bit(unsigned int* data, unsigned int* result, int digits);
void split_28bit(unsigned int* data, unsigned int* result, int digits);

//multiply(a, b, t, DIGITS, DIGITSTIMESTWO);

void hbw_multiply(unsigned int* data_a, unsigned int* data_b, unsigned int* t){

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
	unsigned int* a = hbw_calloc((4*M),sizeof(long));
	unsigned int* b = hbw_calloc((4*M),sizeof(long));
	unsigned int* t_tmp = hbw_calloc((4*M),sizeof(long));

	unsigned int* u = hbw_calloc((4*M),sizeof(long));
	unsigned int* v = hbw_calloc((4*M),sizeof(long));
	unsigned int* w = hbw_calloc((4*M),sizeof(long));

	unsigned int* p = hbw_calloc((4*M),sizeof(long));
	unsigned int* q = hbw_calloc((4*M),sizeof(long));
	unsigned int* r = hbw_calloc((4*M),sizeof(long));
	unsigned int* s = hbw_calloc((4*M),sizeof(long));

	if(!(a && b && t_tmp && u && v && w && p && q && r && s)){
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
	
	split_28bit(data_a, a, SPLITDIGITS);
	split_28bit(data_b, b, SPLITDIGITS);

	multiply_outer(a, b, t_tmp, u, v, w, p, q, r, s);

	//Finally, we do the 29bit carry calculation
	//and add the two result arrays

	calc_carry(t_tmp, u, v, w, p, q, r, s);
    
	combine_28bit(t_tmp, t, COMDIGITS);

	hbw_free(a);
	hbw_free(b);
	hbw_free(t_tmp);
	hbw_free(u);
	hbw_free(v);
	hbw_free(w);
	hbw_free(p);
	hbw_free(q);
	hbw_free(r);
	hbw_free(s);

	a = b = t_tmp = u = v = w = p = q = r = s = NULL;
}

