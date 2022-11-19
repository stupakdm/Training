#include "stdlib.h"
#include "stdio.h"
//#define N_SIZE 65536 //4096 8192 16384 32768
#define WIN_SIZE 8
#define MULT_SIZE 1



void print_matrix(int M, int N, float *matr)
{
    for (int i =0;i<M;++i)
    {
        for (int j = 0;j<N;++j)
            printf("%f ", matr[i*N+j]);
        printf("\n");
    }
}


void delete_matrix(int M, float *matr)
{
    free(matr);
}

void fill_matrix(int M, int N, float *matr)
{
    for (int i =0;i<N*N;i+=WIN_SIZE*WIN_SIZE)
    {
        for (int j =0;j<WIN_SIZE*WIN_SIZE; j++)
        {
            matr[j+i] = j;
        }
    }
}


float* max_pooling2d(int M, long  int N, float *matr)
{
    float *pooling = (float*)malloc(sizeof(float)*N*N/(WIN_SIZE*WIN_SIZE));
    for (int i=0;i<N;i+=WIN_SIZE)
    {
        for (int j=0;j<N;j+=WIN_SIZE)
        {
            float the_max = matr[i*N+j];
            for (int d=0;d<WIN_SIZE;d++)
            {
                for (int k=0;k<WIN_SIZE;k++)
                {
                    if (matr[i*N+j+d*N+k] > the_max)
                        the_max = matr[i*N+j+d*N+k];
                }
            }
            pooling[(i/WIN_SIZE)*WIN_SIZE*WIN_SIZE+j/WIN_SIZE] = the_max;
        }
    }

    return pooling;
}

float* max_pooling1d(int M, long int N, float *matr)
{
    float *pooling = (float*)malloc(sizeof(float)*N*N/(WIN_SIZE*WIN_SIZE));
    for (int i =0;i<N*N;i+=WIN_SIZE*WIN_SIZE)
    {
        float the_max = matr[i];
        for (int j=0;j<WIN_SIZE*WIN_SIZE;j++)
        {
            if (matr[i+j]>the_max)
                the_max = matr[i+j];


        }

        pooling[i/(WIN_SIZE*WIN_SIZE)] = the_max;
    }

    return pooling;
}

int main()
{	
	long int N_SIZE = 32768;
    float *arr = (float*)malloc(N_SIZE*N_SIZE*sizeof(float));
    float *pool_layer;

    fill_matrix(N_SIZE, N_SIZE, arr);
    pool_layer = max_pooling1d(N_SIZE, N_SIZE, arr);
    //print_matrix(N_SIZE/WIN_SIZE, N_SIZE/WIN_SIZE, pool_layer);

    pool_layer = max_pooling2d(N_SIZE, N_SIZE, arr);
    //print_matrix(N_SIZE/WIN_SIZE, N_SIZE/WIN_SIZE, pool_layer);
    free(pool_layer);
    delete_matrix(N_SIZE, arr);
    // Чтобы увидеть результат, нужно выставить N_SIZE 512 WIN_SIZE 8 и раскомментить нижнюю строку
    //print_matrix(N_SIZE/WIN_SIZE, N_SIZE/WIN_SIZE, pool_layer);
    return 0;
}
