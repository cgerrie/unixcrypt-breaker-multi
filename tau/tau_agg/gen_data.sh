# $1 wirefile
# $2 lower bound
# $3 upper bound

for i in $(seq $2 $3)
do
	echo "processing wire $i"
	../K/sigmas_to_tau < $1 $i 2>/dev/null > data/$i.data
done
