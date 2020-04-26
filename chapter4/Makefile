run_simp: simpson
	mpirun --hostfile hostfile -np 16 simpson

run_trap: trapezoid
	mpirun --hostfile hostfile -np 16 trapezoid

simpson: simpson.c
	mpicc -lm -o simpson simpson.c

trapezoid: trapezoid.c
	mpicc -lm -o trapezoid trapezoid.c

clean:
	rm -f trapezoid simpson