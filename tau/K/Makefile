sigmas_to_tau : sigmas_to_tau.o # list.o 
	gcc -o sigmas_to_tau sigmas_to_tau.o

sigmas_to_tau.o : sigmas_to_tau.c # list.o list.h
	gcc -c sigmas_to_tau.c

list.o : list.c list.h
	gcc -c list.c

clean :
	rm sigmas_to_tau sigmas_to_tau.o \
	   list.o
