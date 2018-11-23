#define LOOP 30.0

#define BITSIZE 448 * 2 // 28 * 16
#define N BITSIZE/32 //dataの配列数
#define M BITSIZE/28 //resultの配列数を決める指標
//#define R BITSIZE - 58 * (BITSIZE / 58) //余りのビット数
#define MDIGITS 2*(BITSIZE/116) // 116 = 29*4. used in split_29bit

typedef struct zmm_mul{
	unsigned int* n;
	size_t size;
}zmm_t;

#define PTR(x) ((x).n)
#define SIZ(x) ((x).size)


#define DIGITS BITSIZE/28 //+ 1
//#define DIGITSTIMESTWO 515//((M+1)*2)+1

//#define SPLITDIGITS (BITSIZE/56) // 112 = 28*4. used in split_28bit
//#define COMDIGITS 2*(BITSIZE/56)+2 // (BITSIZE/56). used in combine_28bit

/*
// use for fixed 2048 bit length multiplication
#define ADIGITS ((2048-1)/28 + 1)
#define DIGITSTIMESTWO 149
#define SPLITDIGITS (2048/56) // 112 = 28*4. used in split_28bit
#define COMDIGITS 2*(2048/56)+2 // (BITSIZE/56). used in combine_28bit
*/


// use in BigMultiply
#define LoWord(result)  result & 0xffffffff
#define HiWord(result) (result & 0xffffffff00000000) >> 32
