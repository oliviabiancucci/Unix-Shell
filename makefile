all: myShell.o
	gcc -std=gnu99 -Wpedantic myShell.o -o myShell

myShell.o: myShell.c
	gcc -std=gnu99 -Wpedantic -c myShell.c

clean:
	rm *.o all
