#!/bin/sh

i=40
N=55

loop=$1
st=`expr $N - $i + 2`	#115
en=`expr $loop + $st - 1`	#214

### initialize ###

cat hoge > zmm_mul.h

echo "N,Optimized,gradeSchool,GMP">measure.csv
i=1
while [ $i -lt $N ]
do
	echo "$i,=average(A$st:A$en),=average(B$st:B$en),=average(C$st:C$en)">>measure.csv
	i=`expr $i + 1`
	st=`expr $st + $loop`
	en=`expr $en + $loop`
done

### measure ###

make clean all

outer=40
while [ $outer -lt $N ]
do
	i=0
	while [ $i -lt $loop ]
	do
		echo "$outer : $i"
		./mul `perl -e "print '11111111'x(4*$outer-1)"` >> measure.csv
		sleep 0.1s
		i=`expr $i + 1`
	done
	outer=`expr $outer + 1`
	cat hoge | sed "s/128 \* 1/128 \* $outer/" > zmm_mul.h
	make clean all
done
mv measure.csv data/
make clean


