typedef struct zmm_mul{
	unsigned int* n;
	size_t size;
}zmm_t;

#define PTR(x) ((x).n)
#define SIZ(x) ((x).size)

void multiply(zmm_t a, zmm_t b, zmm_t t);
