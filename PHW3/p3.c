#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"

#define K 6
#define n_iterations 50

int r = 0;
int p = 1;
unsigned char *a; // pixels
unsigned char *labels; // cluster labels for each pixel in a
double *u; // cluster values

typedef struct k_means_args_ {
    int tid; // thread id
    int cluster_sizes[K]; // number of data elements assigned per cluster
    double sums[K]; // corresponding sum value per cluster
} k_means_args_t;

k_means_args_t *k_means_args; // divided among threads

pthread_mutex_t mutex;
pthread_cond_t cv;

void *k_means(void *arg) {
    int i, j, k, row, col, row_start, row_end, tid;
    tid = *((int *)arg);
    row_start = tid * h/p;
    row_end = row_start + h/p;
    /*
    printf("row_start is %d\n", row_start);
    printf("row_end is %d\n", row_end);
    */
    for (i = 0; i < n_iterations; i++) {
        //printf("i is %d\n", i);
        /* Reset number of elements per cluster and sum values */
        memset(k_means_args[tid].cluster_sizes, 0, K * sizeof(int));
        memset(k_means_args[tid].sums, 0.0, K * sizeof(double));
        
        /* Assign each pixel to a cluster. */
        for (row = row_start; row < row_end; row++) {
            for (col = 0; col < w; col++) {
                double pixel = (double) a[row * h + col];
                labels[row * h + col] = 0;
                for (k = 1; k < K; k++) {
                    if (fabs(u[k] - pixel) < fabs(u[labels[row * h + col]] - pixel)) {
                        labels[row * h + col] = k;
                    }
                }
            }
        }
        
        /* Keep track of the sums for each cluster and how many pixels have been assigned to that cluster. */
        for (j = 0; j < K; j++) {
            for (row = row_start; row < row_end; row++) {
                for (col = 0; col < w; col++) {
                    if (labels[row * h + col] == j) {
                        k_means_args[tid].cluster_sizes[j]++;
                        k_means_args[tid].sums[j] += (double) a[row * h + col];
                    }
                }
            }
        }
        
        pthread_mutex_lock(&mutex);
        if (r < p - 1) {
            r++;
            pthread_cond_wait(&cv, &mutex); // go to sleep
        }
        else {
            assert(r == p - 1);
            r = 0;
            
            /* Recompute mean values based on local intermediate data of all threads */
            for (k = 0; k < K; k++) {
                
                int n = 0;
                double total = 0.0;
                
                for (j = 0; j < p; j++) {
                    n += k_means_args[j].cluster_sizes[k];
                    total += k_means_args[j].sums[k];
                }
                
                if (n) {
                    u[k] = total / (double) n;
                }
            }
            
            pthread_cond_broadcast(&cv); // wake up other threads
        }
        pthread_mutex_unlock(&mutex);
         
    }
    
    return NULL;
}

void *assign_pixels(void *arg) {
    int tid, row, col, row_start, row_end;
    tid = *((int *) arg);
    row_start = tid * h/p;
    row_end = row_start + h/p;
    
    for (row = row_start; row < row_end; row++) {
        for (col = 0; col < w; col++) {
            a[row * h + col] = (char) round(u[labels[row * h + col]]);
        }
    }
    
    return NULL;
}


int main(int argc, char** argv){
    int i;
    FILE *fp;
    struct timespec start, stop;
    double time;

    if (argc == 2) {
        p = atoi(argv[1]);
    }
    
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Failed to init mutex\n");
        return 1;
    }
    if (pthread_cond_init(&cv, NULL) != 0) {
        printf("Failed to init conditional variable\n");
        return 1;
    }
    
    a = (unsigned char*) malloc (sizeof(unsigned char)*h*w);
    
    // the matrix is stored in a linear array in row major fashion
    if (!(fp=fopen(input_file, "rb"))) {
        printf("can not open file\n");
        return 1;
    }
    fread(a, sizeof(unsigned char), w*h, fp);
    fclose(fp);
    
    labels = (unsigned char*) malloc (sizeof(unsigned char)*h*w);
    pthread_t *threads = (pthread_t*) malloc (sizeof(pthread_t)*p);
    k_means_args = (k_means_args_t *) malloc (sizeof(k_means_args_t)*p);
    
    for (i = 0; i < p; i++) {
        k_means_args[i].tid = i;
    }
    
    // mean values for clusters
    u = (double*) malloc(sizeof(double) * K);
    u[0] = 0.0;
    u[1] = 65.0;
    u[2] = 100.0;
    u[3] = 125.0;
    u[4] = 190.0;
    u[5] = 255.0;
    
    // measure the start time here
    if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
    
    /* Create threads to execute parallel_k_means */
    for (i = 0; i < p; i++) {
        if(pthread_create(&threads[i], NULL, k_means, (void*) &k_means_args[i].tid)){
            printf("Error creating thread\n");
            return 1;
        }
    }
    
    /* Join threads */
    for (i = 0; i < p; i++) {
        if(pthread_join(threads[i], NULL)){
            printf("Error joining thread\n");
            return 1;
        }
    }
    
    /* Create threads to replace each pixel with mean value of assigned cluster */
    for (i = 0; i < p; i++) {
        if(pthread_create(&threads[i], NULL, assign_pixels, (void*) &k_means_args[i].tid)){
            printf("Error creating thread\n");
            return 1;
        }
    }
    
    /* Join threads */
    for (i = 0; i < p; i++) {
        if(pthread_join(threads[i], NULL)){
            printf("Error joining thread\n");
            return 1;
        }
    }
    /*
    for (i = 0; i < K; i++) {
        printf("Cluster %d avg is %f\n", i, u[i]);
    }
    */
    // measure the end time here
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}
    time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
    
    // print out the execution time here
    printf("Execution time = %f sec\n", time);
    
    
    if (!(fp=fopen(output_file,"wb"))) {
        printf("can not opern file\n");
        return 1;
    }
    fwrite(a, sizeof(unsigned char),w*h, fp);
    fclose(fp);
    
    /* Release memory */
    free(a);
    free(labels);
    free(threads);
    free(k_means_args);
    free(u);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cv);
    
    return 0;
}
