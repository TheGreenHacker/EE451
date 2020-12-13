#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"

#define K 6
#define n_iterations 50

int p = 1;
unsigned char *a; // pixels
unsigned char *labels; // cluster labels for each pixel in a
double *u; // cluster values
int cluster_sizes[K]; // global count of cluster sizes
double sums[K]; // global sums per cluster

typedef struct k_means_args_ {
    int tid; // thread id
    int cluster_sizes[K]; // number of data elements assigned per cluster
    double sums[K]; // corresponding sum value per cluster
} k_means_args_t;

k_means_args_t *k_means_args; // divided among threads

void *k_means(void *arg) {
    int i, r, c, r_start, r_end, tid;
    tid = *((int *)arg);
    
    /* Reset number of elements per cluster and sum values */
    memset(k_means_args[tid].cluster_sizes, 0, K * sizeof(int));
    memset(k_means_args[tid].sums, 0.0, K * sizeof(double));
    
    r_start = tid * h/p;
    r_end = r_start + h/p;
    
    /* Assign each pixel to a cluster. */
    for (r = r_start; r < r_end; r++) {
        for (c = 0; c < w; c++) {
            double pixel = (double) a[r * h + c];
            labels[r * h + c] = 0;
            for (i = 1; i < K; i++) {
                if (fabs(u[i] - pixel) < fabs(u[labels[r * h + c]] - pixel)) {
                    labels[r * h + c] = i;
                }
            }
        }
    }
    
    /* Keep track of the sums for each cluster and how many pixels have been assigned to that cluster. */
    for (i = 0; i < K; i++) {
        for (r = r_start; r < r_end; r++) {
            for (c = 0; c < w; c++) {
                if (labels[r * h + c] == i) {
                    k_means_args[tid].cluster_sizes[i]++;
                    k_means_args[tid].sums[i] += (double) a[r * h + c];
                }
            }
        }
    }
    
    return NULL;
}

void *assign_pixels(void *arg) {
    int tid, r, c, r_start, r_end;
    tid = *((int *) arg);
    r_start = tid * h/p;
    r_end = r_start + h/p;
    
    for (r = r_start; r < r_end; r++) {
        for (c = 0; c < w; c++) {
            a[r * h + c] = (char) round(u[labels[r * h + c]]);
        }
    }
    
    return NULL;
}


int main(int argc, char** argv){
    int i, j, k, r, c;
    FILE *fp;
    struct timespec start, stop;
    double time;

    if (argc == 2) {
        p = atoi(argv[1]);
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
    
    for (i = 0; i < n_iterations; i++) {
        /* Reset globals */
        memset(cluster_sizes, 0, K * sizeof(int));
        memset(sums, 0.0, K * sizeof(double));
        
        /* Create threads to execute parallel_k_means */
        for (j = 0; j < p; j++) {
            if(pthread_create(&threads[j], NULL, k_means, (void*) &k_means_args[j].tid)){
                printf("Error creating thread\n");
                return 1;
            }
        }
        
        /* Join (synchronize) threads */
        for (j = 0; j < p; j++) {
            if(pthread_join(threads[j], NULL)){
                printf("Error joining thread\n");
                return 1;
            }
        }
        
        /* Recompute mean value of each cluster */
        for (k = 0; k < K; k++) {
            for (j = 0; j < p; j++) {
                cluster_sizes[k] += k_means_args[j].cluster_sizes[k];
                sums[k] += k_means_args[j].sums[k];
            }
            
            if (cluster_sizes[k]) {
                u[k] = sums[k] / (double) cluster_sizes[k];
            }
        }
    }
    
    /* Create threads to replace each pixel with mean value of assigned cluster */
    for (j = 0; j < p; j++) {
        if(pthread_create(&threads[j], NULL, assign_pixels, (void*) &k_means_args[j].tid)){
            printf("Error creating thread\n");
            return 1;
        }
    }
    
    /* Join threads */
    for (j = 0; j < p; j++) {
        if(pthread_join(threads[j], NULL)){
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
    
    return 0;
}
