#!/bin/bash

# gen wirefile of desired length
#head -n $(echo $((35*$1))) < ../../../../wires/wires_complete.txt > wire_file
#wirefile="./wire_file"

# gen dist file
../../dist/get_binom_dist $1 > dist_file
distfile="./dist_file"

# get data
#mkdir data 2>/dev/null
#for i in {0..255}
#do
#	../../K/sigmas_to_tau < $wirefile $i 2>/dev/null > data/$i.data
#done
#echo "done collecting data for $1"

# process data
#mkdir wires 2>/dev/null
#for i in {0..255}
#do
#	../../tau_agg/tau_table_picker < data/$i.data $distfile 1> wires/$i.wire 2>/dev/null
#done
#../../tau_agg/wires_to_tau wires >/dev/null
#echo "done processing $1"

# generate renormalized data
#mkdir data_renormalized 2>/dev/null
#for i in {0..255}
#do
#	../../tau_agg/dist_renormalizer < data/$i.data 2> /dev/null 1> data_renormalized/$i.data
#done

# process renormalized data
#mkdir wires_renormalized 2>/dev/null
#for i in {0..255}
#do
#	../../tau_agg/tau_table_picker < data_renormalized/$i.data $distfile 2>/dev/null 1> wires_renormalized/$i.wire
#done
#../../tau_agg/wires_to_tau wires_renormalized
#echo "done processing renormalized $1"

# find difference
diff wires/tau ../tau.real > diff

# count error
../../../rho_pi/correct_guesses wires/tau ../tau.real > number_correct

# find differences and error renormalized 
diff wires_renormalized/tau ../tau.real > diff_renormalized
../../../rho_pi/correct_guesses wires_renormalize/tau ../tau.real > number_correct_renormalized

# report
echo "done analyzing $1"
cat number_correct
cat number_correct_renormalized
date
