CC		= gcc
CFLAGS	= -g -Wall -O3 -lgmp
OBJS	= split_28bit.c combine_28bit.o opt_mul.o gmp_mul.o BigMultiply.o cmain.o
PROGRAM	= mul

all:	$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM)

clean:
	rm -f *.o $(PROGRAM)

