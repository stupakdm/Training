#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "x86intrin.h"
#include "immintrin.h"
#define N_SIZE 2048
#define M_ITER 10

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
            C[i*N+j] = multiply_vector(&A[i*N], &B[j*N], N);
        }
    }
}

void add(float *A, float *B, float *C, int N)
{
    __m256 p;
    for (int i=0;i<N;i++)
    {
        //__m256 *xx, *yy;
        //xx= (__m256*)&A[i*N];
        //yy = (__m256*)&B[i*N];

        //for (; i + 7 < length; i += 8)
        for (int j=0;j+7<N;j+=8)
        {
            p=_mm256_add_ps(_mm256_loadu_ps(A+(i*N+j)), _mm256_loadu_ps(B+(i*N+j)));
            _mm256_storeu_ps(&C[i*N+j], p);
        }
    }
}


void sub(float *A, float *B, float *C, int N)
{
    __m256 p;
    for (int i=0;i<N;i++)
    {
        for (int j =0;j+7<N;j+=8)
        {
            //__m256 *xx,*yy;
            //xx= (__m256*)&A[i*N];
            //yy = (__m256*)&B[i*N];
            p = _mm256_sub_ps(_mm256_loadu_ps(A+(i*N+j)), _mm256_loadu_ps(B+(i*N+j)));
            _mm256_storeu_ps(&C[i*N+j], p);
            //_mm_store_ps(&C[i*N+j*4],p);
        }
    }
}

/*float multiply_vector(int *x, int *y, int N)
{
    int sum;
    int *result = (int*)malloc(N/8*sizeof(int));
    memset(result, 0, N);
    __m256i *xx, *yy, *res;
    __m256i p,s;

    xx = (__m256i*)x;
    yy = (__m256i*)y;
    res = (__m256i*)result;



    //s = _mm_set_ps1(0);
    for (int i=0;i<N; i+=8)
    {
        res[i/8] = _mm256_maddubs_epi16(xx[i], yy[i]);
        //p = _mm256_mul_ps(xx[i], yy[i]);
        //s = _mm256_add_ps(s, p);
        //p = _mm_mul_ps(xx[i], yy[i]);
        //s = _mm_add_ps(s, p);
    }
    __m256i final_sum = _mm256_setzero_si256();
    for (int i=0;i<N/8;i++)
    {
        final_sum+=res[i];
    }
    //_m256_
    //s = _mm256_add_ps(s, p);
    //_mm256_maskstore_ps(&sum, _mm256_set_epi32(0,0,0,0,0,0,0,~0), final_sum);
    //p = _mm256_shuffle_ps(s, s, 1);
    //p = _mm_movehl_ps(p, s);
    //s = _mm_add_ps(s,p);
    //p = _mm_shuffle_ps(s,s,1);
    //s = _mm_add_ss(s,p);

    //_mm256_maskstore_ps(&sum, _mm256_set_epi32(0,0,0,0,0,0,0,~0), s);
    //_mm_store_ss(&sum, s);
    //return sum;


}
*/

float find_norm(int M, int N, float **matr)
{
    float m_max = 0;
    for (int i =0;i<M;i++)
    {
        float sum = 0;
        for (int j=0;j<N;j++)
            sum += fabs(matr[i][j]);
        if (i==0)
            m_max = sum;
        else
            if (sum >= m_max)
                m_max = sum;
    }
    return m_max;
}


float find_norm1(int M, int N, float *matr)
{
    float m_max = 0;
    for (int i =0;i<M;i++)
    {
        float sum = 0;
        for (int j=0;j<N;j++)
            sum += fabs(matr[i*N+j]);
        if (i==0)
            m_max = sum;
        else
            if (sum >= m_max)
                m_max = sum;
    }
    return m_max;
}

float find_norm_inf(int M, int N, float *matr)
{
    float m_max = 0;
    for (int j =0;j<M;j++)
    {
        float sum = 0;
        for (int i=0;i<N;i++)
            sum += fabs(matr[i*N+j]);
        if (j==0)
            m_max = sum;
        else
            if (sum >= m_max)
                m_max = sum;
    }
    return m_max;
}

void divide_matr(float *matr, float *B, int N, float norm_1, float norm_inf)
{
    for (int i=0;i<N;i++)
        for (int j=i;j<N;j++)
            B[i*N+j] = matr[j*N+i]/(norm_1*norm_inf);
}

void copy_matrix(float *matr1, float *matr2, int N)
{
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            matr2[i*N+j] = matr1[i*N+j];
}

void delete(float *matr)
{
    free(matr);
}

void fill_eye(float *matr, int N)
{
    for (int i=0;i<N;i++)
    {
        for (int j=0;j<N;j++)
        {
            if (i==j)
                matr[i*N+j] = 1;
            else
                matr[i*N+j] = 0;
        }
    }
}

double count_avx(float *A, float *BB)
{
    float *I;
    float *B;
    float *R;
    float *C;
    float *sib;
    float *A_tr;
    //int sum;

    float norm1, norm_inf;
    I = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    B = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    R = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    C = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    sib = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    A_tr = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);

    norm1 = find_norm1(N_SIZE, N_SIZE, A);
    norm_inf = find_norm_inf(N_SIZE, N_SIZE, A);

    fill_eye(I, N_SIZE);

    transpose(A, A_tr, N_SIZE, N_SIZE);
    divide_matr(A_tr, B, N_SIZE, norm1, norm_inf);

    dot(A, B, C, N_SIZE);
    sub(I, C, R, N_SIZE);

    for (int i=0;i<N_SIZE;i++)
        for (int j=0;j<N_SIZE;j++)
        {
            BB[i*N_SIZE+j] = I[i*N_SIZE+j];
            sib[i*N_SIZE+j] = I[i*N_SIZE+j];
        }

    for (int s=1;s<=M_ITER;s++)
    {
        dot(sib, R,C, N_SIZE);
        copy_matrix(C, sib, N_SIZE);
        add(BB, sib, BB, N_SIZE);
    }

    dot(BB, B, C, N_SIZE);

    //Для проверки корректности выставить N_SIZE 8, и раскомментировать строку снизу
    //print_matrix(C, N_SIZE);
    delete(I);
    delete(B);
    delete(R);
    delete(C);
    delete(sib);

    return 0.0;
}


int main()
{

    float *A, *BB;
    A = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    BB = (float*)malloc(sizeof(float)*N_SIZE*N_SIZE);
    for (int i=0;i<N_SIZE;i++)
        for (int j=0;j<N_SIZE;j++)
        {
            if (i==j)
                A[i*N_SIZE+j] = 512;
            else
                A[i*N_SIZE+j] = 1;
        }

    count_avx(A, BB);

    delete(A);
    delete(BB);
    /*float *arr1, *arr2;
    arr1 = (float*)malloc(N_SIZE*sizeof(float));
    arr2 = (float*)malloc(N_SIZE*sizeof(float));


    for (int i =0;i<N_SIZE;i++)
    {
        arr1[i] = i;
        arr2[i] = i*N_SIZE;
    }
    printf("%f\n", multiply_vector(arr1, arr2, N_SIZE));
    //printf("%f\n", multiply_vector(arr1_i, arr2_i, N_SIZE));

    return 0;*/
}
