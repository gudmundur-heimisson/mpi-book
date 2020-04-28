#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

float func(float x) {
  return powf(x, 2.0);
}

float func_integral(float left, float right) {
  return (1.0/3.0) * (powf(right, 3.0) - powf(left, 3.0));
}

float trapezoid(float left_bound, float right_bound, int num_traps) {
  float interval_size = (right_bound - left_bound) / (float) num_traps;
  float right_value = func(right_bound);
  float left_value = func(left_bound);
  float total = 0.5 * (left_value + right_value);
  for (int trap_idx = 1; trap_idx < num_traps; ++trap_idx) {
    float x = left_bound + trap_idx * interval_size;
    total += func(x);
  }
  total = interval_size * total;
  return total;
}

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
  float true_value = func_integral(left_bound, right_bound);

  float interval_size = (right_bound - left_bound) / comm_size;
  float local_left_bound = left_bound + comm_rank * interval_size;
  float local_right_bound = local_left_bound + interval_size;
  float local_total = trapezoid(local_left_bound, local_right_bound,
                                num_traps);

  float total;
  MPI_Reduce(&local_total, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (comm_rank == 0) {
    printf("True value: %.3f\n", true_value);
    printf("Calculated value: %.3f\n", total);
    float relative_error = (total - true_value) / true_value;
    printf("Relative error: %.3f%%\n", 100.0 * relative_error);
  }

  MPI_Finalize();
  return 0;
}