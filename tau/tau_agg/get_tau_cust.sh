# $1 SHOUld be complete wirefile of sigma hats
# $2 is distribution file
# $3 is lower bound
# $4 is upper bound

for i in $(seq $3 $4)
do
	echo "processing wire $i"
	../K/sigmas_to_tau < $1 $i 2>/dev/null | ./tau_table_picker $2 1> wires/$i.wire
done

./wires_to_tau wires
