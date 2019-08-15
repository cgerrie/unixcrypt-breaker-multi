#!/bin/bash

for j in {5..65..5}
do
	echo -n "$j "
	values=$(for i in $(ls $j/wires_renormalized); do echo ${i::-5}; done)
	max=$(echo -e "$values\n" | sort -n | tail -1)
	#echo -e "$(for i in $(ls 20/data); do echo ${i::-5}; done)\n" | sort -n | tail -1
	if [ $max -eq 255 ]
	then
		echo "completed"
		cat $j/number_correct_renormalized
	else
		echo "on $max/256"
	fi
done
