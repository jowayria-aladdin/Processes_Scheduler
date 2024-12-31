cc= gcc -g
CC= g++ -g
make:
	$(cc) -c main.cpp
	$(CC) -o lab6 main.o
