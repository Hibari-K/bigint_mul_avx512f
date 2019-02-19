# large integer multiplication with AVX-512F
Multiplication using AVX-512F instructions with naive algorithm

## Dependencies
- The processor that can execute AVX-512F instructions
    - 2nd gen of Intel Xeon Phi (co)processor (Knights Landing)
    - Intel Skylake X processor

Note that Makefile is written for KNL architecture.  
To use this on Skylake X, change compile option from "-xMIC-AVX512" to "-xSKYLAKE-AVX512" because Skylake X processor cannot execute AVX-512PF intructions.

## How to use
declare "zmm_t" value, malloc for each zmm_t value, and set operand size of value. "SIZ" is the number of 32-bit integer arrays. For example, in 2048-bit * 2048-bit multiplication, SIZ(a) and SIZ(b) are 64 (2048 / 32).  
call "multiply()" function for caluculate a \* b multiplication.

PTR(a) is a pointer of unsigned int array.

## Future work
- implement initialize function for zmm_t to allocate memory, set size of value, and set multiplier and multiplicand.  
- implement delete function to "free" values.

## Example
```
zmm_t a, b, t;

PTR(a) = hbw_calloc((1024), sizeof(long));
PTR(b) = hbw_calloc((1024), sizeof(long));
PTR(t) = hbw_calloc((2048), sizeof(long));

SIZ(a) = 64;
SIZ(b) = 64;
SIZ(t) = 128;

multiply(a, b, t);
```

_cmain.c_ file is a concrete example to use zmm_t and multiply() function.
