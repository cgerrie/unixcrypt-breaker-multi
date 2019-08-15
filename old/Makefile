all : gen_std_blocks get_actual_wires

gen_std_blocks : gen_std_blocks.o
	gcc -o gen_std_blocks gen_std_blocks.o

gen_std_blocks.o : gen_std_blocks.c
	gcc -c gen_std_blocks.c

get_actual_wires : get_actual_wires.o
	gcc -o get_actual_wires get_actual_wires.o

get_actual_wires.o : get_actual_wires.c
	gcc -c get_actual_wires.c

clean :
	rm gen_std_blocks gen_std_blocks.o \
	   get_actual_wires get_actual_wires.o \
