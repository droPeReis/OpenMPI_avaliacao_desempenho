#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_PEDIDO    1     
#define TAG_TRABALHO  2     
#define TAG_RESULTADO 3     
#define TAG_TERMINO   4     

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
    int rank;
    int proc_n;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    int n_linhas = atoi(argv[1]);     
    int n_colunas = atoi(argv[2]);    
    if (rank == 0) {    
        int *matriz = malloc(n_linhas * n_colunas * sizeof(int));
        int *resultado = malloc(n_linhas * n_colunas * sizeof(int));
        srand(67);
        for (int i = 0; i < n_linhas; i++) {
            for (int j = 0; j < n_colunas; j++) {
                matriz[i * n_colunas + j] = rand() % 100;
            }
        }
        int proxima_linha = 0;
        int linhas_processadas = 0;
        int trabalhadores_finalizados = 0;
        int total_trabalhadores = proc_n - 1;
        double inicio = MPI_Wtime();
        while (trabalhadores_finalizados < total_trabalhadores) {
            int mensagem;
            MPI_Recv(&mensagem,1, MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            int rank_trabalhador = status.MPI_SOURCE;
            if (status.MPI_TAG == TAG_PEDIDO) {
                if (proxima_linha < n_linhas) {
                    int indx_linha = proxima_linha;
                    
                    MPI_Send(&indx_linha,1,MPI_INT,rank_trabalhador,TAG_TRABALHO,MPI_COMM_WORLD);

                    MPI_Send(&matriz[indx_linha * n_colunas],n_colunas,MPI_INT,rank_trabalhador,TAG_TRABALHO,MPI_COMM_WORLD);
                    proxima_linha++;
                } else {
                    int fim = -1;
                    MPI_Send(&fim,1,MPI_INT,rank_trabalhador,TAG_TERMINO,MPI_COMM_WORLD);
                    trabalhadores_finalizados++;
                }
            }
            else if (status.MPI_TAG == TAG_RESULTADO) {
                int indx_mensagem = mensagem;

                MPI_Recv(&resultado[indx_mensagem * n_colunas],n_colunas,MPI_INT,rank_trabalhador,TAG_RESULTADO,MPI_COMM_WORLD,&status);
                linhas_processadas++;
            }
        }
        double fim = MPI_Wtime();
        printf("linhas processadas: %d\n", linhas_processadas);
        printf("tempo de execucao: %f segundos\n", fim - inicio);
        free(matriz);
        free(resultado);
    }
    else {
        int *vetor = malloc(n_colunas * sizeof(int));
        if (vetor == NULL) {
            printf("Processo %d: erro ao alocar vetor.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        while (1) {
            int pedido = 1;
            int indice;
            MPI_Send(&pedido,1,MPI_INT,0,TAG_PEDIDO,MPI_COMM_WORLD);
            MPI_Recv(&indice,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            if (status.MPI_TAG == TAG_TERMINO) {
                break;
            }
            MPI_Recv(vetor,n_colunas,MPI_INT,0,TAG_TRABALHO,MPI_COMM_WORLD,&status);
            bubble(vetor, n_colunas);
            MPI_Send(&indice,1,MPI_INT,0,TAG_RESULTADO,MPI_COMM_WORLD);
            MPI_Send(vetor,n_colunas,MPI_INT,0,TAG_RESULTADO,MPI_COMM_WORLD);
        }
        free(vetor);
    }
    MPI_Finalize();
    return 0;
}
