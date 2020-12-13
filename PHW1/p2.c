#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
//#include <omp.h>

#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"

#define n_iterations 30

int main(int argc, char** argv){
    int i, j, r, c;
    FILE *fp;
    struct timespec start, stop;
    double time;

    unsigned char *a = (unsigned char*) malloc (sizeof(unsigned char)*h*w);
    
    // the matrix is stored in a linear array in row major fashion
    if (!(fp=fopen(input_file, "rb"))) {
        printf("can not opern file\n");
        return 1;
    }
    fread(a, sizeof(unsigned char), w*h, fp);
    fclose(fp);
    
    // cluster labels for each pixel in a
    unsigned char *labels = (unsigned char*) malloc (sizeof(unsigned char)*h*w);
    
    // mean values for clusters
    double *u = (double*) malloc(sizeof(double) * 4);
    u[0] = 0.0;
    u[1] = 85.0;
    u[2] = 170.0;
    u[3] = 255.0;
    
    // measure the start time here
    if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
    
    //  Your code goes here
    for (i = 0; i < n_iterations; i++) {
        /* For each pixel, compute its distance to each mean value and assign it to the closest cluster. */
        for (r = 0; r < h; r++) {
            for (c = 0; c < w; c++) {
                double pixel = (double) a[r * h + c];
                labels[r * h + c] = 0;
                for (j = 1; j < 4; j++) {
                    if (fabs(u[j] - pixel) < fabs(u[labels[r * h + c]] - pixel)) {
                        labels[r * h + c] = j;
                    }
                }
            }
        }
        
        /* Recompute mean value of each cluster */
        for (j = 0; j < 4; j++) {
            double total = 0.0;
            int n = 0;
            for (r = 0; r < h; r++) {
                for (c = 0; c < w; c++) {
                    if (labels[r * h + c] == j) {
                        n++;
                        total += (double) a[r * h + c];
                    }
                }
            }
            
            /* If at least 1 element was assigned to this cluster */
            if (n) {
                u[j] = total / (double) n;
            }
        }
    }
    
    /* Replace each pixel with mean value of assigned cluster  */
    for (r = 0; r < h; r++) {
        for (c = 0; c < w; c++) {
            a[r * h + c] = (char) round(u[labels[r * h + c]]);
            /*
            printf("floating point %f\n", u[labels[r * h + c]]);
            printf("-------------------------\n");
            printf("integer %d\n", (int)a[r * h + c]);
             */
        }
    }
        
    //
    
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
