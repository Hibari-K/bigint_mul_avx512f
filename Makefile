#ICC		= icc
#IFLAGS	= -g -Wall -O0 -lgmp -xMIC-AVX512
GCC		= gcc
CFLAGS	= -g -Wall -O3 -lgmp -lmemkind 
OBJS	= opt_mul_hbw.o split_28bit.o combine_28bit.o opt_mul.o gmp_mul.o BigMultiply.o cmain.o
PROGRAM	= mul

all:	$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(GCC) $(CFLAGS) $(OBJS) -o $(PROGRAM)

clean:
	rm -f *.o $(PROGRAM)

