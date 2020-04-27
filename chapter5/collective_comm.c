#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

void get_parameters(float* left_bound, float* right_bound, int* num_traps,
                    int comm_rank, int comm_size) {
  int source = 0;
  int tag = 0;
  if (comm_rank == source) {
    printf("Enter left bound:\n");
    scanf("%f", left_bound);
    printf("Enter right bound:\n");
    scanf("%f", right_bound);
    printf("Enter number of trapezoids for each process:\n");
    scanf("%d", num_traps);
  }
  MPI_Bcast(left_bound, 1, MPI_FLOAT, source, MPI_COMM_WORLD);
  MPI_Bcast(right_bound, 1, MPI_FLOAT, source, MPI_COMM_WORLD);
  MPI_Bcast(num_traps, 1, MPI_INT, source, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  int comm_rank;
  int comm_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  if (comm_rank == 0) printf("Using %d processes.\n", comm_size);
  
  float left_bound;
  float right_bound;
  int num_traps;
  get_parameters(&left_bound, &right_bound, &num_traps, comm_rank, comm_size);
  printf("Process %d, left bound: %f, right bound: %f, segments: %d\n",
         comm_rank, left_bound, right_bound, num_traps);

  MPI_Finalize();
  return 0;
}