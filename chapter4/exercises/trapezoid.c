#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

float func(float x) {
  return powf(x, 2.0);
}

float func_integral(float left, float right) {
  return (1.0/3.0) * powf(right, 3.0) - powf(left, 3.0);
}

main(int argc, char* argv[]) {
  float left_bound = 0;
  float right_bound = 10;
  float true_value = func_integral(left_bound, right_bound);
  int local_num_traps = 10;

  MPI_Init(&argc, &argv);
  int comm_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  if (comm_rank == 0) printf("Using %d processes.\n", comm_size);

  // Exercise 1, integrate x^2 from 0 to 10
  // Size of interval that gets distributed to each process
  float interval_size = (right_bound - left_bound) / comm_size;
  // Start of local process interval
  float local_left_bound = comm_rank * interval_size;
  // End of local process interval
  float local_right_bound = local_left_bound + interval_size;
  float local_left_value = func(local_left_bound);
  float local_right_value = func(local_right_bound);

  float local_total = 0.5 * (local_left_value + local_right_value);
  float local_interval_size = (local_right_bound - local_left_bound) / local_num_traps;
  for (int trap_idx = 1; trap_idx < local_num_traps; ++trap_idx) {
    float local_x = local_left_bound + trap_idx * local_interval_size;
    local_total += func(local_x);
  }
  local_total *= local_interval_size;

  if (comm_rank != 0) {
    MPI_Send(&local_total, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
  } else {
    float total = 0;
    MPI_Status status;
    total += local_total;
    for (int i = 1; i < comm_size; ++i) {
      float remote_total;
      MPI_Recv(&remote_total, 1, MPI_FLOAT, MPI_ANY_SOURCE,
        MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      total += remote_total;
    }
    printf("True value: %.3f\n", true_value);
    printf("Calculated value: %.3f\n", total);
    float relative_error = fabsf(true_value - total) / true_value;
    printf("Relative error: %.3f%%\n", 100.0 * relative_error);
  }

  MPI_Finalize();
}