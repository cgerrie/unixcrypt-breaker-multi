# $1 SHOUld be complete wirefile of sigma hats


for i in {0..255}
do
	echo "processing wire $i"
	../K/sigmas_to_tau < $1 $i 2>/dev/null | ./tau_table_picker 1> wires/$i.wire
done

./wires_to_tau wires
