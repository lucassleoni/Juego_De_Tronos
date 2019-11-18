CC = gcc
CFLAGS = -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0

compile: juego.c trono.c
	clear
	$(CC) $(CFLAGS) *.c -o juego -g

juego: compile
	clear
	./juego

valgrind: compile
	clear
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./juego

S: compile
	clear
	./juego S

A: compile
	clear
	./juego A

L: compile
	clear
	./juego L

E: compile
	clear
	./juego E

Q: compile
	clear
	./juego Q