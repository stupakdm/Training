#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "xmmintrin.h"
#include "math.h"
#define N_SIZE 2048
#define M_ITER 10

float multiply(float *x, float *y, int N)
{
    float sum;
    __m128 *xx, *yy;
    __m128 p,s;

    xx = (__m128*)x;
    yy = (__m128*)y;

    s = _mm_set_ps1(0);
    for (int i=0;i<N/4; i++)
    {
        p = _mm_mul_ps(xx[i], yy[i]);
        s = _mm_add_ps(s, p);
    }

    p = _mm_movehl_ps(p, s);
    s = _mm_add_ps(s,p);
    p = _mm_shuffle_ps(s,s,1);
    s = _mm_add_ss(s,p);

    _mm_store_ss(&sum, s);
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
            C[i*N+j] = multiply(&A[i*N], &B[j*N], N);
        }
    }
}

void add(float *A, float *B, float *C, int N)
{
    __m128 p,s;
    for (int i=0;i<N;i++)
    {
        __m128 *xx, *yy;
        xx= (__m128*)&A[i*N];
        yy = (__m128*)&B[i*N];
        for (int j=0;j<N/4;j++)
        {
            p=_mm_add_ps(xx[j], yy[j]);
            _mm_store_ps(&C[i*N+j*4], p);
        }
    }
}

void sub(float *A, float *B, float *C, int N)
{
    __m128 p;
    for (int i=0;i<N;i++)
    {
        for (int j=0;j<N/4;j++)
        {
            __m128 *xx,*yy;
            xx= (__m128*)&A[i*N];
            yy = (__m128*)&B[i*N];
            p = _mm_sub_ps(xx[j], yy[j]);
            _mm_store_ps(&C[i*N+j*4],p);
        }
    }
}

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

double count_sse(float *A, float *BB)
{
    float *I;
    float *B;
    float *R;
    float *C;
    float *sib;
    float *A_tr;
    int sum;

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

    //Для проверки корректности выставить N_SIZE 4, и раскомментировать строку снизу
    //print_matrix(C, N_SIZE);
    delete(I);
    delete(B);
    delete(R);
    delete(C);
    delete(sib);


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

    count_sse(A, BB);

    delete(A);
    delete(BB);

}
