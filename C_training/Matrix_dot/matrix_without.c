#include "stdlib.h"
#include "time.h"
#include "stdio.h"
#define N_SIZE 4096

void gemm_v0(int M, int N, int K, float **A, float **B, float **C)
{
    for (int i=0;i<M;++i)
    {
        for (int j=0; j<N;++j)
        {
            C[i][j] = 0;
            for (int k=0;k<K;++k)
                C[i][j] += A[i][k]*B[k][j];
        }
    }
}

void print_matrix(int M, int N, float **matr)
{
    for (int i =0;i<M;++i)
    {
        for (int j = 0;j<N;++j)
            printf("%f ", matr[i][j]);
        printf("\n");;
    }
}

void fill_matrix(int M, int N, float **matr)
{
    float number;
    for (int i=0;i<M;i++)
    {
        for (int j=0;j<N;j++)
        {
            number = (((float)rand())/((float)RAND_MAX))*10;
            matr[i][j] = number;
            //printf("%f ", matr[i][j]);
        }
    }
}

void delete(int M, float **A, float **B, float **C)
{
    for (int i = 0; i < M; i++)
    {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
}

int main()
{

    float **A = (float**)malloc(N_SIZE * sizeof(float*));
    float **B = (float**)malloc(N_SIZE * sizeof(float*));
    float **C = (float**)malloc(N_SIZE * sizeof(float*));
    
    srand((unsigned)time(NULL));
    
    for (int i = 0; i < N_SIZE; i++)
    {
        A[i] = (float*)malloc(N_SIZE * sizeof(float));
        B[i] = (float*)malloc(N_SIZE * sizeof(float));
        C[i] = (float*)malloc(N_SIZE * sizeof(float));
    }

    

    fill_matrix(N_SIZE, N_SIZE, A);
    fill_matrix(N_SIZE, N_SIZE, B);
    
    
    
    gemm_v0(N_SIZE, N_SIZE, N_SIZE, A, B, C);
    

    
    delete(N_SIZE, A, B, C);

        
    return 0;
}
