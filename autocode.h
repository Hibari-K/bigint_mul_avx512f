#define FPRINT(str) fprintf();




#define CODE_INIT			\
"\
#include<stdio.h>\n\
#include<stdlib.h>\n\
\
#include<hbwmalloc.h>\n\
#include<immintrin.h>\n\
\n\
#include \"zmm_mul.h\"\n\
\n\
\n\
void multiply_outer(unsigned int* a, unsigned int* b, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s){\n\
\n\
	__m512i zmm_fed = _mm512_set_epi64(0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8);\n\
	__m512i zmm_765 = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);\n\
	__m512i zmm_000 = _mm512_setzero_epi32();\n\
	__m512i zmm_111 = _mm512_set_epi64(1,1,1,1,1,1,1,1);\n\
	__m512i zmm_222;// = _mm512_set_epi64(2,2,2,2,2,2,2,2);\n\
	__m512i zmm_333;// = _mm512_set_epi64(3,3,3,3,3,3,3,3);\n\
	__m512i zmm_444;// = _mm512_set_epi64(4,4,4,4,4,4,4,4);\n\
	__m512i zmm_555;// = _mm512_set_epi64(5,5,5,5,5,5,5,5);\n\
	__m512i zmm_666;// = _mm512_set_epi64(6,6,6,6,6,6,6,6);\n\
	__m512i zmm_777;// = _mm512_set_epi64(7,7,7,7,7,7,7,7);\n\
	__m512i zmm_888 = _mm512_set_epi64(8,8,8,8,8,8,8,8);\n\
	__m512i zmm_999;// = _mm512_set_epi64(9,9,9,9,9,9,9,9);\n\
	__m512i zmm_aaa;// = _mm512_set_epi64(10,10,10,10,10,10,10,10);\n\
	__m512i zmm_bbb;// = _mm512_set_epi64(11,11,11,11,11,11,11,11);\n\
	__m512i zmm_ccc;// = _mm512_set_epi64(12,12,12,12,12,12,12,12);\n\
	__m512i zmm_ddd;// = _mm512_set_epi64(13,13,13,13,13,13,13,13);\n\
	__m512i zmm_eee;// = _mm512_set_epi64(14,14,14,14,14,14,14,14);\n\
	__m512i zmm_fff;// = _mm512_set_epi64(15,15,15,15,15,15,15,15);\n\
\n\
	int index_outer = 0;\n\
	int index_inner = 8;\n\
	int rdi;\n\
	int index;\n\
	__m512i zmm_a, zmm_aH, zmm_aL, zmm_b;\n\
	__m512i tmp1, tmp2;\n\
\n\
	__m512i zmm_b0 = _mm512_load_epi32(b);\n\
\n\
	__m512i zmm_a0 = _mm512_load_epi32(a);\n\
	__m512i zmm_a0H = _mm512_permutexvar_epi32(zmm_fed, zmm_a0);\n\
	__m512i zmm_a0L = _mm512_permutexvar_epi32(zmm_765, zmm_a0);\n\
\n\
	__m512i zmm_b1 = _mm512_load_epi32(b+16);\n\
\n\
	__m512i zmm_a1 = _mm512_load_epi32(a+16);\n\
	__m512i zmm_a1H = _mm512_permutexvar_epi32(zmm_fed, zmm_a1);\n\
	__m512i zmm_a1L = _mm512_permutexvar_epi32(zmm_765, zmm_a1);\n\
\n\
"


#define CODE_END "}\n"

/*
 * used in comba algorithm
 */
#define INCREMENT(i0, i1, i8, i9) \
		fprintf( fp, \
"	zmm_%c%c%c = _mm512_add_epi64(zmm_%c%c%c, zmm_111);\n\
	zmm_%c%c%c = _mm512_add_epi64(zmm_%c%c%c, zmm_111);\n",\
		i0, i0, i0, i1, i1, i1, i8, i8, i8, i9, i9, i9);
#define PERM_B0(i0, i8) \
		fprintf( fp, \
"	__m512i zmm_b0%c = _mm512_permutexvar_epi32(zmm_%c%c%c, zmm_b0);\n\
	__m512i zmm_b0%c = _mm512_permutexvar_epi32(zmm_%c%c%c, zmm_b0);\n",\
		i0, i0, i0, i0, i8, i8, i8, i8);
#define PERM_B1(i0, i8) \
		fprintf( fp, \
"	__m512i zmm_b1%c = _mm512_permutexvar_epi32(zmm_%c%c%c, zmm_b1);\n\
	__m512i zmm_b1%c = _mm512_permutexvar_epi32(zmm_%c%c%c, zmm_b1);\n",\
		i0, i0, i0, i0, i8, i8, i8, i8);

#define CALC_1(t, i) \
	fprintf( fp, \
"	__m512i %c1 = _mm512_load_epi64(%c);\n\
	%c1 = _mm512_add_epi64(%c1, _mm512_mul_epu32(zmm_a0L, zmm_b0%c));\n\
	_mm512_store_epi64(%c, %c1);\n\n", \
	t, t, t, t, i, t, t);

#define CALC_2(t, i0, i8) \
	fprintf( fp, \
"	__m512i %c2 = _mm512_load_epi64(%c+16);\n\
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0%c),\n\
							_mm512_mul_epu32(zmm_a0L, zmm_b0%c));\n\
	%c2 = _mm512_add_epi64(%c2, tmp1);\n\
	_mm512_store_epi64(%c+16, %c2);\n\n", \
	t, t, i0, i8, t, t, t, t);

