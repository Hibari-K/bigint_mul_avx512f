ICC		= icc
IFLAGS	= -g -Wall -O3 -lgmp -lmemkind -xMIC-AVX512 -fno-alias -std=c99 -qopenmp
#GCC		= gcc
#CFLAGS	= -g -Wall -O3 -lgmp -lmemkind
OBJS	=  opt_mul_scal.o multiply.o split_28bit.o combine_28bit.o split_29bit.o combine_29bit.o gmp_mul.o BigMultiply.o cmain.o
FILES	=  opt_mul_scal.c multiply.c split_28bit.c combine_28bit.c split_29bit.c combine_29bit.c gmp_mul.c BigMultiply.c cmain.c
PROGRAM	= mul

all:	$(PROGRAM)

$(PROGRAM):	$(OBJS) opt_mul.o
	$(ICC) $(IFLAGS) $(OBJS) opt_mul.o -o $(PROGRAM)
$(OBJS): $(FILES)
	$(ICC) -c $(IFLAGS) $(FILES) 
opt_mul.o: opt_mul.c
	$(ICC) -c $(IFLAGS) opt_mul.c 


clean:
	rm -f *.o $(PROGRAM)

