#!/bin/sh

i=15
N=55

loop=$1
st=`expr \( $N - $i \) \/ 5 + 1 + 2`	#115
en=`expr $loop + $st - 1`	#214

### initialize ###

cat hoge > zmm_mul.h

echo "N,Optimized,GMP,gradeSchool">measure.csv

bit=0
while [ $i -le $N ]
do
	bit=`expr $i \* 128`
	echo "$bit,=average(A$st:A$en),=average(B$st:B$en),=average(C$st:C$en)">>measure.csv
	i=`expr $i + 5`
	st=`expr $st + $loop`
	en=`expr $en + $loop`
done

### measure ###

outer=15

cat hoge | sed "s/128 \* 1/128 \* $outer/" > zmm_mul.h
make clean all

while [ $outer -le $N ]
do
	i=0
	while [ $i -lt $loop ]
	do
		echo "$outer : $i"
		./mul `perl -e "print '11111111'x(4*$outer-1)"` >> measure.csv
		sleep 0.1s
		i=`expr $i + 1`
	done
	outer=`expr $outer + 5`
	cat hoge | sed "s/128 \* 1/128 \* $outer/" > zmm_mul.h
	make clean all
done
mv measure.csv data/
make clean


