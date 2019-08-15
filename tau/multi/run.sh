#!/bin/bash

for i in {5..69..5}
do
	mkdir $i 2>/dev/null
	cd $i
	../proc.sh $i &
	cd ..
	
done
