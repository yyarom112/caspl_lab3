all: f

f:  addresses.o 
	gcc -g -Wall -o f addresses.o 

addresses.o: addresses.c
	gcc -g -Wall -c -o addresses.o addresses.c

clean:
	rm -f *.o
