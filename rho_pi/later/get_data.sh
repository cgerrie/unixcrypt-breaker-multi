
for i in {0..68}
do
	bash get_sigmahat_i.sh $(($i+1)) > data/$i.data
	echo "testing sigma hat $i" 
	../correct_guesses sigma data/$i.data
done
