#define BITSIZE 128 * 13
#define N BITSIZE/32 //dataの配列数
#define M BITSIZE/29 //resultの配列数を決める指標
//#define R BITSIZE - 58 * (BITSIZE / 58) //余りのビット数
#define MDIGITS 2*(BITSIZE/116) // 116 = 29*4. used in split_29bit

#define DIGITS M+1
#define DIGITSTIMESTWO ((M+1)*2)+1

// use in BigMultiply
#define LoWord(result)  result & 0xffffffff
#define HiWord(result) (result & 0xffffffff00000000) >> 32
