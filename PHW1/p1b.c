#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int min(int x, int y) {
    return (x < y) ? x : y;
}

int main(int argc, char *argv[]){
        int i, j, k, u, v, w;
        struct timespec start, stop;
        double time;
        int n = 4096; // matrix size is n*n; n was 4096
        int b;  // block size is b*b
    
        if (argc != 2) {
            printf("USAGE: ./p1b b, where b is an integer for block size\n");
            printf("Exiting...\n");
            return 1;
        }
    
        b = atoi(argv[1]); // parse command line for block size
        
        double **A = (double**) malloc (sizeof(double*)*n);
        double **B = (double**) malloc (sizeof(double*)*n);
        double **C = (double**) malloc (sizeof(double*)*n);
        for (i=0; i<n; i++) {
            A[i] = (double*) malloc(sizeof(double)*n);
            B[i] = (double*) malloc(sizeof(double)*n);
            C[i] = (double*) malloc(sizeof(double)*n);
        }
        
        for (i=0; i<n; i++){
            for(j=0; j< n; j++){
                A[i][j]=i;
                B[i][j]=i+j;
                C[i][j]=0;
            }
        }
    
        
                
        if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
        
        // Your code goes here //
        // Matrix C = Matrix A * Matrix B //
        //*******************************//
        for (i = 0; i < n; i += b) {
            for (j = 0; j < n; j += b) {
                for (k = 0; k < n; k += b) {
                    for (u = i; u < min(i + b, n); u++) {
                        for (v = j; v < min(j + b, n); v++) {
                            for (w = k; w < min(k + b, n); w++) {
                                C[u][v] += A[u][w] * B[w][v];
                            }
                        }
                    }
                }
            }
        }
        /*
        for (i=0; i<n; i++){
            for(j=0; j< n; j++){
                printf("%f\n", C[i][j]);
            }
        }
        */
        //*******************************//
        
        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}
        time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
        
        printf("Number of FLOPs = %lu, Execution time = %f sec,\n%lf MFLOPs per sec\n", 2*n*n*n, time, 1/time/1e6*2*n*n*n);
        printf("C[100][100]=%f\n", C[100][100]);
        
        // release memory
        for (i=0; i<n; i++) {
            free(A[i]);
            free(B[i]);
            free(C[i]);
        }
        free(A);
        free(B);
        free(C);
        return 0;
}
