#!/bin/sh

init=16

interval=16
i=$init
N=32

### initialize ###

form=0
form_end=10
name=811
bit=896

while [ $form -lt $form_end ]
do
	
### measure ###

	echo $form

	if [ $form -eq 0 ]; then
		./test 8 1 1
		name=811
	elif [ $form -eq 1 ]; then
		./test 4 2 1
		name=421
	elif [ $form -eq 2 ]; then
		./test 4 1 2
		name=412
	elif [ $form -eq 3 ]; then
		./test 2 4 1
		name=241
	elif [ $form -eq 4 ]; then
		./test 2 2 2
		name=222
	elif [ $form -eq 5 ]; then
		./test 2 1 4
		name=214
	elif [ $form -eq 6 ]; then
		./test 1 8 1
		name=181
	elif [ $form -eq 7 ]; then
		./test 1 4 2
		name=142
	elif [ $form -eq 8 ]; then
		./test 1 2 4
		name=124
	elif [ $form -eq 9 ]; then
		./test 1 1 8
		name=118
	fi
	
	./makes icc_${bit}_${name}.s

	mv icc_${bit}_${name}.s asm/${bit}bit/

	form=`expr $form + 1`
done


