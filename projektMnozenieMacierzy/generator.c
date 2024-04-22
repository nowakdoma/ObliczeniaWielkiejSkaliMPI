#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{FILE *plik;
int i,j;

   plik = fopen("macierze.txt","w");
   if (plik == NULL) 
   {
      printf("Blad otwarcia pliku \"liczby.txt\"\n");
      exit(0);
   }
 
    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 16; j++)
        {
            fprintf(plik, "%0.1f ", (float)i+1);

        }
        fprintf(plik, "\n");
    }
   fclose(plik);
   return 0;
}