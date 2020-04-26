run: trapezoid
	mpirun --hostfile hostfile -np 16 trapezoid

trapezoid: trapezoid.c
	mpicc -lm -o trapezoid trapezoid.c

clean:
	rm -f trapezoid