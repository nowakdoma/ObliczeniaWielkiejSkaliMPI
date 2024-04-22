#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, num_procs, znacznik;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (rank != 0) {
        MPI_Recv(&znacznik, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proces %d otrzymal znacznik %d z procesu %d\n", rank, znacznik, rank - 1);
    } else {
        znacznik = -1;
    }

    MPI_Send(&znacznik, 1, MPI_INT, (rank + 1) % num_procs, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        MPI_Recv(&znacznik, 1, MPI_INT, num_procs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proces %d otrzymal %d od procesu %d\n", rank, znacznik, num_procs - 1);
    }

    MPI_Finalize();
    return 0;
}
