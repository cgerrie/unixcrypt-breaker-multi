#!/bin/bash

# $1 dist file
# $2 lower bound
# $3 upper bound

for i in $(seq $2 $3)
do
	echo "processing wire $i"
	./tau_table_picker < data_renormalized/$i.data $1 1> wires/$i.wire
done
./wires_to_tau wires
