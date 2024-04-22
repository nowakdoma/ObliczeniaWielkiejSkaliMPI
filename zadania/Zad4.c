#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 5

int main(int argc, char *argv[]) {
    int rank, np, w;
    int size = N * N;
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm kom_wiersza, kom_kolumny;

    int a[] = {1, 7, 2, 5, 6};
    int wynik[N];

    int i = rank / N;
    int j = rank % N;
    int index = 0;

    if ((a[i] > a[j]) || (a[i] == a[j] && (i > j))) {
        w = 1;
    } else {
        w = 0; 
    }

    MPI_Comm_split(MPI_COMM_WORLD, i, j, &kom_wiersza); 
    MPI_Reduce(&w, &index, 1, MPI_INT, MPI_SUM, 0, kom_wiersza);
    MPI_Comm_split(MPI_COMM_WORLD, j, index, &kom_kolumny);

    if (j == 0) {
        MPI_Gather(&a[i], 1, MPI_INT, wynik, 1, MPI_INT, 0, kom_kolumny);
        if (rank == 0) {
            for (int i = 0; i < N; i++)
                printf("%d ", wynik[i]);
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
