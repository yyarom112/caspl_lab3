all: toy_printf

toy_printf:  main[1].o toy_printf.o 
	gcc -g -Wall -o toy_printf main[1].o toy_printf.o

main[1].o: main[1].c
	gcc -g -Wall -c -o main[1].o main[1].c
toy_printf.o: toy_printf.c
	gcc -g -Wall -c -o toy_printf.o toy_printf.c


clean:
	rm -f *.o
