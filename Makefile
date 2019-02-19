CC		= icc
CFLAGS	= -g -Wall -O3 -lgmp -lmemkind -xMIC-AVX512 -fno-alias -std=c99 -qopenmp
OBJS	= multiply.o split_28bit.o combine_28bit.o split_29bit.o combine_29bit.o cmain.o
FILES	= multiply.c split_28bit.c combine_28bit.c split_29bit.c combine_29bit.c cmain.c
PROGRAM	= mul

all:	$(PROGRAM)

$(PROGRAM):	$(OBJS) opt_mul.o
	$(CC) $(IFLAGS) $(OBJS) opt_mul.o -o $(PROGRAM)
$(OBJS): $(FILES)
	$(CC) -c $(IFLAGS) $(FILES) 
opt_mul.o: opt_mul.c
	$(CC) -c $(IFLAGS) opt_mul.c 


clean:
	rm -f *.o $(PROGRAM)

