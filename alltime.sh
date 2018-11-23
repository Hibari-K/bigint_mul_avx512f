#!/bin/sh

init=4

interval=4
i=$init
N=32

loop=$1
st=`expr \( $N - $i \) \/ $interval + 1 + 2`	#115
en=`expr $loop + $st - 1`	#214
### initialize ###

#cat hoge > zmm_mul.h
form=0
form_end=10

while [ $form -lt $form_end ]
do
	
	i=$init
	echo "N,Optimized(HBW),GMP">measure.csv

	bit=0
	while [ $i -le $N ]
	do
		bit=`expr $i \* 128`
		echo "$bit,=average(B$st:B$en),=average(C$st:C$en),">>measure.csv
		i=`expr $i + $interval`
		st=`expr $st + $loop`
		en=`expr $en + $loop`
	done

### measure ###


	if [ $form -eq 0 ]; then
		./test 8 1 1
	elif [ $form -eq 1 ]; then
		./test 4 2 1
	elif [ $form -eq 2 ]; then
		./test 4 1 2
	elif [ $form -eq 3 ]; then
		./test 2 4 1
	elif [ $form -eq 4 ]; then
		./test 2 2 2
	elif [ $form -eq 5 ]; then
		./test 2 1 4
	elif [ $form -eq 6 ]; then
		./test 1 8 1
	elif [ $form -eq 7 ]; then
		./test 1 4 2
	elif [ $form -eq 8 ]; then
		./test 1 2 4
	elif [ $form -eq 9 ]; then
		./test 1 1 8
	fi
	make clean all

	outer=$init

	while [ $outer -le $N ]
	do
		i=0
		while [ $i -lt $loop ]
		do
			echo "$form : $outer : $i"
			numactl --membind=1 ./mul `perl -e "print (4*$outer)"` >> measure.csv
			sleep 0.1s
			i=`expr $i + 1`
		done
		outer=`expr $outer + $interval`
		
		make clean all
	done

	mv measure.csv data/measure${form}.csv
	form=`expr $form + 1`
done
make clean


