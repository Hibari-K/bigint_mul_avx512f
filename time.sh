#!/bin/sh

init=4

interval=4
i=$init
N=56

loop=$1
st=`expr \( $N - $i \) \/ $interval + 1 + 2`	#115
en=`expr $loop + $st - 1`	#214

### initialize ###

cat hoge > zmm_mul.h

echo "N,Optimized,GMP,gradeSchool">measure.csv

bit=0
while [ $i -le $N ]
do
	bit=`expr $i \* 128`
	echo "$bit,=average(A$st:A$en),=average(B$st:B$en),=average(C$st:C$en)">>measure.csv
	i=`expr $i + $interval`
	st=`expr $st + $loop`
	en=`expr $en + $loop`
done

### measure ###

outer=$init

cat hoge | sed "s/128 \* 1/128 \* $outer/" > zmm_mul.h
make clean all

while [ $outer -le $N ]
do
	i=0
	while [ $i -lt $loop ]
	do
		echo "$outer : $i"
		./mul `perl -e "print 'ffffffff'x(4*$outer)"` >> measure.csv
		sleep 0.1s
		i=`expr $i + 1`
	done
	outer=`expr $outer + $interval`
	cat hoge | sed "s/128 \* 1/128 \* $outer/" > zmm_mul.h
	make clean all
done
mv measure.csv data/
make clean


