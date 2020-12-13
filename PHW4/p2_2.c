#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//#include <assert.h>

#define N 64

int main(int argc, char** argv) {
    int size, rank;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int i, partial_sum, sum;
    int arr[N];
    
    if (rank == 0) {
        FILE* fp;
        if ((fp = fopen("number.txt", "r")) == NULL) {
            fprintf(stderr, "Couldn't open file number.txt\n");
            return 1;
        }
        
        for (i = 0; i < N; i++) {
            fscanf(fp, "%d", &arr[i]);
        }
    }
    
    MPI_Bcast(arr, N, MPI_INT, 0, MPI_COMM_WORLD);
        
    partial_sum = 0;
    for (i = rank * N/4; i < (rank + 1) * N/4; i++) {
        partial_sum += arr[i];
    }
    
    MPI_Reduce(&partial_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Process %d: sum = %d\n", rank, sum);
    }
    
    MPI_Finalize();
    return 0;
}
