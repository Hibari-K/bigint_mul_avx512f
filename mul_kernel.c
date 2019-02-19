#include<stdio.h>
#include<stdlib.h>
#include<hbwmalloc.h>
#include<immintrin.h>

#include "zmm_mul.h"


void multiply_outer(unsigned int* a, unsigned int* b, unsigned int* t, unsigned int* u, unsigned int* v, unsigned int* w, unsigned int* p, unsigned int* q, unsigned int* r, unsigned int* s){

	__m512i zmm_fed = _mm512_set_epi64(0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8);
	__m512i zmm_765 = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);
	__m512i zmm_000 = _mm512_setzero_epi32();
	__m512i zmm_111 = _mm512_set_epi64(1,1,1,1,1,1,1,1);
	__m512i zmm_222;// = _mm512_set_epi64(2,2,2,2,2,2,2,2);
	__m512i zmm_333;// = _mm512_set_epi64(3,3,3,3,3,3,3,3);
	__m512i zmm_444;// = _mm512_set_epi64(4,4,4,4,4,4,4,4);
	__m512i zmm_555;// = _mm512_set_epi64(5,5,5,5,5,5,5,5);
	__m512i zmm_666;// = _mm512_set_epi64(6,6,6,6,6,6,6,6);
	__m512i zmm_777;// = _mm512_set_epi64(7,7,7,7,7,7,7,7);
	__m512i zmm_888 = _mm512_set_epi64(8,8,8,8,8,8,8,8);
	__m512i zmm_999;// = _mm512_set_epi64(9,9,9,9,9,9,9,9);
	__m512i zmm_aaa;// = _mm512_set_epi64(10,10,10,10,10,10,10,10);
	__m512i zmm_bbb;// = _mm512_set_epi64(11,11,11,11,11,11,11,11);
	__m512i zmm_ccc;// = _mm512_set_epi64(12,12,12,12,12,12,12,12);
	__m512i zmm_ddd;// = _mm512_set_epi64(13,13,13,13,13,13,13,13);
	__m512i zmm_eee;// = _mm512_set_epi64(14,14,14,14,14,14,14,14);
	__m512i zmm_fff;// = _mm512_set_epi64(15,15,15,15,15,15,15,15);

	int index_outer = 0;
	int index_inner = 8;
	int rdi;
	int index;
	__m512i zmm_a, zmm_aH, zmm_aL, zmm_b;
	__m512i tmp1, tmp2;

	__m512i zmm_b0 = _mm512_load_epi32(b);

	__m512i zmm_a0 = _mm512_load_epi32(a);
	__m512i zmm_a0H = _mm512_permutexvar_epi32(zmm_fed, zmm_a0);
	__m512i zmm_a0L = _mm512_permutexvar_epi32(zmm_765, zmm_a0);

	__m512i zmm_b1 = _mm512_load_epi32(b+16);

	__m512i zmm_a1 = _mm512_load_epi32(a+16);
	__m512i zmm_a1H = _mm512_permutexvar_epi32(zmm_fed, zmm_a1);
	__m512i zmm_a1L = _mm512_permutexvar_epi32(zmm_765, zmm_a1);

	__m512i zmm_b00 = _mm512_permutexvar_epi32(zmm_000, zmm_b0);
	__m512i zmm_b08 = _mm512_permutexvar_epi32(zmm_888, zmm_b0);
	__m512i zmm_b10 = _mm512_permutexvar_epi32(zmm_000, zmm_b1);
	__m512i zmm_b18 = _mm512_permutexvar_epi32(zmm_888, zmm_b1);
	zmm_111 = _mm512_add_epi64(zmm_000, zmm_111);
	zmm_999 = _mm512_add_epi64(zmm_888, zmm_111);
	__m512i zmm_b01 = _mm512_permutexvar_epi32(zmm_111, zmm_b0);
	__m512i zmm_b09 = _mm512_permutexvar_epi32(zmm_999, zmm_b0);
	__m512i zmm_b11 = _mm512_permutexvar_epi32(zmm_111, zmm_b1);
	__m512i zmm_b19 = _mm512_permutexvar_epi32(zmm_999, zmm_b1);
	__m512i t1 = _mm512_load_epi64(t);
	t1 = _mm512_add_epi64(t1, _mm512_mul_epu32(zmm_a0L, zmm_b00));
	_mm512_store_epi64(t, t1);

	__m512i u1 = _mm512_load_epi64(u);
	u1 = _mm512_add_epi64(u1, _mm512_mul_epu32(zmm_a0L, zmm_b01));
	_mm512_store_epi64(u, u1);

	__m512i t2 = _mm512_load_epi64(t+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b00),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b08));
	t2 = _mm512_add_epi64(t2, tmp1);
	_mm512_store_epi64(t+16, t2);

	__m512i u2 = _mm512_load_epi64(u+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b01),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b09));
	u2 = _mm512_add_epi64(u2, tmp1);
	_mm512_store_epi64(u+16, u2);

	__m512i t3 = _mm512_load_epi64(t+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b08),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b00));
	t3 = _mm512_add_epi64(t3, _mm512_mul_epu32(zmm_a0L, zmm_b10));
	t3 = _mm512_add_epi64(t3, tmp1);
	_mm512_store_epi64(t+32, t3);

	__m512i u3 = _mm512_load_epi64(u+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b09),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b01));
	u3 = _mm512_add_epi64(u3, _mm512_mul_epu32(zmm_a0L, zmm_b11));
	u3 = _mm512_add_epi64(u3, tmp1);
	_mm512_store_epi64(u+32, u3);

	__m512i t4 = _mm512_load_epi64(t+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b00),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b08));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b10),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b18));
	t4 = _mm512_add_epi64(t4, tmp1);
	t4 = _mm512_add_epi64(t4, tmp2);
	_mm512_store_epi64(t+48, t4);

	__m512i u4 = _mm512_load_epi64(u+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b01),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b09));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b11),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b19));
	u4 = _mm512_add_epi64(u4, tmp1);
	u4 = _mm512_add_epi64(u4, tmp2);
	_mm512_store_epi64(u+48, u4);

	__m512i t5 = _mm512_load_epi64(t+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b18),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b10));
	t5 = _mm512_add_epi64(t5, _mm512_mul_epu32(zmm_a1H, zmm_b08));
	t5 = _mm512_add_epi64(t5, tmp1);
	_mm512_store_epi64(t+64, t5);

	__m512i u5 = _mm512_load_epi64(u+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b19),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b11));
	u5 = _mm512_add_epi64(u5, _mm512_mul_epu32(zmm_a1H, zmm_b09));
	u5 = _mm512_add_epi64(u5, tmp1);
	_mm512_store_epi64(u+64, u5);

	__m512i t6 = _mm512_load_epi64(t+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b10),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b18));
	t6 = _mm512_add_epi64(t6, tmp1);
	_mm512_store_epi64(t+80, t6);

	__m512i u6 = _mm512_load_epi64(u+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b11),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b19));
	u6 = _mm512_add_epi64(u6, tmp1);
	_mm512_store_epi64(u+80, u6);

	__m512i t7 = _mm512_load_epi64(t+96);
	t7 = _mm512_add_epi64(t7, _mm512_mul_epu32(zmm_a1H, zmm_b18));
	_mm512_store_epi64(t+96, t7);

	__m512i u7 = _mm512_load_epi64(u+96);
	u7 = _mm512_add_epi64(u7, _mm512_mul_epu32(zmm_a1H, zmm_b19));
	_mm512_store_epi64(u+96, u7);

	zmm_222 = _mm512_add_epi64(zmm_111, zmm_111);
	zmm_aaa = _mm512_add_epi64(zmm_999, zmm_111);
	__m512i zmm_b02 = _mm512_permutexvar_epi32(zmm_222, zmm_b0);
	__m512i zmm_b0a = _mm512_permutexvar_epi32(zmm_aaa, zmm_b0);
	__m512i zmm_b12 = _mm512_permutexvar_epi32(zmm_222, zmm_b1);
	__m512i zmm_b1a = _mm512_permutexvar_epi32(zmm_aaa, zmm_b1);
	zmm_333 = _mm512_add_epi64(zmm_222, zmm_111);
	zmm_bbb = _mm512_add_epi64(zmm_aaa, zmm_111);
	__m512i zmm_b03 = _mm512_permutexvar_epi32(zmm_333, zmm_b0);
	__m512i zmm_b0b = _mm512_permutexvar_epi32(zmm_bbb, zmm_b0);
	__m512i zmm_b13 = _mm512_permutexvar_epi32(zmm_333, zmm_b1);
	__m512i zmm_b1b = _mm512_permutexvar_epi32(zmm_bbb, zmm_b1);
	__m512i v1 = _mm512_load_epi64(v);
	v1 = _mm512_add_epi64(v1, _mm512_mul_epu32(zmm_a0L, zmm_b02));
	_mm512_store_epi64(v, v1);

	__m512i w1 = _mm512_load_epi64(w);
	w1 = _mm512_add_epi64(w1, _mm512_mul_epu32(zmm_a0L, zmm_b03));
	_mm512_store_epi64(w, w1);

	__m512i v2 = _mm512_load_epi64(v+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b02),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b0a));
	v2 = _mm512_add_epi64(v2, tmp1);
	_mm512_store_epi64(v+16, v2);

	__m512i w2 = _mm512_load_epi64(w+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b03),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b0b));
	w2 = _mm512_add_epi64(w2, tmp1);
	_mm512_store_epi64(w+16, w2);

	__m512i v3 = _mm512_load_epi64(v+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0a),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b02));
	v3 = _mm512_add_epi64(v3, _mm512_mul_epu32(zmm_a0L, zmm_b12));
	v3 = _mm512_add_epi64(v3, tmp1);
	_mm512_store_epi64(v+32, v3);

	__m512i w3 = _mm512_load_epi64(w+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0b),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b03));
	w3 = _mm512_add_epi64(w3, _mm512_mul_epu32(zmm_a0L, zmm_b13));
	w3 = _mm512_add_epi64(w3, tmp1);
	_mm512_store_epi64(w+32, w3);

	__m512i v4 = _mm512_load_epi64(v+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b02),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b0a));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b12),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b1a));
	v4 = _mm512_add_epi64(v4, tmp1);
	v4 = _mm512_add_epi64(v4, tmp2);
	_mm512_store_epi64(v+48, v4);

	__m512i w4 = _mm512_load_epi64(w+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b03),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b0b));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b13),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b1b));
	w4 = _mm512_add_epi64(w4, tmp1);
	w4 = _mm512_add_epi64(w4, tmp2);
	_mm512_store_epi64(w+48, w4);

	__m512i v5 = _mm512_load_epi64(v+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1a),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b12));
	v5 = _mm512_add_epi64(v5, _mm512_mul_epu32(zmm_a1H, zmm_b0a));
	v5 = _mm512_add_epi64(v5, tmp1);
	_mm512_store_epi64(v+64, v5);

	__m512i w5 = _mm512_load_epi64(w+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1b),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b13));
	w5 = _mm512_add_epi64(w5, _mm512_mul_epu32(zmm_a1H, zmm_b0b));
	w5 = _mm512_add_epi64(w5, tmp1);
	_mm512_store_epi64(w+64, w5);

	__m512i v6 = _mm512_load_epi64(v+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b12),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b1a));
	v6 = _mm512_add_epi64(v6, tmp1);
	_mm512_store_epi64(v+80, v6);

	__m512i w6 = _mm512_load_epi64(w+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b13),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b1b));
	w6 = _mm512_add_epi64(w6, tmp1);
	_mm512_store_epi64(w+80, w6);

	__m512i v7 = _mm512_load_epi64(v+96);
	v7 = _mm512_add_epi64(v7, _mm512_mul_epu32(zmm_a1H, zmm_b1a));
	_mm512_store_epi64(v+96, v7);

	__m512i w7 = _mm512_load_epi64(w+96);
	w7 = _mm512_add_epi64(w7, _mm512_mul_epu32(zmm_a1H, zmm_b1b));
	_mm512_store_epi64(w+96, w7);

	zmm_444 = _mm512_add_epi64(zmm_333, zmm_111);
	zmm_ccc = _mm512_add_epi64(zmm_bbb, zmm_111);
	__m512i zmm_b04 = _mm512_permutexvar_epi32(zmm_444, zmm_b0);
	__m512i zmm_b0c = _mm512_permutexvar_epi32(zmm_ccc, zmm_b0);
	__m512i zmm_b14 = _mm512_permutexvar_epi32(zmm_444, zmm_b1);
	__m512i zmm_b1c = _mm512_permutexvar_epi32(zmm_ccc, zmm_b1);
	zmm_555 = _mm512_add_epi64(zmm_444, zmm_111);
	zmm_ddd = _mm512_add_epi64(zmm_ccc, zmm_111);
	__m512i zmm_b05 = _mm512_permutexvar_epi32(zmm_555, zmm_b0);
	__m512i zmm_b0d = _mm512_permutexvar_epi32(zmm_ddd, zmm_b0);
	__m512i zmm_b15 = _mm512_permutexvar_epi32(zmm_555, zmm_b1);
	__m512i zmm_b1d = _mm512_permutexvar_epi32(zmm_ddd, zmm_b1);
	__m512i p1 = _mm512_load_epi64(p);
	p1 = _mm512_add_epi64(p1, _mm512_mul_epu32(zmm_a0L, zmm_b04));
	_mm512_store_epi64(p, p1);

	__m512i q1 = _mm512_load_epi64(q);
	q1 = _mm512_add_epi64(q1, _mm512_mul_epu32(zmm_a0L, zmm_b05));
	_mm512_store_epi64(q, q1);

	__m512i p2 = _mm512_load_epi64(p+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b04),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b0c));
	p2 = _mm512_add_epi64(p2, tmp1);
	_mm512_store_epi64(p+16, p2);

	__m512i q2 = _mm512_load_epi64(q+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b05),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b0d));
	q2 = _mm512_add_epi64(q2, tmp1);
	_mm512_store_epi64(q+16, q2);

	__m512i p3 = _mm512_load_epi64(p+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0c),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b04));
	p3 = _mm512_add_epi64(p3, _mm512_mul_epu32(zmm_a0L, zmm_b14));
	p3 = _mm512_add_epi64(p3, tmp1);
	_mm512_store_epi64(p+32, p3);

	__m512i q3 = _mm512_load_epi64(q+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0d),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b05));
	q3 = _mm512_add_epi64(q3, _mm512_mul_epu32(zmm_a0L, zmm_b15));
	q3 = _mm512_add_epi64(q3, tmp1);
	_mm512_store_epi64(q+32, q3);

	__m512i p4 = _mm512_load_epi64(p+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b04),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b0c));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b14),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b1c));
	p4 = _mm512_add_epi64(p4, tmp1);
	p4 = _mm512_add_epi64(p4, tmp2);
	_mm512_store_epi64(p+48, p4);

	__m512i q4 = _mm512_load_epi64(q+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b05),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b0d));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b15),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b1d));
	q4 = _mm512_add_epi64(q4, tmp1);
	q4 = _mm512_add_epi64(q4, tmp2);
	_mm512_store_epi64(q+48, q4);

	__m512i p5 = _mm512_load_epi64(p+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1c),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b14));
	p5 = _mm512_add_epi64(p5, _mm512_mul_epu32(zmm_a1H, zmm_b0c));
	p5 = _mm512_add_epi64(p5, tmp1);
	_mm512_store_epi64(p+64, p5);

	__m512i q5 = _mm512_load_epi64(q+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1d),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b15));
	q5 = _mm512_add_epi64(q5, _mm512_mul_epu32(zmm_a1H, zmm_b0d));
	q5 = _mm512_add_epi64(q5, tmp1);
	_mm512_store_epi64(q+64, q5);

	__m512i p6 = _mm512_load_epi64(p+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b14),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b1c));
	p6 = _mm512_add_epi64(p6, tmp1);
	_mm512_store_epi64(p+80, p6);

	__m512i q6 = _mm512_load_epi64(q+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b15),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b1d));
	q6 = _mm512_add_epi64(q6, tmp1);
	_mm512_store_epi64(q+80, q6);

	__m512i p7 = _mm512_load_epi64(p+96);
	p7 = _mm512_add_epi64(p7, _mm512_mul_epu32(zmm_a1H, zmm_b1c));
	_mm512_store_epi64(p+96, p7);

	__m512i q7 = _mm512_load_epi64(q+96);
	q7 = _mm512_add_epi64(q7, _mm512_mul_epu32(zmm_a1H, zmm_b1d));
	_mm512_store_epi64(q+96, q7);

	zmm_666 = _mm512_add_epi64(zmm_555, zmm_111);
	zmm_eee = _mm512_add_epi64(zmm_ddd, zmm_111);
	__m512i zmm_b06 = _mm512_permutexvar_epi32(zmm_666, zmm_b0);
	__m512i zmm_b0e = _mm512_permutexvar_epi32(zmm_eee, zmm_b0);
	__m512i zmm_b16 = _mm512_permutexvar_epi32(zmm_666, zmm_b1);
	__m512i zmm_b1e = _mm512_permutexvar_epi32(zmm_eee, zmm_b1);
	zmm_777 = _mm512_add_epi64(zmm_666, zmm_111);
	zmm_fff = _mm512_add_epi64(zmm_eee, zmm_111);
	__m512i zmm_b07 = _mm512_permutexvar_epi32(zmm_777, zmm_b0);
	__m512i zmm_b0f = _mm512_permutexvar_epi32(zmm_fff, zmm_b0);
	__m512i zmm_b17 = _mm512_permutexvar_epi32(zmm_777, zmm_b1);
	__m512i zmm_b1f = _mm512_permutexvar_epi32(zmm_fff, zmm_b1);
	__m512i r1 = _mm512_load_epi64(r);
	r1 = _mm512_add_epi64(r1, _mm512_mul_epu32(zmm_a0L, zmm_b06));
	_mm512_store_epi64(r, r1);

	__m512i s1 = _mm512_load_epi64(s);
	s1 = _mm512_add_epi64(s1, _mm512_mul_epu32(zmm_a0L, zmm_b07));
	_mm512_store_epi64(s, s1);

	__m512i r2 = _mm512_load_epi64(r+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b06),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b0e));
	r2 = _mm512_add_epi64(r2, tmp1);
	_mm512_store_epi64(r+16, r2);

	__m512i s2 = _mm512_load_epi64(s+16);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b07),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b0f));
	s2 = _mm512_add_epi64(s2, tmp1);
	_mm512_store_epi64(s+16, s2);

	__m512i r3 = _mm512_load_epi64(r+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0e),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b06));
	r3 = _mm512_add_epi64(r3, _mm512_mul_epu32(zmm_a0L, zmm_b16));
	r3 = _mm512_add_epi64(r3, tmp1);
	_mm512_store_epi64(r+32, r3);

	__m512i s3 = _mm512_load_epi64(s+32);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b0f),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b07));
	s3 = _mm512_add_epi64(s3, _mm512_mul_epu32(zmm_a0L, zmm_b17));
	s3 = _mm512_add_epi64(s3, tmp1);
	_mm512_store_epi64(s+32, s3);

	__m512i r4 = _mm512_load_epi64(r+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b06),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b0e));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b16),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b1e));
	r4 = _mm512_add_epi64(r4, tmp1);
	r4 = _mm512_add_epi64(r4, tmp2);
	_mm512_store_epi64(r+48, r4);

	__m512i s4 = _mm512_load_epi64(s+48);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b07),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b0f));
	tmp2 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b17),
	                        _mm512_mul_epu32(zmm_a0L, zmm_b1f));
	s4 = _mm512_add_epi64(s4, tmp1);
	s4 = _mm512_add_epi64(s4, tmp2);
	_mm512_store_epi64(s+48, s4);

	__m512i r5 = _mm512_load_epi64(r+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1e),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b16));
	r5 = _mm512_add_epi64(r5, _mm512_mul_epu32(zmm_a1H, zmm_b0e));
	r5 = _mm512_add_epi64(r5, tmp1);
	_mm512_store_epi64(r+64, r5);

	__m512i s5 = _mm512_load_epi64(s+64);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a0H, zmm_b1f),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b17));
	s5 = _mm512_add_epi64(s5, _mm512_mul_epu32(zmm_a1H, zmm_b0f));
	s5 = _mm512_add_epi64(s5, tmp1);
	_mm512_store_epi64(s+64, s5);

	__m512i r6 = _mm512_load_epi64(r+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b16),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b1e));
	r6 = _mm512_add_epi64(r6, tmp1);
	_mm512_store_epi64(r+80, r6);

	__m512i s6 = _mm512_load_epi64(s+80);
	tmp1 = _mm512_add_epi64(_mm512_mul_epu32(zmm_a1H, zmm_b17),
	                        _mm512_mul_epu32(zmm_a1L, zmm_b1f));
	s6 = _mm512_add_epi64(s6, tmp1);
	_mm512_store_epi64(s+80, s6);

	__m512i r7 = _mm512_load_epi64(r+96);
	r7 = _mm512_add_epi64(r7, _mm512_mul_epu32(zmm_a1H, zmm_b1e));
	_mm512_store_epi64(r+96, r7);

	__m512i s7 = _mm512_load_epi64(s+96);
	s7 = _mm512_add_epi64(s7, _mm512_mul_epu32(zmm_a1H, zmm_b1f));
	_mm512_store_epi64(s+96, s7);

}
