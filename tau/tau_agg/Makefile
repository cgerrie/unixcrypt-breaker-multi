all : tau_table_picker wires_to_tau dist_renormalizer


tau_table_picker : tau_table_picker.o
	gcc -o tau_table_picker tau_table_picker.o -lm -lgmp
tau_table_picker.o : tau_table_picker.c
	gcc -c tau_table_picker.c


wires_to_tau : wires_to_tau.o
	gcc -o wires_to_tau wires_to_tau.o
wires_to_tau.o : wires_to_tau.c
	gcc -c wires_to_tau.c


dist_renormalizer : dist_renormalizer.o
	gcc -o dist_renormalizer dist_renormalizer.o
dist_renormalizer.o : dist_renormalizer.c
	gcc -c dist_renormalizer.c

clean :
	rm tau_table_picker tau_table_picker.o \
	   wires_to_tau wires_to_tau.o \
	   dist_renormalizer dist_renormalizer.o
