#!/bin/bash

echo -n > results
for i in {5..65..5}
do
	echo -ne "$i\t" >> results
done
echo >> results
#echo $(for i in {5..65..5}; do x=$(cat $i/number_correct); echo -ne "${x::-8}\t"; done) >> results
for i in {5..65..5}
do
	x=$(cat $i/number_correct)
	echo -ne "${x::-8}\t" >> results
done
echo >> results
cat results
