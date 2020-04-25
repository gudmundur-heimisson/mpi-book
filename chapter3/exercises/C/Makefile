run: hello_world
	mpirun --hostfile hostfile hello_world

hello_world: hello_world.c
	mpicc -o hello_world hello_world.c

clean:
	rm hello_world
