#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

typedef struct tid_ {
    int i;
    int j;
} tid_t;

int n = 4096; // matrix size is n*n; n was 4096
int p = 1;  // square root of total number of threads

double **A;
double **B;
double **C;

/* Thread function for computing output block C(i, j) */
void *computeBlock(void *arg) {
    tid_t *tid = (tid_t*) arg;
    int i, j, k;
    for (i = tid->i; i < tid->i + n/p; i++) {
        for (j = tid->j; j < tid->j + n/p; j++) {
            for (k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return NULL;
}

int main(int argc, char *argv[]){
        int i, j, k;
        struct timespec start, stop;
        double time;
    
        if (argc == 2) {
            p = atoi(argv[1]);
        }
        
        pthread_t **threads = (pthread_t**) malloc (sizeof(pthread_t*)*p);
        tid_t **tids = (tid_t**) malloc (sizeof(tid_t*)*p);
        for (i = 0; i < p; i++) {
            threads[i] = (pthread_t*) malloc(sizeof(pthread_t)*p);
            tids[i] = (tid_t*) malloc(sizeof(tid_t)*p);
        }
        
        A = (double**) malloc (sizeof(double*)*n);
        B = (double**) malloc (sizeof(double*)*n);
        C = (double**) malloc (sizeof(double*)*n);
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
        
        /* Create pxp threads */
        int ii, jj;
        for (i = 0, ii = 0; i < n; i += n/p, ii++) {
            for (j = 0, jj = 0; j < n; j += n/p, jj++) {
                /* Assign each thread(i,j) a block */
                tids[ii][jj].i = i;
                tids[ii][jj].j = j;
                /*
                printf("ii is %d, jj is %d\n", ii, jj);
                printf("i is %d, j is %d\n", i, j);
                printf("-----------------------\n");
                 */
                if(pthread_create(&threads[ii][jj], NULL, computeBlock, (void*) &tids[ii][jj])){
                    printf("Error creating thread\n");
                    return 1;
                }
            }
        }
    
        /* Join the threads */
        for (i = 0; i < p; i++) {
            for (j = 0; j < p; j++) {
                if(pthread_join(threads[i][j], NULL)){
                    printf("Error joining thread\n");
                    return 1;
                }
            }
        }

        //*******************************//
        
        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}
        time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
        
        printf("Execution time = %f sec\n", time);
        printf("C[100][100]=%f\n", C[100][100]);
        /*
        for (i=0; i<n; i++){
            for(j=0; j< n; j++){
                printf("%f\n", C[i][j]);
            }
        }
        */
        /* Release memory */
        for (i=0; i<n; i++) {
            free(A[i]);
            free(B[i]);
            free(C[i]);
        }
               
        for (i = 0; i < p; i++) {
            free(threads[i]);
            free(tids[i]);
        }
        
        free(A);
        free(B);
        free(C);
        free(threads);
        free(tids);
                   
        return 0;
}
