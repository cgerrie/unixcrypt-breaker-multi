# $1 dist file
# $2 lower bound
# $3 upper bound

for i in $(seq $2 $3)
do
	echo "processing wire $i"
	./tau_table_picker < data/$i.data $1 1> wires/$i.wire
done
