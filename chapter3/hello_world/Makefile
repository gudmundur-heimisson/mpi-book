run: hello_world
	mpirun --hostfile hostfile hello_world

hello_world:
	mpic++ -o hello_world hello_world.cc

clean:
	rm hello_world