#define CALC_3(t, i0, i8) \
	fprintf( fp, \
"	__m512i %c3 = _mm512_load_epi64(%c+32);\n\
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0%c),\n\
							_mm512_mul_epu32(zmm_a1L, zmm_b0%c));\n\
	%c3 = _mm512_add_epi64(%c3, _mm512_mul_epu32(zmm_a0L, zmm_b1%c));\n\
	%c3 = _mm512_add_epi64(%c3, tmp1);\n\
	_mm512_store_epi64(%c+32, %c3);\n\n", \
	t, t, i8, i0, t, t, i0, t, t, t, t);

#define CALC_4(t, i0, i8) \
	fprintf( fp, \
"	__m512i %c4 = _mm512_load_epi64(%c+48);\n\
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b0%c),\n\
							_mm512_mul_epu32(zmm_a1L, zmm_b0%c));\n\
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1%c),\n\
							_mm512_mul_epu32(zmm_a0L, zmm_b1%c));\n\
	%c4 = _mm512_add_epi64(%c4, tmp1);\n\
	%c4 = _mm512_add_epi64(%c4, tmp2);\n\
	_mm512_store_epi64(%c+48, %c4);\n\n" ,\
	t, t, i0, i8, i0, i8, t, t, t, t, t, t);

#define CALC_5(t, i0, i8) \
	fprintf( fp, \
"	__m512i %c5 = _mm512_load_epi64(%c+64);\n\
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1%c),\n\
							_mm512_mul_epu32(zmm_a1L, zmm_b1%c));\n\
	%c5 = _mm512_add_epi64(%c5, _mm512_mul_epu32(zmm_a1H, zmm_b0%c));\n\
	%c5 = _mm512_add_epi64(%c5, tmp1);\n\
	_mm512_store_epi64(%c+64, %c5);\n\n", \
	t, t, i8, i0, t, t, i8, t, t, t, t);

#define CALC_6(t, i0, i8) \
	fprintf( fp, \
"	__m512i %c6 = _mm512_load_epi64(%c+80);\n\
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b1%c),\n\
							_mm512_mul_epu32(zmm_a1L, zmm_b1%c));\n\
	%c6 = _mm512_add_epi64(%c6, tmp1);\n\
	_mm512_store_epi64(%c+80, %c6);\n\n", \
	t, t, i0, i8, t, t, t, t);

#define CALC_7(t, i) \
	fprintf( fp, \
"	__m512i %c7 = _mm512_load_epi64(%c+96);\n\
	%c7 = _mm512_add_epi64(%c7, _mm512_mul_epu32(zmm_a1H, zmm_b1%c));\n\
	_mm512_store_epi64(%c+96, %c7);\n\n", \
	t, t, t, t, i, t, t);

/*
 * used in write outer
 */

#define LOAD_A_INIT \
		fprintf( fp, \
"		zmm_a = _mm512_load_epi32(a);\n\
		zmm_aH = _mm512_permutexvar_epi32(zmm_fed, zmm_a);\n\
		zmm_aL = _mm512_permutexvar_epi32(zmm_765, zmm_a);\n\n");

#define LOAD_B \
		fprintf( fp, \
"		zmm_b = _mm512_load_epi32(b+index_outer);\n");
#define PERM_B(i) \
		fprintf( fp, \
"		__m512i zmm_b%c = _mm512_permutexvar_epi32(zmm_%c%c%c, zmm_b);\n",\
		i, i, i, i);
#define LOAD_L(s) \
		fprintf( fp, \
"		__m512i %cl = _mm512_load_epi64(%c+(index_outer*2));\n",\
		s, s);
#define LOAD_H(s) \
		fprintf( fp, \
"		__m512i %ch = _mm512_load_epi64(%c+(index_outer*2)+16);\n\n",\
		s, s);
#define MUL_ADD_FIRST(s, i) \
		fprintf( fp, \
"         %cl = _mm512_add_epi64(%cl, _mm512_mul_epu32(zmm_aL, zmm_b%c));\n",\
		s, s, i);
#define STORE_FIRST(s) \
		fprintf( fp, \
"        _mm512_store_epi64(%c+(index_outer*2), %cl);\n",\
		s, s);

#define CALC_RDI \
		fprintf( fp, \
"        rdi = (index_outer*2) + 16 * (((DIGITS - 1) / 8) + 1);\n\n");

#define MUL_ADD_LAST(s, i) \
		fprintf( fp, \
"        %ch = _mm512_add_epi64(%ch, _mm512_mul_epu32(zmm_aL, zmm_b%c));\n",\
		s, s, i);
#define STORE_LAST(s) \
		fprintf( fp, \
"        _mm512_store_epi64(%c+(rdi), %ch);\n",\
		s, s);



/*
 * used in inner
 */

#define CALC_INDEX \
		fprintf( fp, \
"			index = (index_inner * 2) + (index_outer * 2);\n");
#define MUL_ADD_TMP(i1, i2, s) \
		fprintf( fp, \
"			__m512i tmp_%c = _mm512_add_epi64(_mm512_mul_epu32(zmm_aH, zmm_b%c),\n\
											_mm512_mul_epu32(zmm_aL, zmm_b%c));\n",\
		s, i1, i2);

#define ADD_TMP(s) \
		fprintf( fp, \
"			%ch = _mm512_add_epi64(tmp_%c, %ch);\n",\
		s, s, s);
#define STORE_INNER(s) \
		fprintf( fp, \
"			_mm512_store_epi64(%c+(index) ,%ch);\n",\
		s, s);
#define LOAD_A_INNER \
		fprintf( fp, \
"			zmm_a = _mm512_load_epi32(a+index_inner);\n\
			zmm_aL = zmm_aH;\n\
			zmm_aH = _mm512_permutexvar_epi32(zmm_fed, zmm_a);\n\n");
#define LOAD_INNER(s) \
		fprintf( fp, \
"			%ch = _mm512_load_epi64(%c+(index)+16);\n",\
		s, s);



