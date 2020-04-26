run: modulo
	mpirun --hostfile hostfile modulo

modulo: modulo.c
	mpicc -o modulo modulo.c

run: hello_world
	mpirun --hostfile hostfile hello_world

run_cpp: hello_world_cpp
	mpirun --hostfile hostfile hello_world_cpp

hello_world: hello_world.c
	mpicc -o hello_world hello_world.c

hello_world_cpp: hello_world.cpp
	mpic++ -lboost_mpi -lboost_serialization -L/usr/lib -I/usr/include -o hello_world_cpp hello_world.cpp 

clean:
	rm -f hello_world hello_world_cpp modulo
