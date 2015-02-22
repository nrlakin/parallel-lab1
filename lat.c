#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int sz, myid, ack, i;
  double send;
  double total_lat = 0;
  int send_id = 0, rec_id = 1, tag = 1, packet_size = 1;
  // number of latency runs
  int runs = 100;

  // initialize mpi
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  // cheeck size of world
  if (sz < 2) {
    MPI_Finalize();
    return 0;
  }

  // initialize runs for sender / receiver
  for (i = 0; i < runs; i++) {
    if (myid == send_id) {
      // sender sends start time
      send = MPI_Wtime();
      MPI_Send(&send, packet_size, MPI_DOUBLE, rec_id, tag, MPI_COMM_WORLD);

      // wait for ack
      MPI_Recv(&ack, 1, MPI_INT, rec_id, tag, MPI_COMM_WORLD, &status);
    } else if (myid == rec_id) {
      // receive start time
      MPI_Recv(&send, packet_size, MPI_DOUBLE, send_id, tag, MPI_COMM_WORLD, &status);
      // record end time
      double receive = MPI_Wtime();
      // calculate latency (seconds per byte)
      double lat = (receive-send)/sizeof(send);
      total_lat += lat;
      // printf("one-way trip - Start Time: %.11f, End Time: %.11f \n",
      //   send, receive);
      // printf("one-way trip - Time Elapsed: %.11f, Latency: %.11f \n",
      //   receive-send, lat);

      // send ack to block before next run
      ack = 1;
      MPI_Send(&ack, 1, MPI_INT, send_id, tag, MPI_COMM_WORLD);
    }
  }

  // find average latency for all trials
  if (total_lat) {
    printf("Average Latency: %.11f \n",
      (total_lat)/runs);
  }

  MPI_Finalize();
  return 0;
}


