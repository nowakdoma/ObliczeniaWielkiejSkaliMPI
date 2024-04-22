# Generator

Generuje macierze A i B do pliku ```macierze.txt```. W kodzie rozmiar macierzy ustawiony jest na 2000, aby zmienić należy zmienić wartość N.

## Kompilacja

```gcc generator.c -o generator```

## Uruchomienie

```./generator```

# Szkielet

Program wykonuje mnożenie macierzy metodą Canona i sekwencyjnie, porównuje czasy ich działania oraz sprawdza poprawność wyniku niesekwencyjnego.

Aby zmienić:
  * rozmiar macierzy (ustawiony w kodzie na 2000), należy zmienić wartość mxSize.
  * liczbę procesów wykorzystywanych do sortowania (ustawioną na 4), należy zmienić wartość NP oraz ustawić N na pierwiastek kwadratowy z NP.

## Kompilacja

```mpicc szkielet.c -o szkielet -O3```

## Uruchomienie

```mpirun -np 4 ./szkielet```
