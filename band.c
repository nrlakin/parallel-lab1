#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int sz, myid, packet_size, ack, i;
  int send_id = 0, rec_id = 1, tag = 1;

  // granularity of messages/sizes to send
  int packet_step = 10000;
  int max_size = 10000000;
  int runs = 5;

  // init mpi
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Status status;

  // check min size
  if (sz < 2) {
    MPI_Finalize();
    return 0;
  }

  // for each message size of interest (number of packets)
  for (packet_size = packet_step; packet_size <= max_size; packet_size+=packet_step) {
    // initialize space for message in memory
    double *send = (double *)malloc(packet_size * sizeof(double));
    if (myid == send_id) {
      // build message
      int j;
      for (j = 1; j < packet_size; j++) {
        send[j] = 0.0;
      }
      // run bandwidth tests
      for (i = 0; i < runs; i++) {
        // initialize start time for each message
        send[0] = MPI_Wtime();
        MPI_Send(send, packet_size, MPI_DOUBLE, rec_id, tag, MPI_COMM_WORLD);
        // wait for ack to proceed
        MPI_Recv(&ack, 1, MPI_INT, rec_id, tag, MPI_COMM_WORLD, &status);
      }
      free(send);
    } else if (myid == rec_id) {
      double total_bw = 0;
      for (i = 0; i < runs; i++) {
        MPI_Recv(send, packet_size, MPI_DOUBLE, send_id, tag, MPI_COMM_WORLD, &status);
        double end = MPI_Wtime();
        // calculate receive time
        double bw = (sizeof(double) * packet_size) / (end - send[0]);
        total_bw += bw;

        ack = 1;
        MPI_Send(&ack, 1, MPI_INT, send_id, tag, MPI_COMM_WORLD);
      }
      // calculate average bandwidth
      printf("Runs: %i, Packet Size: %.11i, Bandwidth: %.11f \n",
       runs, packet_size, total_bw/runs);
      free(send);
    }
  }

  MPI_Finalize();
  return 0;
}
