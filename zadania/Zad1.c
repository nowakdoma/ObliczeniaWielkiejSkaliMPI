#include <stdio.h>
#include <mpi.h>
#include <string.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Status status;
    char message[100];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank != 0) {
        snprintf(message, 100, "Pozdrowienia od procesu %d z %d\n", rank, size);
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        for (int i = 1; i < size; i++) {
            MPI_Recv(message, 100, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
            printf("%s\n", message);
        }
    }

    MPI_Finalize();
    return 0;
}
