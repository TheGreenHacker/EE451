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
    int a[N];
    int b[N/4];
    int partial_sums[4];
    
    if (rank == 0) {
        FILE* fp;
        if ((fp = fopen("number.txt", "r")) == NULL) {
            fprintf(stderr, "Couldn't open file number.txt\n");
            return 1;
        }
        
        for (i = 0; i < N; i++) {
            fscanf(fp, "%d", &a[i]);
        }
    }
    
    MPI_Scatter(a, N/4, MPI_INT, b, N/4, MPI_INT, 0, MPI_COMM_WORLD);
        
    partial_sum = 0;
    for (i = 0; i < N/4; i++) {
        partial_sum += b[i];
    }
    
    MPI_Gather(&partial_sum, 1, MPI_INT, partial_sums, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        sum = 0;
        for (i = 0; i < 4; i++) {
            sum += partial_sums[i];
        }
        printf("Process %d: sum = %d\n", rank, sum);
    }
    
    MPI_Finalize();
    return 0;
}
