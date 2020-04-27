run_collective_comm: collective_comm
	mpirun --hostfile hostfile -np 16 collective_comm

collective_comm: collective_comm.c
	mpicc -o collective_comm collective_comm.c

clean:
	rm -f collective_comm