#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>

#include<hbwmalloc.h>

#include<immintrin.h>

#include "zmm_mul.h"



void multiply_outer(unsigned int* a, unsigned int* b, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s);
void calc_carry(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, int digitsTimesTwo);
void calc_carry_29bit(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, int digitsTimesTwo);
void cloop(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, int digitsTimesTwo);

void combine_28bit(unsigned int* data, unsigned int* result, int digits);
void split_28bit(unsigned int* data, unsigned int* result, int digits);

void combine_29bit(unsigned int* data, unsigned int* result, int digits);
void split_29bit(unsigned int* data, unsigned int* result, int digits);

void ScalarMultiply(unsigned int* A, unsigned int* B, unsigned int* T, int loopa, int loopb);


extern double total_k;
extern double total_s;
extern double total_c;
extern double total_a;
extern double total_o;



//void multiply(unsigned int* data_a, unsigned int* data_b, unsigned int* t){
void multiply(zmm_t data_a, zmm_t data_b, zmm_t t){


	

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
	                          D D D D D D D D E E E E E E E E          
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


	  Alphabet commented out in following code (e.g., //A)
	  corresponds to the alphabet shown in above figure.
	  Steplike number (e.g., 03020100) and repeated number (e.g., 01010101) 
	  indicates multiplicand and multiplier, respectively;
	  that is, the former is (i) and the latter is (ii).
	  
	 */


	

/*
	static unsigned int a[500];
	static unsigned int b[500];
	static unsigned int t_tmp[1000];
	static unsigned int u[1000];
	static unsigned int v[1000];
	static unsigned int w[1000];
	static unsigned int p[1000];
	static unsigned int q[1000];
	static unsigned int r[1000];
	static unsigned int s[1000];
*/

	struct timeval sk, ss, sc, sa, so;
	struct timeval ek, es, ec, ea, eo;
	

	gettimeofday(&so, NULL);

	int a_array_len, b_array_len; 
/*
	unsigned long* tmp_a = (unsigned long*)data_a;
	unsigned long* tmp_b = (unsigned long*)data_b;
	
	int flag = 0;
	for(a_array_len=112; !flag; a_array_len-=2){

		flag  = (tmp_a[a_array_len] | tmp_b[a_array_len]);
		flag |= (tmp_a[a_array_len-1] | tmp_b[a_array_len-1]);	
	}

	a_array_len+=2;
	
	for(b_array_len=a_array_len; tmp_b[b_array_len]==0; b_array_len--);
	for(; tmp_a[a_array_len]==0; a_array_len--);
	a_array_len = (a_array_len+1)<<1;
	b_array_len = (b_array_len+1)<<1;
*/	


	int a_bitsize, b_bitsize, t_bitsize;
	int a_digits, b_digits, t_digits;
	int digitsTimesTwo;

	if(SIZ(data_a) == SIZ(data_b)){
		
		a_bitsize = b_bitsize = t_bitsize = SIZ(data_a) * 32;
		
		if(SIZ(data_a) > 224){
			puts("Error : The number of digits is too large.");
			exit(1);
		}

		a_digits = b_digits = t_digits = (a_bitsize-1)/28 + 1;
		
		/*
		if(t_bitsize >= 1792 || t_bitsize <= 512){
			a_digits = b_digits = t_digits = (a_bitsize-1)/28 + 1;
		}
		else{
			a_digits = b_digits = t_digits = (a_bitsize-1)/29 + 1;
		}	
		*/

		digitsTimesTwo = t_digits*2+1;
		
	}
	else{

		a_bitsize = (32 * SIZ(data_a));
		b_bitsize = (32 * SIZ(data_b));

		t_bitsize = (a_bitsize > b_bitsize) ? a_bitsize : b_bitsize;

		if(t_bitsize > 224 * 32){
			puts("Error : The number of digits is too large.");
			exit(1);
		}

	
		if(t_bitsize >= 1792 || t_bitsize <= 512){
			a_digits = (a_bitsize-1)/28 + 1;
			b_digits = (b_bitsize-1)/28 + 1;
		}
		else{
			a_digits = (a_bitsize-1)/29 + 1;
			b_digits = (b_bitsize-1)/29 + 1;
		}	

		t_digits = (a_digits > b_digits) ? a_digits : b_digits;
		digitsTimesTwo = t_digits*2+1;
	}


	gettimeofday(&eo, NULL);
	total_o += (eo.tv_sec - so.tv_sec) + (eo.tv_usec - so.tv_usec)*1.0E-6;
	
	gettimeofday(&sa, NULL);

	unsigned int *a, *b, *t_tmp, *u, *v, *w, *p, *q, *r, *s;

	if(t_bitsize <= 179 * 32){

	a = hbw_calloc((2*t_digits*10),sizeof(long));
	b = a+(4*t_digits);
	t_tmp = a+(8*t_digits);
	u = a+(12*t_digits);
	v = a+(16*t_digits);
	w = a+(20*t_digits);
	p = a+(24*t_digits);
	q = a+(28*t_digits);
	r = a+(32*t_digits);
	s = a+(36*t_digits);
	}
	else{

	a = hbw_calloc((a_digits),sizeof(long));
	b = hbw_calloc((b_digits),sizeof(long));
	t_tmp = hbw_calloc((2*t_digits),sizeof(long));

	u = hbw_calloc((2*t_digits),sizeof(long));
	v = hbw_calloc((2*t_digits),sizeof(long));
	w = hbw_calloc((2*t_digits),sizeof(long));

	p = hbw_calloc((2*t_digits),sizeof(long));
	q = hbw_calloc((2*t_digits),sizeof(long));
	r = hbw_calloc((2*t_digits),sizeof(long));
	s = hbw_calloc((2*t_digits),sizeof(long));

	}
	
	
	if(!(a && b && t_tmp && u && v && w && p && q && r && s)){
		puts("malloc error");
		exit(1);
	}
	
	gettimeofday(&ea, NULL);
	total_a += (ea.tv_sec - sa.tv_sec) + (ea.tv_usec - sa.tv_usec)*1.0E-6;

	
	
	int i,j;

	int a_split, b_split, t_comb;

	// scalar multiply few digits
	// 58 = 1856 / 32
	// 1856 = 29 * 64

		a_split = a_bitsize / 56;
		b_split = b_bitsize / 56;
		t_comb = 2 * (t_bitsize / 56) + 2;

		gettimeofday(&ss, NULL);
		
		split_28bit(PTR(data_a), a, a_split);
		split_28bit(PTR(data_b), b, b_split);
	
		gettimeofday(&es, NULL);
		total_s += (es.tv_sec - ss.tv_sec) + (es.tv_usec - ss.tv_usec)*1.0E-6;

	
		int plus = BITSIZE/448 * 16;
		
		gettimeofday(&sk, NULL);

		int offset;

		for(i=0; i<b_digits; i+=plus){ 
			for(j=0; j<a_digits; j+=plus){
			
				offset = (i + j) * 2;

				multiply_outer(a+j, b+i, t_tmp+offset, u+(offset), v+(offset), w+(offset), p+(offset), q+(offset), r+(offset), s+(offset));
			}
		}

		//Finally, we do the 29bit carry calculation
		//and add the two result arrays
		calc_carry(t_tmp, u, v, w, p, q, r, s, digitsTimesTwo);

		gettimeofday(&ek, NULL);
		total_k += (ek.tv_sec - sk.tv_sec) + (ek.tv_usec - sk.tv_usec)*1.0E-6;


		gettimeofday(&sc, NULL);
		combine_28bit(t_tmp, PTR(t), t_comb);
		gettimeofday(&ec, NULL);
		total_c += (ec.tv_sec - sc.tv_sec) + (ec.tv_usec - sc.tv_usec)*1.0E-6;
	


	//combine_28bit(t_tmp, t, t_comb);

	gettimeofday(&sa, NULL);

	hbw_free(a);

	if(t_bitsize > 179*32){
	hbw_free(b);
	hbw_free(t_tmp);
	hbw_free(u);
	hbw_free(v);
	hbw_free(w);
	hbw_free(p);
	hbw_free(q);
	hbw_free(r);
	hbw_free(s);
	}

	gettimeofday(&ea, NULL);
	total_a += (ea.tv_sec - sa.tv_sec) + (ea.tv_usec - sa.tv_usec)*1.0E-6;

	//a = b = t_tmp = u = v = w = p = q = r = s = NULL;
	

}



void calc_carry(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, int DIGITSTIMESTWO)
{
/*
	  in this routine,
	  mm7 : 0xfffffff
	  mm6 : carry
	  mm0 : t
	  mm1 : u
	  mm2 : v
	  mm3 : w
	 */
/*
	 __asm__ volatile(
		"pxor %%mm6, %%mm6;"
		"mov $0xfffffff, %%eax;"
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
*/
	unsigned long* t_long = (unsigned long*) t;
	unsigned long* u_long = (unsigned long*) u;
	unsigned long* v_long = (unsigned long*) v;
	unsigned long* w_long = (unsigned long*) w;
	unsigned long* p_long = (unsigned long*) p;
	unsigned long* q_long = (unsigned long*) q;
	unsigned long* r_long = (unsigned long*) r;
	unsigned long* s_long = (unsigned long*) s;

	unsigned long and28 = 0xfffffff;
	unsigned long tmp;

	// [0]
	tmp = t_long[0];
	t[0] = t_long[0] & and28;
	tmp >>= 28;

	// [1]
	tmp += (t_long[1] + u_long[0]);
	t[1] = tmp & and28;
	tmp >>= 28;

	// [2]
	tmp += (t_long[2] + u_long[1] + v_long[0]);
	t[2] = tmp & and28;
	tmp >>= 28;

	// [3]
	tmp += (t_long[3] + u_long[2] + v_long[1] + w_long[0]);
	t[3] = tmp & and28;
	tmp >>= 28;

	// [4]
	tmp += (t_long[4] + u_long[3] + v_long[2] + w_long[1] + p_long[0]);
	t[4] = tmp & and28;
	tmp >>= 28;

	// [5]
	tmp += (t_long[5] + u_long[4] + v_long[3] + w_long[2] + p_long[1] + q_long[0]);
	t[5] = tmp & and28;
	tmp >>= 28;

	// [6]
	tmp += (t_long[6] + u_long[5] + v_long[4] + w_long[3] + p_long[2] + q_long[1] + r_long[0]);
	t[6] = tmp & and28;
	tmp >>= 28;


	//cloop(t, u, v, w, p, q, r, s, DIGITSTIMESTWO);

	int i;
	for(i=7; i<=DIGITSTIMESTWO; i+=2){
	
		tmp += (t_long[i]+u_long[i-1]+v_long[i-2]+w_long[i-3]+p_long[i-4]+q_long[i-5]+r_long[i-6]+s_long[i-7]);

		t[i] = tmp & and28;
		tmp >>= 28;

		tmp += (t_long[i+1]+u_long[i]+v_long[i-1]+w_long[i-2]+p_long[i-3]+q_long[i-4]+r_long[i-5]+s_long[i-6]);

		t[i+1] = tmp & and28;
		tmp >>= 28;
	}


	long rbx = DIGITSTIMESTWO - 1;

	t[rbx] = tmp;
}


void calc_carry_29bit(unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s, int DIGITSTIMESTWO)
{
	
	unsigned long* t_long = (unsigned long*) t;
	unsigned long* u_long = (unsigned long*) u;
	unsigned long* v_long = (unsigned long*) v;
	unsigned long* w_long = (unsigned long*) w;
	unsigned long* p_long = (unsigned long*) p;
	unsigned long* q_long = (unsigned long*) q;
	unsigned long* r_long = (unsigned long*) r;
	unsigned long* s_long = (unsigned long*) s;

	unsigned long and29 = 0x1fffffff;
	unsigned long tmp;

	// [0]
	tmp = t_long[0];
	t[0] = t_long[0] & and29;
	tmp >>= 29;

	// [1]
	tmp += (t_long[1] + u_long[0]);
	t[1] = tmp & and29;
	tmp >>= 29;

	// [2]
	tmp += (t_long[2] + u_long[1] + v_long[0]);
	t[2] = tmp & and29;
	tmp >>= 29;

	// [3]
	tmp += (t_long[3] + u_long[2] + v_long[1] + w_long[0]);
	t[3] = tmp & and29;
	tmp >>= 29;

	// [4]
	tmp += (t_long[4] + u_long[3] + v_long[2] + w_long[1] + p_long[0]);
	t[4] = tmp & and29;
	tmp >>= 29;

	// [5]
	tmp += (t_long[5] + u_long[4] + v_long[3] + w_long[2] + p_long[1] + q_long[0]);
	t[5] = tmp & and29;
	tmp >>= 29;

	// [6]
	tmp += (t_long[6] + u_long[5] + v_long[4] + w_long[3] + p_long[2] + q_long[1] + r_long[0]);
	t[6] = tmp & and29;
	tmp >>= 29;


	//cloop(t, u, v, w, p, q, r, s, DIGITSTIMESTWO);

	int i;
	for(i=7; i<=DIGITSTIMESTWO; i+=2){
	
		tmp += (t_long[i]+u_long[i-1]+v_long[i-2]+w_long[i-3]+p_long[i-4]+q_long[i-5]+r_long[i-6]+s_long[i-7]);

		t[i] = tmp & and29;
		tmp >>= 29;

		tmp += (t_long[i+1]+u_long[i]+v_long[i-1]+w_long[i-2]+p_long[i-3]+q_long[i-4]+r_long[i-5]+s_long[i-6]);

		t[i+1] = tmp & and29;
		tmp >>= 29;
	}

	long rbx = DIGITSTIMESTWO - 1;

	t[rbx] = tmp;
}
