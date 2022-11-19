#include "stdlib.h"
#include "time.h"
#include "stdio.h"
#include "mmintrin.h"
#include "emmintrin.h"
#define N_SIZE 4096


float inner(float *x, float *y, int N_size)
{
    __m128 *xx, *yy;
    __m128 p,s;
    xx = (__m128*)x;
    yy = (__m128*)y;
    s = _mm_set_ps1(0);

    for (int i=0;i<N_size/4;++i)
    {
        p = _mm_mul_ps(xx[i], yy[i]);
        s = _mm_add_ps(s,p);
    }

    p = _mm_movehl_ps(p, s);
    s = _mm_add_ps(s,p);
    p = _mm_shuffle_ps(s,s,1);
    s = _mm_add_ss(s,p);

    float sum;
    _mm_store_ss(&sum, s);
    return sum;

}

float mult_orig(float *x, float *y, int N_size, int K_size, int m, int k)
{
    float s = 0;
    //A[m*N+n]*B[n*K+k];
    //for i in ra
    for (int n=0;n<N_size;n++)
        s+=x[m*N_size+n]*y[n*K_size+k];
        //s+=x[i]*y[i];
    return s;
}


void transpose(float *a, float *b, int M, int N)
{
    for (int m=0;m<M;m++)
    {
        for (int n=0;n<N;n++)
            b[m*N+n] = a[n*M+m];
    }
}

void transpose_sse(float **a, float **b, int M, int N)
{
    for (int m=0;m<M;m++)
    {
        for (int n=0;n<N;n++)
            b[m][n] = a[n][m];
    }
}



void gemm_v0(int M, int N, int K, float *A, float *B, float *C)
{
    for (int m=0;m<M;++m)
    {
        for (int k=0; k<K;++k)
        {
            C[m*N+k] = 0;
            C[m*K+k] += mult_orig(A, B, N, K, m, k);

        }
    }
}


void gemm_sse(int M, int N, int K, float **A, float **B, float **C)
{
    for (int m=0;m<M;++m)
    {
        for (int k=0; k<K;++k)
        {
            C[m][k] = inner(A[m], B[k], N);

        }
    }
}

void print_matrix(int M, int N, float *matr)
{
    for (int i =0;i<M;++i)
    {
        for (int j = 0;j<N;++j)
            printf("%f ", matr[i*N+j]);
        printf("\n");;
    }
}

void print_matrix_sse(int M, int N, float **matr)
{
    for (int i =0;i<M;++i)
    {
        for (int j = 0;j<N;++j)
            printf("%f ", matr[i][j]);
        printf("\n");;
    }
}

void fill_matrix(int M, int N, float *matr)
{
    float number;
    for (int i=0;i<M;i++)
    {
        for (int j=0;j<N;j++)
        {
            number = (((float)rand())/((float)RAND_MAX))*10;
            matr[i*N+j] = number;
            //printf("%f ", matr[i][j]);
        }
    }
}

void fill_matrix_sse(int M, int N, float **matr)
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


void delete_sse(int M, float **matr)
{
    for (int i =0;i<M;i++)
    {
        free(matr[i]);
    }
    free(matr);
}

void delete(int M, float *A, float *B, float *C)
{

    free(A);
    free(B);
    free(C);
}

int main()
{


    srand((unsigned)time(NULL));

    float **x, **y, **x_x, **res;
    x = (float**)_mm_malloc(N_SIZE*sizeof(float*), 32);
    y = (float**)_mm_malloc(N_SIZE*sizeof(float*), 32);
    x_x = (float**)_mm_malloc(N_SIZE*sizeof(float*), 32);

    res = (float**)_mm_malloc(N_SIZE*sizeof(float*), 32);
    for (int i = 0; i < N_SIZE; i++)
    {
        x[i] = (float*)_mm_malloc(N_SIZE * sizeof(float), 16);
        y[i] = (float*)_mm_malloc(N_SIZE * sizeof(float), 16);
        x_x[i] = (float*)_mm_malloc(N_SIZE * sizeof(float), 16);
        res[i] = (float*)_mm_malloc(N_SIZE * sizeof(float), 16);
        //C[i] = (float*)malloc(M_SIZE * sizeof(float));
    }

    fill_matrix_sse(N_SIZE, N_SIZE, x);
    fill_matrix_sse(N_SIZE, N_SIZE, y);


    transpose_sse(x, x_x, N_SIZE, N_SIZE);

    delete_sse(N_SIZE, x);

    gemm_sse(N_SIZE, N_SIZE, N_SIZE, x_x, y, res);


    delete_sse(N_SIZE, x_x);
    delete_sse(N_SIZE, y);
    delete_sse(N_SIZE, res);


    return 0;
}
