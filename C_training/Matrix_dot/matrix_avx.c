#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "x86intrin.h"
#include "immintrin.h"
#define N_SIZE 4096


float multiply_vector(float * array1, float * array2, int length) {
  __m256 vsum = _mm256_setzero_ps();
  int i = 0;

  for (; i + 7 < length; i += 8) {
    vsum = _mm256_fmadd_ps(_mm256_loadu_ps(array1 + i),_mm256_loadu_ps(array2 + i),vsum);
  }
  float buffer[8];
  _mm256_storeu_ps(buffer,vsum);
  float sum = buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5] + buffer[6] + buffer[7];
  // Скалярное произведение тех элементов, что не делятся на 8
  for (; i < length; ++i) {
      sum += array1[i] * array2[i];
  }
  return sum;
}


void print_matrix(float *matr, int N)
{
    for (int i=0;i<N;i++)
    {
        for (int j =0;j<N;j++)
            printf("%f ", matr[i*N+j]);
        printf("\n");
    }
}

void transpose(float *a, float *b, int M, int N)
{
    for (int m=0;m<M;m++)
    {
        for (int n=0;n<N;n++)
            b[m*N+n] = a[n*N+m];
    }
}

void dot(float *A, float *B, float *C, int N)
{
    float *tmp = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    transpose(B, tmp, N, N);
    for (int i=0;i<N;i++)
    {
        for (int j=0;j<N;j++)
        {
            C[i*N+j] = multiply_vector(&A[i*N], &tmp[j*N], N);
        }
    }
}

void delete(float *matr)
{
    free(matr);
}



int main()
{

    float *A, *B, *C;
    A = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    B = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    C = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);

    for (int i =0;i<N_SIZE;i++)
    {
        for (int j=0;j<N_SIZE;j++)
        {
            A[i*N_SIZE+j] = i;
            B[i*N_SIZE+j] = i;
        }
    }

    dot(A,B,C, N_SIZE);
    //Чтобы увидеть результат, выставить N_SIZE в 8 и раскомментить строку снизу
    //print_matrix(C, N_SIZE);

    delete(A);
    delete(B);
    delete(C);


    return 0;
}
