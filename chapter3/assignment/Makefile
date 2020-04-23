run: modulo
	mpirun --hostfile hostfile modulo

modulo: modulo.c
	mpicc -o modulo modulo.c

clean:
	rm modulo
