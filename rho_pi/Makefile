all : rhopi_to_tausigma correct_guesses

rhopi_to_tausigma : rhopi_to_tausigma.o
	gcc -o rhopi_to_tausigma rhopi_to_tausigma.o

rhopi_to_tausigma.o : rhopi_to_tausigma.c
	gcc -c rhopi_to_tausigma.c

correct_guesses : correct_guesses.o
	gcc -o correct_guesses correct_guesses.o

correct_guesses.o : correct_guesses.c
	gcc -c correct_guesses.c

clean :
	rm rhopi_to_tausigma rhopi_to_tausigma.o \
	   correct_guesses correct_guesses.o
