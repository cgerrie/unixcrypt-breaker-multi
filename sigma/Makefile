#all : conjs_to_sigma get_conjs true_sigma

#conjs_to_sigma : conjs_to_sigma.c
#	gcc conjs_to_sigma.c -o conjs_to_sigma

#get_conjs : get_conjs.c
#	gcc get_conjs.c -o get_conjs

true_sigma : true_sigma.o
	gcc -o true_sigma true_sigma.o

true_sigma.o : true_sigma.c
	gcc -c true_sigma.c

clean : 
	rm true_sigma true_sigma.o
