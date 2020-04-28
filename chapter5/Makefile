run_matrix_vector_product: matrix_vector_product
	mpirun --hostfile hostfile matrix_vector_product

run_collective_comm: collective_comm
	mpirun --hostfile hostfile collective_comm

matrix_vector_product: matrix_vector_product.c
	mpicc -lm -o matrix_vector_product matrix_vector_product.c

collective_comm: collective_comm.c
	mpicc -lm -o collective_comm collective_comm.c

clean:
	rm -f collective_comm matrix_vector_product