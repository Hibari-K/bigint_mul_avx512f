CC		= icc
CFLAGS	= -g -Wall -O3 -lmemkind -xMIC-AVX512 -fno-alias -std=c99
OBJS	= multiply.o split_28bit.o combine_28bit.o cmain.o mul_kernel.o
FILES	= multiply.c split_28bit.c combine_28bit.c cmain.c mul_kernel.c
PROGRAM	= mul

all:	$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(CC) $(IFLAGS) $(OBJS) -o $(PROGRAM)
$(OBJS): $(FILES)
	$(CC) -c $(IFLAGS) $(FILES) 


clean:
	rm -f *.o $(PROGRAM)

