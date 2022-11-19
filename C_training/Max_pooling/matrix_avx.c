//#define N_SIZE 65536
#define WIN_SIZE 8
#define MULT_SIZE 8
#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "x86intrin.h"
#include "immintrin.h"
#include "math.h"


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


float* max_pooling2d_avx(int M, long int N, float *matr)
{
    float *result = (float*)malloc(sizeof(float)*N*N/(WIN_SIZE*WIN_SIZE));

    __m256 *xx;

    xx = (__m256*)matr;
    __m256 the_max = _mm256_set1_ps(-__FLT_MAX__);

    float res[MULT_SIZE];
    float true_max;
    for (int i=0;i<N;i+=WIN_SIZE)
    {
        for (int j=0;j<N;j+=WIN_SIZE)
        {
            the_max = _mm256_set1_ps(-__FLT_MAX__);
            for (int d=0;d<WIN_SIZE;d++)
            {
                for (int k=0;k<WIN_SIZE;k+=MULT_SIZE)
                {
                    the_max = _mm256_max_ps(the_max, _mm256_loadu_ps(matr+i*N+d*N+j+k));
                }
            }
            _mm256_store_ps(res, the_max);
            true_max = res[0];
            for (int k=1;k<MULT_SIZE;k++)
            {
                if (res[k] > true_max)
                    true_max = res[k];
            }
            //printf("%d\n", (i/WIN_SIZE)*(WIN_SIZE*WIN_SIZE)+j/(WIN_SIZE));

            result[(i/WIN_SIZE)*(WIN_SIZE*WIN_SIZE)+j/(WIN_SIZE)] = true_max;
        }
    }

    return result;
}

float* max_pooling1d_avx(int M, long int N, float *matr)
{
    float *result = (float*)malloc(sizeof(float)*N*N/(WIN_SIZE*WIN_SIZE));
    __m256 *xx;

    xx = (__m256*)matr;
    __m256 the_max = _mm256_set1_ps(-__FLT_MAX__);
    float res[MULT_SIZE];
    float true_max;
    for (int i =0;i<N*N;i+=WIN_SIZE*WIN_SIZE)
    {
       the_max = _mm256_set1_ps(-__FLT_MAX__);
       for (int j=0;j<WIN_SIZE*WIN_SIZE;j+=MULT_SIZE)
       {
           the_max = _mm256_max_ps(the_max, _mm256_loadu_ps(matr+i+j));
       }
       _mm256_store_ps(res, the_max);
       true_max = res[0];
       for (int k=1;k<MULT_SIZE;k++)
       {
           if (res[k] > true_max)
               true_max = res[k];
       }
       //printf("%d\n", i/(WIN_SIZE*WIN_SIZE));
       result[i/(WIN_SIZE*WIN_SIZE)] = true_max;

    }
    return result;
}

int main()
{
	long int N_SIZE = 32768;
    float *arr = (float*)malloc(N_SIZE*N_SIZE*sizeof(float));
    float *pool_layer;

    fill_matrix(N_SIZE, N_SIZE, arr);
    pool_layer = max_pooling1d_avx(N_SIZE, N_SIZE, arr);
    //print_matrix(N_SIZE/WIN_SIZE, N_SIZE/WIN_SIZE, pool_layer);

    pool_layer = max_pooling2d_avx(N_SIZE, N_SIZE, arr);
    //print_matrix(N_SIZE/WIN_SIZE, N_SIZE/WIN_SIZE, pool_layer);
    free(pool_layer);
    delete_matrix(N_SIZE, arr);

    // Чтобы увидеть результат, нужно выставить N_SIZE 512 WIN_SIZE 8 и раскомментить нижнюю строку
    //print_matrix(N_SIZE/WIN_SIZE, N_SIZE/WIN_SIZE, pool_layer);
    return 0;
}
