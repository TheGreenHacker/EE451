#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int size, rank;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int msg;
    
    if (rank == 0) {
        msg = 451;
        printf("Process 0: Initially Msg = %d\n", msg);
        MPI_Send(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&msg, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process 0: Received Msg = %d. Done!\n", msg);
    }
    else if(rank == 1){
        MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        msg++;
        printf("Process 1: Msg = %d\n", msg);
        MPI_Send(&msg, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    }
    else if(rank == 2){
        MPI_Recv(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        msg++;
        printf("Process 2: Msg = %d\n", msg);
        MPI_Send(&msg, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    }
    else if(rank == 3){
        MPI_Recv(&msg, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        msg++;
        printf("Process 3: Msg = %d\n", msg);
        MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
