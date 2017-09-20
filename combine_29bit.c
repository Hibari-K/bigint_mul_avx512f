#include<stdio.h>

#include "zmm_mul.h"

void combine_29bit(unsigned int* data, unsigned int* result){

    int index, i, j;

    //下記for文で29通りの組み合わせがあるため。29bitという意味ではない
    const int LIMIT = (2*N) / 29;

    //あまりの組み合わせ処理用
    const int REM = 2*N - 29 * LIMIT;
    
    for(index = 0; index < LIMIT; index++){

	j = index * 29; // result
	i = index << 5; // data
	//i = index * 32; // data

	result[j] = (data[i] & 0x1fffffff) ^ ((data[i+1] & 0x7) << 29);
	result[j+1] = ((data[i+1] & 0x1ffffff8) >> 3) ^ ((data[i+2] & 0x3f) << 26);	
	result[j+2] = ((data[i+2] & 0x1fffffc0) >> 6) ^ ((data[i+3] & 0x1ff) << 23);
	result[j+3] = ((data[i+3] & 0x1ffffe00) >> 9) ^ ((data[i+4] & 0xfff) << 20);
	result[j+4] = ((data[i+4] & 0x1ffff000) >> 12) ^ ((data[i+5] & 0x7fff) << 17);
	result[j+5] = ((data[i+5] & 0x1fff8000) >> 15) ^ ((data[i+6] & 0x3ffff) << 14);
	result[j+6] = ((data[i+6] & 0x1ffc0000) >> 18) ^ ((data[i+7] & 0x1fffff) << 11);
	result[j+7] = ((data[i+7] & 0x1fe00000) >> 21) ^ ((data[i+8] & 0xffffff) << 8);
	result[j+8] = ((data[i+8] & 0x1f000000) >> 24) ^ ((data[i+9] & 0x7ffffff) << 5);

	result[j+9] = ((data[i+9] & 0x18000000) >> 27) ^ ((data[i+10] & 0x1fffffff) << 2) ^ ((data[i+11] & 0x1) << 31);

	result[j+10] = ((data[i+11] & 0x1ffffffe) >> 1) ^ ((data[i+12] & 0xf) << 28);
	result[j+11] = ((data[i+12] & 0x1ffffff0) >> 4) ^ ((data[i+13] & 0x7f) << 25);
	result[j+12] = ((data[i+13] & 0x1fffff80) >> 7) ^ ((data[i+14] & 0x3ff) << 22);
	result[j+13] = ((data[i+14] & 0x1ffffc00) >> 10) ^ ((data[i+15] & 0x1fff) << 19);
	result[j+14] = ((data[i+15] & 0x1fffe000) >> 13) ^ ((data[i+16] & 0xffff) << 16);
	result[j+15] = ((data[i+16] & 0x1fff0000) >> 16) ^ ((data[i+17] & 0x7ffff) << 13);
	result[j+16] = ((data[i+17] & 0x1ff80000) >> 19) ^ ((data[i+18] & 0x3fffff) << 10);
	result[j+17] = ((data[i+18] & 0x1fc00000) >> 22) ^ ((data[i+19] & 0x1ffffff) << 7);
	result[j+18] = ((data[i+19] & 0x1e000000) >> 25) ^ ((data[i+20] & 0xfffffff) << 4);

	result[j+19] = ((data[i+20] & 0x10000000) >> 28) ^ ((data[i+21] & 0x1fffffff) << 1) ^ ((data[i+22] & 0x3) << 30);

	result[j+20] = ((data[i+22] & 0x1ffffffc) >> 2) ^ ((data[i+23] & 0x1f) << 27);
	result[j+21] = ((data[i+23] & 0x1fffffe0) >> 5) ^ ((data[i+24] & 0xff) << 24);
	result[j+22] = ((data[i+24] & 0x1fffff00) >> 8) ^ ((data[i+25] & 0x7ff) << 21);
	result[j+23] = ((data[i+25] & 0x1ffff800) >> 11) ^ ((data[i+26] & 0x3fff) << 18);
	result[j+24] = ((data[i+26] & 0x1fffc000) >> 14) ^ ((data[i+27] & 0x1ffff) << 15);
	result[j+25] = ((data[i+27] & 0x1ffe0000) >> 17) ^ ((data[i+28] & 0xfffff) << 12);
	result[j+26] = ((data[i+28] & 0x1ff00000) >> 20) ^ ((data[i+29] & 0x7fffff) << 9);
	result[j+27] = ((data[i+29] & 0x1f800000) >> 23) ^ ((data[i+30] & 0x3ffffff) << 6);
	result[j+28] = ((data[i+30] & 0x1c000000) >> 26) ^ ((data[i+31] & 0x1fffffff) << 3);

	
    }

    j = index * 29;
    i = index << 5;
    int offset;
    
    if(REM == 0){ return; }
    else if(REM < 9){

	for(offset=0; offset<REM; offset++){
	    
	    result[j+offset] =
		((data[i+offset] & ((0x1fffffff >> offset*3) << offset*3)) >> offset*3) ^
		((data[i+offset+1] & (0x1fffffff >> (29 - (offset+1)*3))) << (29 - offset*3));
	}
    }
    else if(REM < 19){

	result[j] = (data[i] & 0x1fffffff) ^ ((data[i+1] & 0x7) << 29);
	result[j+1] = ((data[i+1] & 0x1ffffff8) >> 3) ^ ((data[i+2] & 0x3f) << 26);
	result[j+2] = ((data[i+2] & 0x1fffffc0) >> 6) ^ ((data[i+3] & 0x1ff) << 23);
	result[j+3] = ((data[i+3] & 0x1ffffe00) >> 9) ^ ((data[i+4] & 0xfff) << 20);
	result[j+4] = ((data[i+4] & 0x1ffff000) >> 12) ^ ((data[i+5] & 0x7fff) << 17);
	result[j+5] = ((data[i+5] & 0x1fff8000) >> 15) ^ ((data[i+6] & 0x3ffff) << 14);
	result[j+6] = ((data[i+6] & 0x1ffc0000) >> 18) ^ ((data[i+7] & 0x1fffff) << 11);
	result[j+7] = ((data[i+7] & 0x1fe00000) >> 21) ^ ((data[i+8] & 0xffffff) << 8);
	result[j+8] = ((data[i+8] & 0x1f000000) >> 24) ^ ((data[i+9] & 0x7ffffff) << 5);

	result[j+9] = ((data[i+9] & 0x18000000) >> 27) ^ ((data[i+10] & 0x1fffffff) << 2) ^ ((data[i+11] & 0x1) << 31);

	int shift = 0;
	
	for(offset=10; offset<REM; offset++, shift++){

	    result[j+offset] =
		((data[i+offset+1] & ((0x1fffffff >> (shift*3+1)) << (shift*3+1))) >> (shift*3+1)) ^
		((data[i+offset+2] & (0x1fffffff >> (28 - (shift+1)*3))) << (28 - shift*3));
	    
	}
    }
    else if(REM < 29){

		result[j] = (data[i] & 0x1fffffff) ^ ((data[i+1] & 0x7) << 29);
		result[j+1] = ((data[i+1] & 0x1ffffff8) >> 3) ^ ((data[i+2] & 0x3f) << 26);
		result[j+2] = ((data[i+2] & 0x1fffffc0) >> 6) ^ ((data[i+3] & 0x1ff) << 23);
		result[j+3] = ((data[i+3] & 0x1ffffe00) >> 9) ^ ((data[i+4] & 0xfff) << 20);
		result[j+4] = ((data[i+4] & 0x1ffff000) >> 12) ^ ((data[i+5] & 0x7fff) << 17);
		result[j+5] = ((data[i+5] & 0x1fff8000) >> 15) ^ ((data[i+6] & 0x3ffff) << 14);
		result[j+6] = ((data[i+6] & 0x1ffc0000) >> 18) ^ ((data[i+7] & 0x1fffff) << 11);
		result[j+7] = ((data[i+7] & 0x1fe00000) >> 21) ^ ((data[i+8] & 0xffffff) << 8);
		result[j+8] = ((data[i+8] & 0x1f000000) >> 24) ^ ((data[i+9] & 0x7ffffff) << 5);
	
		result[j+9] = ((data[i+9] & 0x18000000) >> 27) ^ ((data[i+10] & 0x1fffffff) << 2) ^ ((data[i+11] & 0x1) << 31);

		result[j+10] = ((data[i+11] & 0x1ffffffe) >> 1) ^ ((data[i+12] & 0xf) << 28);
		result[j+11] = ((data[i+12] & 0x1ffffff0) >> 4) ^ ((data[i+13] & 0x7f) << 25);
		result[j+12] = ((data[i+13] & 0x1fffff80) >> 7) ^ ((data[i+14] & 0x3ff) << 22);
		result[j+13] = ((data[i+14] & 0x1ffffc00) >> 10) ^ ((data[i+15] & 0x1fff) << 19);
		result[j+14] = ((data[i+15] & 0x1fffe000) >> 13) ^ ((data[i+16] & 0xffff) << 16);
		result[j+15] = ((data[i+16] & 0x1fff0000) >> 16) ^ ((data[i+17] & 0x7ffff) << 13);
		result[j+16] = ((data[i+17] & 0x1ff80000) >> 19) ^ ((data[i+18] & 0x3fffff) << 10);
		result[j+17] = ((data[i+18] & 0x1fc00000) >> 22) ^ ((data[i+19] & 0x1ffffff) << 7);
		result[j+18] = ((data[i+19] & 0x1e000000) >> 25) ^ ((data[i+20] & 0xfffffff) << 4);

		result[j+19] = ((data[i+20] & 0x10000000) >> 28) ^ ((data[i+21] & 0x1fffffff) << 1) ^ ((data[i+22] & 0x3) << 30);

		int shift = 0;

		for(offset=20; offset<REM; offset++, shift++){
	    
	    	result[j+offset] =
			((data[i+offset+2] & ((0x1fffffff >> (shift*3+2)) << (shift*3+2))) >> (shift*3+2)) ^
			((data[i+offset+3] & (0x1fffffff >> (27 - (shift+1)*3))) << (27 - shift*3));

		}
    }
}
