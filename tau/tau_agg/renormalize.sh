#!/bin/bash

for i in {0..255}
do
	echo "processing $i"
	./dist_renormalizer < data/$i.data 2> /dev/null 1> data_renormalized/$i.data
done
