#include <stdio.h>
#include <mpi.h>
#include <string.h>
# include <math.h>

int main(int argc, char *argv[]) {

    int rank, numprocs, rank_wiersz, rank_kol;
    int left_neighbor, right_neighbor, top_neighbor, down_neighbor;
    int wynik_wiersz, wynik_kol;
    MPI_Init(0, 0);
    MPI_Comm kom_wiersza;
    MPI_Comm kom_kolumny;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Request reqSend_wiersz, reqRecv_wiersz;
    MPI_Request reqSend_kol, reqRecv_kol;
    MPI_Status statRecv_wiersz, statRecv_kol;


    int N = sqrt(numprocs);
    int row = rank/N; 
    int col = rank % N; 
    
    MPI_Comm_split(MPI_COMM_WORLD,row,col,&kom_wiersza);
    MPI_Comm_split(MPI_COMM_WORLD,col,row,&kom_kolumny);
    
    //w wierszu
    MPI_Comm_rank(kom_wiersza, &rank_wiersz);
    left_neighbor = (rank_wiersz - 1 + N) % N;
    right_neighbor = (rank_wiersz + 1) % N;
    MPI_Isend(&rank, 1, MPI_INT, right_neighbor, 0, kom_wiersza, &reqSend_wiersz);
    MPI_Irecv(&wynik_wiersz, 1, MPI_INT, left_neighbor, 0, kom_wiersza, &reqRecv_wiersz);
    
    //w kolumnie
    MPI_Comm_rank(kom_kolumny, &rank_kol);
    top_neighbor = (rank_kol - 1 + N) % N;
    down_neighbor = (rank_kol + 1) % N;
    MPI_Isend(&rank, 1, MPI_INT, top_neighbor, 0, kom_kolumny, &reqSend_kol);
    MPI_Irecv(&wynik_kol, 1, MPI_INT, down_neighbor, 0, kom_kolumny, &reqRecv_kol);
    
    MPI_Wait(&reqRecv_wiersz, &statRecv_wiersz);
    MPI_Wait(&reqRecv_kol, &statRecv_kol);
    
    printf("Proces %d otrzymal wiadomosc od lewego sąsiada %d i dolnego sąsiada %d\n", rank, wynik_wiersz, wynik_kol);

    MPI_Finalize();
}
