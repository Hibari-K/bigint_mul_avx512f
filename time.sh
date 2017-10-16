#!/bin/sh

N=15
loop=$1
st=`expr $N + 1`	#115
tmp=`expr $N - 1`
en=`expr $loop + $tmp`	#214

### initialize ###

cat hoge > zmm_mul.h

echo "N,Optimized,GMP,gradeSchool">measure.csv
i=1
bit=0
while [ $i -lt $N ]
do
	bit=`expr $i \* 128`
	echo "$bit,=average(A$st:A$en),=average(B$st:B$en),=average(C$st:C$en)">>measure.csv
	i=`expr $i + 1`
	st=`expr $st + $loop`
	en=`expr $en + $loop`
done

### measure ###

make clean all

outer=1
cat hoge | sed "s/128 \* 1/128 \* $outer/" > zmm_mul.h
while [ $outer -lt $N ]
do
	i=0
	while [ $i -lt $loop ]
	do
		echo "$outer : $i"
		./mul `perl -e "print 'ffffffff'x(4*$outer)"` >> measure.csv
		sleep 0.1s
		i=`expr $i + 1`
	done
	outer=`expr $outer + 1`
	cat hoge | sed "s/128 \* 1/128 \* $outer/" > zmm_mul.h
	make clean all
done
mv measure.csv data/
make clean


