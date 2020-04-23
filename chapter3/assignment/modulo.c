// A program that sends a message from process i to process (i+1) % p

#include <stdio.h>
#include <string.h>
#include "mpi.h"

main(int argc, char* argv[]) {
  int my_rank;  // process rank
  int p;  // number of processes
  int source;  // sender rank
  int dest;  // receiver rank
  int tag = 0;  // messages tag
  char outgoing[100];  // outgoing message storage
  char incoming[100]; // incoming message storage
  MPI_Status status;  // receive return status

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p); 

  dest = (my_rank + 1) % p;
  source = my_rank == 0 ? p - 1 : my_rank - 1;
  sprintf(outgoing, "Hello to process %d from process %d", dest, my_rank);

  // Even processes send first
  if (my_rank % 2 == 0) {
    MPI_Send(outgoing, strlen(outgoing) + 1, MPI_CHAR, dest, tag,
      MPI_COMM_WORLD);
    MPI_Recv(incoming, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
  } else {
    MPI_Recv(incoming, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
    MPI_Send(outgoing, strlen(outgoing) + 1, MPI_CHAR, dest, tag,
      MPI_COMM_WORLD);
  }
  printf("%s\n", incoming);

  MPI_Finalize();
}