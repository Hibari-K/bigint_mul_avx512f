#define BITSIZE 128 * 1
#define N BITSIZE/32 //dataの配列数
#define M BITSIZE/28 //resultの配列数を決める指標
//#define R BITSIZE - 58 * (BITSIZE / 58) //余りのビット数
#define MDIGITS 2*(BITSIZE/116) // 116 = 29*4. used in split_29bit

#define DIGITS M+1
#define DIGITSTIMESTWO ((M+1)*2)+1

#define SPLITDIGITS (BITSIZE/56) // 112 = 28*4. used in split_28bit
#define COMDIGITS 2*(BITSIZE/56)+2 // (BITSIZE/56). used in combine_28bit

// use in BigMultiply
#define LoWord(result)  result & 0xffffffff
#define HiWord(result) (result & 0xffffffff00000000) >> 32
