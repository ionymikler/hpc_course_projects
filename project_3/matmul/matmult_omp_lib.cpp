extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <omp.h>
#include "matmult_omp_lib.h"

int NUM_TEAMS = 114, THREADS_PER_TEAM = 32/64; //QUESTION: How mamy threads per team

void init_C_dev(int m, int n, double **C, int num_teams, int threads_per_team){
    // Remember; array mapping is [lower:length], NOT [lower:upper]
    #pragma omp target teams distribute parallel for \
        map(to:m,n), map(from:C[0:m][0:n]) \
        num_teams(num_teams) thread_limit(threads_per_team) // QUESTION: Should we hard code these values?
        // collapse(2)
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++){
            C[i][j] = 0.0;
        }
    }

}

void init_C(int m,int n,double **C){
    #pragma omp parallel for collapse(2) \
        shared(C, m,n )
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            #pragma omp critical
            {
                C[i][j] = 0.0;
            }
        }
    }
}

double get_sum_u(double **u, int N){
    double sum = 0;
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
                sum += u[i][j];
        }
    }
    return sum;
}

void matmult_mkn_omp(int m,int n,int k,double **A,double **B,double **C){
    init_C(m,n,C);

    
    #pragma omp parallel for collapse(2) \
        shared(A,B,C, m,n,k)
        for (int row=0; row <m; row++){
            for (int q=0; q<k; q++){
                for (int col=0; col < n; col++){
                    C[row][col] += A[row][q] * B[q][col];
                }
            }
        }
}

void matmult_mkn_offload(int m,int n,int k,double **A,double **B,double **C){
    init_C_dev(m,n,C, NUM_TEAMS, THREADS_PER_TEAM);
    
    #pragma omp target teams distribute parallel for \
        map(to:m,n,k,A[0:m][0:k],B[0:k][0:n]), map(from:C[0:m][0:n])
        for (int row=0; row <m; row++){
            for (int q=0; q<k; q++){
                for (int col=0; col < n; col++){
                    C[row][col] += A[row][q] * B[q][col];
                }
            }
        } // END PARALLEL FOR
}

void matmult_mnk_offload(int m,int n,int k,double **A,double **B,double **C){
    init_C_dev(m,n,C, NUM_TEAMS, THREADS_PER_TEAM);

    #pragma omp target teams distribute parallel for \
        map(to:m,n,k,A[0:m][0:k],B[0:k][0:n]), map(from:C[0:m][0:n])
    for (int i = 0; i < m; i++){
        for (int j = 0; j < n; j++){
            for (int q = 0; q < k; q++){
                C[i][j] += A[i][q] * B[q][j];
            }
        }
    }
}

void matmult_lib(int m, int n, int k, double **A, double **B, double **C){
    cblas_dgemm(CblasRowMajor,
                CblasNoTrans,
                CblasNoTrans,
                m, n, k,
                1.0, A[0], k,
                B[0], n, 0.0,
                C[0], n);
}
}