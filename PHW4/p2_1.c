#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 64

int main(int argc, char** argv) {
    int size, rank;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int i, partial_sum, sum;
    int arr[N];
    FILE* fp;
    
    if ((fp = fopen("number.txt", "r")) == NULL) {
        fprintf(stderr, "Couldn't open file number.txt\n");
        return 1;
    }
    
    for (i = 0; i < N; i++) {
        fscanf(fp, "%d", &arr[i]);
    }
    
    partial_sum = 0;
    for (i = rank * N/4; i < (rank + 1) * N/4; i++) {
        partial_sum += arr[i];
    }
    
    if (rank) {
        MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else {
        sum = partial_sum;
        for (i = 1; i < size; i++) {
            MPI_Recv(&partial_sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += partial_sum;
        }
        
        printf("Process %d: sum = %d\n", rank, sum);
    }
    
    MPI_Finalize();
    return 0;
}
