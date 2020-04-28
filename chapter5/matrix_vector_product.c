#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

void print_matrix(int num_rows, int num_cols, float matrix[num_rows][num_cols]) {
  printf("Matrix:\n");
  for (int row_idx = 0; row_idx < num_rows; ++row_idx) {
    for (int col_idx = 0; col_idx < num_cols; ++col_idx) {
      printf("%.1f\t", matrix[row_idx][col_idx]);
    }
    printf("\n");
  }
}

void print_vector(int size, float vector[size]) {
  printf("Vector: ");
  for (int i = 0; i < size; ++i) {
    printf("%.1f ", vector[i]);
  }
  printf("\n");
}

void serial_product(int num_rows, int num_cols, float matrix[num_rows][num_cols],
                    float vector[num_cols], float result[num_rows]) {
  for (int row_idx = 0; row_idx < num_rows; ++row_idx) {
    result[row_idx] = 0;
    for (int col_idx = 0; col_idx < num_cols; ++col_idx) {
      result[row_idx] += vector[col_idx] * matrix[row_idx][col_idx];
    }
  }
}

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  int comm_rank, comm_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  int num_cols = 2*comm_size;
  int num_rows = 3*comm_size;
  float matrix[num_rows][num_cols];
  float vector[num_cols];
  float true_result[num_rows];

  // Allocate initial matrix and vector.
  if (comm_rank == 0) {

    for (int col_idx = 0; col_idx < num_cols; ++col_idx) {
      vector[col_idx] = col_idx + 1;
      for (int row_idx = 0; row_idx < num_rows; ++row_idx) {
        matrix[row_idx][col_idx] = (col_idx+1)*(row_idx+1);
      }
    }
    serial_product(num_rows, num_cols, matrix, vector, true_result);
    print_vector(num_cols, vector);
    print_matrix(num_rows, num_cols, matrix);
    print_vector(num_rows, true_result);
  }

  // Scatter matrix
  int local_num_rows = num_rows / comm_size;
  float local_matrix[local_num_rows][num_cols];
  int local_num_matrix_elements = local_num_rows * num_cols;
  MPI_Scatter(matrix, local_num_matrix_elements, MPI_FLOAT, 
              local_matrix, local_num_matrix_elements, MPI_FLOAT,
              0, MPI_COMM_WORLD);

  // Scatter vector
  int local_vector_size = num_cols / comm_size;
  float local_vector[local_vector_size];
  MPI_Scatter(vector, local_vector_size, MPI_FLOAT,
              local_vector, local_vector_size, MPI_FLOAT,
              0, MPI_COMM_WORLD);

  // Gather vector onto each process
  MPI_Allgather(local_vector, local_vector_size, MPI_FLOAT,
                vector, num_cols, MPI_FLOAT,
                MPI_COMM_WORLD);

  // Compute local result
  float local_result[local_num_rows];
  serial_product(local_num_rows, num_cols, local_matrix, vector, local_result);

  // Gather final result
  float result[num_rows];
  MPI_Gather(local_result, local_num_rows, MPI_FLOAT,
             result, local_num_rows, MPI_FLOAT,
             0, MPI_COMM_WORLD);

  if (comm_rank == 0) print_vector(num_rows, result);

  MPI_Finalize();
  return 0;
}