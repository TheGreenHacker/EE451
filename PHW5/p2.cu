#include <stdlib.h>
#include <stdio.h>
#include <cublas.h>
#include <time.h>

#define N 1024
#define block_size 32

__global__ void MM(int *a, int *b, int *c){
    int row, col, my_x, my_y;
    row = threadIdx.y;
    col = threadIdx.x;
    my_y = blockIdx.y * blockDim.y + threadIdx.y;
    my_x = blockIdx.x * blockDim.x + threadIdx.x;

    int i, j;
    int result = 0;
    __shared__ int A_s[block_size][block_size];
    __shared__ int B_s[block_size][block_size];
    
    
    for (i = 0; i < N / block_size; i++) {
        A_s[row][col] = a[my_y * N + (i * blockDim.y + col)];
        B_s[row][col] = b[(i * blockDim.x + row) * N + my_x];
        __syncthreads();
        for (j = 0; j < block_size; j++) {
            result += A_s[row][j] * B_s[j][col];
        }
        __syncthreads();
    }
    c[my_y * N + my_x] = result;
    
    /*
    for (i = 0; i < N / block_size; i++) {
        A_s[row][col] = a[my_x * N + (i * blockDim.y + col)];
        B_s[row][col] = b[(i * blockDim.x + row) * N + my_y];
        __syncthreads();
        for (j = 0; j < block_size; j++) {
            result += A_s[row][j] * B_s[j][col];
        }
        __syncthreads();
    }
    c[my_x * N + my_y] = result;
    */
}

int main() {
    int i, j;
    int *a, *b, *c;
    a = (int *) malloc(sizeof(int) * N * N);
    b = (int *) malloc(sizeof(int) * N * N);
    c = (int *) malloc(sizeof(int) * N * N);
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            
            a[i * N + j] = 1;
            b[i * N + j] = 2;
            
            /*
            a[i * N + j] = j;
            b[i * N + j] = j + 1;
            */
        }
    }
    
    int *gpu_a, *gpu_b, *gpu_c;
    cudaMalloc((void**)&gpu_a, sizeof(int) * N * N);
    cudaMalloc((void**)&gpu_b, sizeof(int) * N * N);
    cudaMalloc((void**)&gpu_c, sizeof(int) * N * N);
    
    cudaMemcpy(gpu_a, a, sizeof(int) * N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_b, b, sizeof(int) * N * N, cudaMemcpyHostToDevice);
    
    dim3 dimGrid(32, 32);
    dim3 dimBlock(32, 32);
    
    struct timespec start, stop;
    double time;
    
    if(clock_gettime(CLOCK_REALTIME, &start) == -1 ) {perror("clock gettime");}
    MM<<<dimGrid, dimBlock>>>(gpu_a, gpu_b, gpu_c);
    cudaMemcpy(c, gpu_c, sizeof(int) * N * N, cudaMemcpyDeviceToHost);
    if(clock_gettime(CLOCK_REALTIME, &stop) == -1 ) {perror("clock gettime");}
    
    time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time is %f ns\n", time * 1e9);
    printf("C[451][451] is %d\n", c[451 * N + 451]);
    
    /*
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%d\n", c[i * N + j]);
        }
    }
    */
    
    free(a);
    free(b);
    free(c);
    cudaFree(gpu_a);
    cudaFree(gpu_b);
    cudaFree(gpu_c);
    return 0;
}
