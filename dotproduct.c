#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

// Must be less than max unsigned int!
#define VECTOR_LENGTH 100000000
//#define VECTOR_LENGTH 5

float norm(float* vec, int len) {
  int i;
  float result = 0;
  for (i=0; i<len; i++) {
    result += (*vec)*(*vec);
    vec++;
  }
  return result;
}

int main(int argc, char **argv) {
  const unsigned int VectorLength = VECTOR_LENGTH; // evaluate exp here
  int n_proc, rank, i, chunk_size, msg_len, tag=1;
  MPI_Status status;
  float *vector;
  float result;

  MPI_Init(&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &n_proc);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  // Initialize vector (process 0 only)

  if (rank == 0) {
    if (NULL == (vector = (float*)malloc(sizeof(float) * VectorLength))) {
      printf ("malloc failed on process 0...");
    };

    srand(time(NULL));
    for (i = 0; i < VectorLength; i++) {
      vector[i] = (float)rand() / RAND_MAX;
    }

    printf("Generated vector of length %d.\n", i);

  // Chop up vector and send chunks to processes (0 only).
    chunk_size = VectorLength / n_proc;

    for (i = 1; i < n_proc; i++) {
      msg_len = chunk_size;
      if (i == n_proc-1) {
        msg_len += VectorLength % n_proc;
      }
      MPI_Send(&vector[i*chunk_size], msg_len, MPI_FLOAT,
            i, tag, MPI_COMM_WORLD);
      printf("Process %d sent %d entries to Process %d.\n", rank, msg_len, i);
    }
  }   // end 'if rank==0'

  // Calc dot product of subvector and send result
  if (rank == 0) {
    float sub_result;
    printf("Process %d processing %d entries...\n", rank, chunk_size);
    result = norm(vector, chunk_size);
    printf("Process %d got %f.\n", rank, result);
    free(vector);
    for (i = 1; i < n_proc; i++) {
      MPI_Recv(&sub_result, 1, MPI_FLOAT, i, tag, MPI_COMM_WORLD, &status);
      result += sub_result;
      printf("Got %f from Process %d.\n", sub_result, i);
    }
    printf("Dot product = %f\n", result);

  } else {
    MPI_Probe(0, tag, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_FLOAT, &msg_len);
    if (NULL == (vector = (float*) malloc(sizeof(float) * msg_len))) {
      printf("malloc failed on process %d...", rank);
    };
    printf("Process %d processing %d entries...\n", rank, msg_len);
    MPI_Recv(vector, msg_len, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);
    result = norm(vector, msg_len);
    printf("Process %d got %f.\n", rank, result);
    MPI_Send(&result, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
    free(vector);
  }

  MPI_Finalize();

  exit(0);
}
