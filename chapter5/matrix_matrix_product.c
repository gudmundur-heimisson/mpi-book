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

void serial_matrix_vector_product(int num_rows, int num_cols, 
                                  float matrix[num_rows][num_cols],
                                  float vector[num_cols], 
                                  float result[num_rows]) {
  for (int row_idx = 0; row_idx < num_rows; ++row_idx) {
    result[row_idx] = 0;
    for (int col_idx = 0; col_idx < num_cols; ++col_idx) {
      result[row_idx] += vector[col_idx] * matrix[row_idx][col_idx];
    }
  }
}

void parallel_matrix_matrix_product(
  int num_rows_1, int num_cols_1,
  int num_rows_2, int num_cols_2,
  int local_num_rows_1, float local_matrix_1[local_num_rows_1][num_cols_1],
  int local_num_rows_2, float local_matrix_2[local_num_rows_2][num_cols_2],
  float local_result[local_num_rows_1][num_cols_2]) {
  // Iterate through the columns of matrix 2
  for (int col_idx = 0; col_idx < num_cols_2; ++col_idx) {
    float local_col_2[local_num_rows_2];
    // Move local column data into a buffer for sending
    for (int row_idx = 0; row_idx < local_num_rows_2; ++row_idx) {
      local_col_2[row_idx] = local_matrix_2[row_idx][col_idx];
    }
    // Gather column of matrix 2 to all processes
    float col_2[num_rows_2];
    MPI_Allgather(local_col_2, local_num_rows_2, MPI_FLOAT,
                  col_2, local_num_rows_2, MPI_FLOAT,
                  MPI_COMM_WORLD);
    // Compute row column product into column data buffer
    float local_result_col[local_num_rows_1];
    serial_matrix_vector_product(local_num_rows_1, num_cols_1, local_matrix_1,
                                 col_2, local_result_col);
    // Move column data into result matrix
    for (int row_idx = 0; row_idx < local_num_rows_1; ++row_idx) {
      local_result[row_idx][col_idx] = local_result_col[row_idx];
    }
  }
}

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  int comm_rank, comm_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  // Allocate matrices
  int num_rows_1 = 3 * comm_size;
  int num_cols_1 = 2 * comm_size;
  float matrix_1[num_rows_1][num_cols_1];

  int num_rows_2 = 2 * comm_size;
  int num_cols_2 = 4 * comm_size;
  float matrix_2[num_rows_2][num_cols_2];

  // Initialize matrices
  if (comm_rank == 0) {
    printf("(%d x %d) * (%d x %d)\n", num_rows_1, num_cols_1,
           num_rows_2, num_cols_2);
    for (int row_idx = 0; row_idx < num_rows_1; ++row_idx) {
      for (int col_idx = 0; col_idx < num_cols_1; ++col_idx) {
        matrix_1[row_idx][col_idx] = 1;
      }
    }
    for (int row_idx = 0; row_idx < num_rows_2; ++row_idx) {
      for (int col_idx = 0; col_idx < num_cols_2; ++col_idx) {
        matrix_2[row_idx][col_idx] = 1;
      }
    }
  }

  // Scatter matrices
  int local_num_rows_1 = num_rows_1 / comm_size;
  int local_matrix_1_size = local_num_rows_1 * num_cols_1;
  float local_matrix_1[local_num_rows_1][num_cols_1];
  MPI_Scatter(matrix_1, local_matrix_1_size, MPI_FLOAT,
              local_matrix_1, local_matrix_1_size, MPI_FLOAT,
              0, MPI_COMM_WORLD);

  int local_num_rows_2 = num_rows_2 / comm_size;
  int local_matrix_2_size = local_num_rows_2 * num_cols_2;
  float local_matrix_2[local_num_rows_2][num_cols_2];
  MPI_Scatter(matrix_2, local_matrix_2_size, MPI_FLOAT,
              local_matrix_2, local_matrix_2_size, MPI_FLOAT,
              0, MPI_COMM_WORLD);

  float local_result[local_num_rows_1][num_cols_2];
  int local_result_size = local_num_rows_1 * num_cols_2;
  parallel_matrix_matrix_product(
    num_rows_1, num_cols_1,
    num_rows_2, num_cols_2,
    local_num_rows_1, local_matrix_1,
    local_num_rows_2, local_matrix_2,
    local_result);

  float result[num_rows_1][num_cols_2];
  MPI_Gather(local_result, local_result_size, MPI_FLOAT,
             result, local_result_size, MPI_FLOAT,
             0, MPI_COMM_WORLD);

  if (comm_rank == 0) print_matrix(num_rows_1, num_cols_2, result);

  MPI_Finalize();
}