#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

float func(float x) {
  return powf(x, 2.0);
}

float func_integral(float left, float right) {
  return (1.0 / 3.0) * (powf(right, 3.0) - powf(left, 3.0));
}

float simpson(float left_bound, float right_bound, int num_segments) {
  float interval_size = (right_bound - left_bound) / (float) num_segments;
  float right_value = func(right_bound);
  float left_value = func(left_bound);
  float total = 0.5 * (right_value + left_value);
  for (int segment_idx = 1; segment_idx < num_segments; ++segment_idx) {
    float x = left_bound + segment_idx * interval_size;
    float value = func(x);
    total += segment_idx % 2 == 0 ? value : 2 * value;
  }
  total *= (2.0 / 3.0) * interval_size;
  return total;
}

void get_parameters(float* left_bound, float* right_bound, int* num_segments,
                    int comm_rank, int comm_size) {
  int source = 0;
  int tag = 0;
  if (comm_rank == source) {
    printf("Enter left bound:\n");
    scanf("%f", left_bound);
    printf("Enter right bound:\n");
    scanf("%f", right_bound);
    printf("Enter number of segments for each process:\n");
    scanf("%d", num_segments);
    *num_segments = *num_segments % 2 == 0 ? *num_segments : *num_segments + 1;
    for (int proc_idx = 1; proc_idx < comm_size; ++proc_idx) {
      MPI_Send(left_bound, 1, MPI_FLOAT, proc_idx, tag, MPI_COMM_WORLD);
      MPI_Send(right_bound, 1, MPI_FLOAT, proc_idx, tag, MPI_COMM_WORLD);
      MPI_Send(num_segments, 1, MPI_INT, proc_idx, tag, MPI_COMM_WORLD);
    }
  } else {
      MPI_Status status;
      MPI_Recv(left_bound, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
      MPI_Recv(right_bound, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
      MPI_Recv(num_segments, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
  }
}

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  int comm_rank, comm_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  float left_bound;
  float right_bound;
  int num_segments;
  get_parameters(&left_bound, &right_bound, &num_segments,
    comm_rank, comm_size);
  float true_value = func_integral(left_bound, right_bound);

  float interval_size = (right_bound - left_bound) / comm_size;
  float local_left_bound = left_bound + comm_rank * interval_size;
  float local_right_bound = local_left_bound + interval_size;
  float local_total = simpson(local_left_bound, local_right_bound, 
                              num_segments);

  if (comm_rank != 0) {
    MPI_Send(&local_total, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
  } else {
    float total = local_total;
    for (int i = 1; i < comm_size; ++i) {
      MPI_Status status;
      float remote_total;
      MPI_Recv(&remote_total, 1, MPI_FLOAT, MPI_ANY_SOURCE,
        MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      total += remote_total;
    }
    printf("True value: %.3f\n", true_value);
    printf("Calculated value: %.3f\n", total);
    float relative_error = (total - true_value) / true_value;
    printf("Relative error: %.3f%%\n", 100.0 * relative_error);
  }

  MPI_Finalize();
  return 0;
}