#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


void bubble(int *vetor, int tam) {
    for (int i = 0; i < tam - 1; i++) {
        for (int j = 0; j < tam - 1 - i; j++) {
            if (vetor[j] > vetor[j + 1]) {
                int aux = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}

int main(int argc, char **argv) {

    int n_linhas = atoi(argv[1]);
    int n_colunas = atoi(argv[2]);

    int *matriz = malloc(n_linhas * n_colunas * sizeof(int));

    srand(67);

    for (int i = 0; i < n_linhas; i++) {
        for (int j = 0; j < n_colunas; j++) {
            matriz[i * n_colunas + j] = rand() % 100;
        }
    }

    double inicio = MPI_Wtime();


    for (int i = 0; i < n_linhas; i++) {
        bubble(&matriz[i * n_colunas], n_colunas);
    }

    double fim = MPI_Wtime();


    printf("Tempo sequencial: %f segundos\n", fim - inicio);

    free(matriz);

    return 0;
}