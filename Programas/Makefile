all: cache.o
	gcc cache.o simbasica.c -o simbasica -Wall
	gcc cache.o simsplit.c -o simsplit -Wall
	gcc cache.o simniveis.c -o simniveis -Wall
	mv simbasica simsplit simniveis ../Experimentos
	rm -rf *.o

cache.o: cache.h
	gcc -c cache.c
