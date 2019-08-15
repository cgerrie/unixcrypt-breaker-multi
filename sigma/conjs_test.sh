#!/bin/bash
# $1 conjs file
# $2 real sigma file

for i in {1..69}
do
	head -n $(( $i * 33 )) $1 | tail -32 > tmp
	echo -n "$i "
	../rho_pi/correct_guesses tmp $2
done
