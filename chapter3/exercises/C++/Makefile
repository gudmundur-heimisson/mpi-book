run: hello_world
	mpirun --hostfile hostfile hello_world

hello_world: hello_world.cpp
	mpic++ -lboost_mpi -lboost_serialization -L/usr/lib -I/usr/include -o hello_world hello_world.cpp 

clean:
	rm hello_world
