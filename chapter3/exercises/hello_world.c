#include <stdio.h>
#include <string.h>
#include "mpi.h"

main(int argc, char* argv[]) {
  int my_rank;  // process rank
  int p;  // number of processes
  int source;  // sender rank
  int dest;  // receiver rank
  int tag = 0;  // messages tag
  char message[100];  // message storage
  MPI_Status status;  // receive return status

  MPI_Init(&argc, &argv);  // Start up MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);  // Find process rank
  MPI_Comm_size(MPI_COMM_WORLD, &p);  // Find number of processes

  // Exercise 1, get this compiled and running
  if (my_rank == 0) printf("Exercise 1\n");
  if (my_rank != 0) {
    // Create message
    sprintf(message, "Hello process %d!", my_rank);
    dest = 0;
    MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  } else {
    printf("Hello root process!\n");
    for (source = 1; source < p; source++) {
      MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      printf("%s\n", message);
    }
  }

  // Exercise 2, use wildcards
  if (my_rank == 0) printf("\n\nExercise 2\n");
  if (my_rank != 0) {
    sprintf(message, "Hello process %d!", my_rank);
    dest = 0;
    MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  } else {
    printf("Hello root process!\n");
    for (source = 1; source < p; source++) {
      MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, 
        MPI_COMM_WORLD, &status);
      printf("%s\n", message);
    }
  }

  // Exercise 4, send to process p-1
  if (my_rank == 0) printf("\n\nExercise 4\n");
  if (my_rank != p - 1) {
    // Create message
    sprintf(message, "Hello process %d!", my_rank);
    dest = p - 1;
    MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  } else {
    fprintf(stderr, "Hello process p-1!\n");
    for (source = 0; source < p-1; source++) {
      MPI_Recv(message, 100, MPI_CHAR, source, tag,
        MPI_COMM_WORLD, &status);
      printf("%s\n", message);
    }
  }

  MPI_Finalize();
}