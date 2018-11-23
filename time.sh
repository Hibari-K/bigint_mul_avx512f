#!/bin/sh

init=16

interval=16
i=$init
N=224

loop=$1
st=`expr \( $N - $i \) \/ $interval + 1 + 2`	#115
en=`expr $loop + $st - 1`	#214
### initialize ###

#cat hoge > zmm_mul.h

echo ",Kernel,Split,Combine,Calloc,Other,Total,GMP">measure.csv

bit=0
while [ $i -le $N ]
do
	bit=`expr $i \* 32`
	echo "$bit,=average(B$st:B$en),=average(C$st:C$en),=average(D$st:D$en),=average(E$st:E$en),=average(F$st:F$en),=average(G$st:G$en),=average(H$st:H$en),">>measure.csv
	i=`expr $i + $interval`
	st=`expr $st + $loop`
	en=`expr $en + $loop`
done

### measure ###

outer=$init

#cat hoge | sed "s/448 \* 1/448 \* $outer/" > zmm_mul.h
make clean all

while [ $outer -le $N ]
do
	i=0
	while [ $i -lt $loop ]
	do
		echo "$outer : $i"
		numactl --membind=1 ./mul `perl -e "print ($outer)"` >> measure.csv
		diff result_gmp.txt result_t.txt
		sleep 0.1s
		i=`expr $i + 1`
	done
	outer=`expr $outer + $interval`
	#cat hoge | sed "s/448 \* 1/448 \* $outer/" > zmm_mul.h
	#make clean all
done
mv measure.csv data/
make clean


