#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define mxSize 2000
#define N 2
#define NP 4

float a[mxSize][mxSize], b[mxSize][mxSize], c[mxSize][mxSize], cSekw[mxSize][mxSize];
float aSend[mxSize / N][mxSize / N], bSend[mxSize / N][mxSize / N], cLocal[mxSize / N][mxSize / N];
float aRecv[mxSize / N][mxSize / N], bRecv[mxSize / N][mxSize / N], cRecv[mxSize / N][mxSize / N];
float firstA[mxSize / N][mxSize / N], firstB[mxSize / N][mxSize / N];

void getPartOfMatrix(int x, int y, float matrix[mxSize][mxSize], float part[mxSize / N][mxSize / N]) {
    for (int i = 0; i < mxSize / N; i++) {
        for (int j = 0; j < mxSize / N; j++) {
            part[i][j] = matrix[x + i][y + j];
        }
    }
}

void savePartToMatrix(int x, int y, float matrix[mxSize][mxSize], float part[mxSize / N][mxSize / N]) {
    for (int i = 0; i < mxSize / N; i++) {
        for (int j = 0; j < mxSize / N; j++) {
            matrix[x + i][y + j] = part[i][j];
        }
    }
}


int main(int argc, char **argv) {
    FILE *plik, *plikOut;

    int rank, ncpus;
    int row, col, rowSend, colSend, rowRank, colRank, rowRecv, colRecv;
    int dataReceived = -1;
    int tag = 101;
    int koniec;
    int left, right, top, down;
	double startwtime1, startwtime2, endwtime, startSekw, endSekw;

    MPI_Status statRecv[2];
    MPI_Request reqSend[2], reqRecv[2];
    MPI_Request reqRecvA[NP*N], reqRecvB[NP*N];
	MPI_Request reqSendA[NP*N], reqSendB[NP*N];
	MPI_Comm rowComm, colComm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ncpus);

	if (ncpus != NP) {
        if (rank == 0)
            printf("wywolano obliczenia iloczynu macierzy metoda cannona na %d procesach - uruchom mpirun -np %d ./szkielet\n", ncpus, NP);
        MPI_Finalize();
        exit(0);
    }

    if (rank == 0)
        printf("obliczenia metod  Cannona dla tablicy %d x %d elementow \n", mxSize, mxSize);


    // wczytanie danych przez proces rank=0
    if (rank == 0) {
        plik = fopen("macierze.txt", "r");
        if (plik == NULL) {
            printf("Blad otwarcia pliku \"macierze.txt\"\n");
            koniec = 1;
            MPI_Bcast(&koniec, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Finalize();
            exit(0);
        } else {
            koniec = 0;
            MPI_Bcast(&koniec, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Bcast(&koniec, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (koniec) {
            MPI_Finalize();
            exit(0);
        }
    }

    if (rank == 0) {
        // odczyt macierzy A
        for (int i = 0; i < mxSize; ++i) {
            for (int j = 0; j < mxSize; ++j) {
                fscanf(plik, "%f", &a[i][j]);
            }
        }

        // Odczyt macierzy B
        for (int i = 0; i < mxSize; ++i) {
            for (int j = 0; j < mxSize; ++j) {
                fscanf(plik, "%f", &b[i][j]);
            }
        }

            if (rank == 0) startwtime1 = MPI_Wtime(); // czas w sekundach


        for (int i = 1; i < NP; i++) {
            rowSend = i / N;
            colSend = i % N;

            // rozesłanie tablicy aSend zgodnie z dystrybucją początkową tablicy A
            getPartOfMatrix(rowSend * (mxSize / N), ((colSend + rowSend) % N) * (mxSize / N), a, firstA);
            MPI_Send(firstA, (mxSize / N) * (mxSize / N), MPI_FLOAT, i, i, MPI_COMM_WORLD);

            // rozesłanie tablicy aSend zgodnie z dystrybucją początkową tablicy B
            getPartOfMatrix(((rowSend + colSend) % N) * (mxSize / N), colSend * (mxSize / N), b, firstB);
            MPI_Send(firstB, (mxSize / N) * (mxSize / N), MPI_FLOAT, i, i * 2, MPI_COMM_WORLD);
        }
        getPartOfMatrix(0, 0, a, aSend);
        getPartOfMatrix(0, 0, b, bSend);

    } else {
        MPI_Recv(aSend, (mxSize / N) * (mxSize / N), MPI_FLOAT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(bSend, (mxSize / N) * (mxSize / N), MPI_FLOAT, 0, rank * 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }


    row = rank / N;
    col = rank % N;

    // przygotowanie lokalnej  tablicy wynikowej
    for (int i = 0; i < mxSize / N; i++)
        for (int j = 0; j < mxSize / N; j++) {
            cLocal[i][j] = 0;
        }

    if (rank == 0) startwtime2 = MPI_Wtime(); // czas w sekundach

    MPI_Comm_split(MPI_COMM_WORLD, row, col, &rowComm);
    MPI_Comm_split(MPI_COMM_WORLD, col, row, &colComm);

	//obliczenia iloczynu macierzy zgodnie z algorytmem Cannona 
    for (int kk = 0; kk < N; kk++) {
        for (int i = 0; i < mxSize / N; i++) {
            for (int k = 0; k < mxSize / N; k++) {
                for (int j = 0; j < mxSize / N; j++)
                    cLocal[i][j] += aSend[i][k] * bSend[k][j];
            }
        }

        MPI_Comm_rank(rowComm, &rowRank);
        MPI_Comm_rank(colComm, &colRank);
        left = (rowRank - 1 + N) % N;
        right = (rowRank + 1) % N;
        top = (colRank - 1 + N) % N;
        down = (colRank + 1) % N;

        // Wysyłanie i odbieranie wyników
        MPI_Irecv(aRecv, mxSize * mxSize / N / N, MPI_FLOAT, right, 0, rowComm, &reqRecvA[rank+(N*kk)]);
        MPI_Irecv(bRecv, mxSize * mxSize / N / N, MPI_FLOAT, down, 0, colComm, &reqRecvB[rank+(N*kk)]);
        MPI_Isend(aSend, mxSize * mxSize / N / N, MPI_FLOAT, left, 0, rowComm, &reqSendA[rank+(N*kk)]);
        MPI_Isend(bSend, mxSize * mxSize / N / N, MPI_FLOAT, top, 0, colComm, &reqSendB[rank+(N*kk)]);

		MPI_Wait(&reqRecvA[rank+(N*kk)], MPI_STATUS_IGNORE);
		MPI_Wait(&reqRecvB[rank+(N*kk)], MPI_STATUS_IGNORE);
		MPI_Wait(&reqSendA[rank+(N*kk)], MPI_STATUS_IGNORE);
		MPI_Wait(&reqSendB[rank+(N*kk)], MPI_STATUS_IGNORE);

        for (int i = 0; i < mxSize / N; i++) {
            for (int j = 0; j < mxSize / N; j++) {
                aSend[i][j] = aRecv[i][j];
                bSend[i][j] = bRecv[i][j];
            }
        }
    }

    if (rank == 0) {

        savePartToMatrix(0, 0, c, cLocal);

        for (int i = 1; i < NP; i++) {
            rowRecv = i / N;
            colRecv = i % N;
            MPI_Recv(cRecv, (mxSize / N) * (mxSize / N), MPI_FLOAT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            savePartToMatrix(rowRecv * (mxSize / N), colRecv * (mxSize / N), c, cRecv);
        }
    } else {
        
        MPI_Send(cLocal, (mxSize / N) * (mxSize / N), MPI_FLOAT, 0, rank, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        endwtime = MPI_Wtime();
        printf("Calkowity czas przetwarzania wynosi %f sekund\n", endwtime - startwtime1);
        printf("Calkowity czas obliczen wynosi %f sekund\n", endwtime - startwtime2);
    }

    if (rank == 0) {
        // Zapis do pliku
        plikOut = fopen("wynikCanon.txt", "w");
        for (int i = 0; i < mxSize; i++) {
            for (int j = 0; j < mxSize; j++) {
                fprintf(plikOut, "%6.1f ", c[i][j]);
            }
            fprintf(plikOut, "\n");
        }
        fclose(plikOut);
    }


    if (rank == 0) {
        // obliczenia sekwencyjne mnożenia tablic CSekw=A*B
        startSekw = MPI_Wtime();
        // Mnożenie macierzy A i B, zapis wyniku do macierzy C
        for (int i = 0; i < mxSize; ++i) {
            for (int k = 0; k < mxSize; ++k) {
                for (int j = 0; j < mxSize; ++j) {
                    cSekw[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        endSekw = MPI_Wtime();

        printf("Calkowity czas obliczen sekwencyjnych wynosi %f sekund\n", endSekw - startSekw);
        printf("Przyspieszenie %5.3f \n", (endSekw - startSekw) / (endwtime - startwtime2));

    // test poprawnosci wyniku
        int wynik = 1;
        for (int k = 0; k < mxSize; k++) {
            for (int l = 0; l < mxSize; l++) {
                if (((c[k][l] / cSekw[k][l]) >= 1.1) || ((c[k][l] / cSekw[k][l]) <= 0.9)){
					printf("Nieprawidlowy wynik obliczen.\n");
					wynik =0;
					break;
				}
                    
            }
			if(wynik == 0) break;
        }

        if (wynik == 1) {
            printf("Prawidlowy wynik obliczen.\n");
        }
    }

    MPI_Finalize();
    return 0;
}
