#include <stdio.h>
#include <stdlib.h>

#define N 2000

int main(int argc, char **argv)
{FILE *plik;
int i,j;

   plik = fopen("macierze.txt","w");
   if (plik == NULL) 
   {
      printf("Blad otwarcia pliku \"liczby.txt\"\n");
      exit(0);
   }
 
    for (i = 0; i < N * 2; i++)
    {
        for (j = 0; j < N; j++)
        {
            fprintf(plik, "%0.1f ", (float)i+1);

        }
        fprintf(plik, "\n");
    }
   fclose(plik);
   return 0;
}
