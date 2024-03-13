#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "src/distdir.h"

int main () {

    MPI_Init(NULL,NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 4) return 1;

    // split communicator
    int color_id = 1;
    if (world_rank >= 2) color_id = 2;
    MPI_Comm group_comm;
    new_group(&group_comm, MPI_COMM_WORLD, color_id);
    int group_rank;
    MPI_Comm_rank(group_comm, &group_rank);
    int group_size;
    MPI_Comm_size(group_comm, &group_size);

    printf("%d: rank %d of %d\n", world_rank, group_rank, group_size);

    MPI_Finalize();
    return 0;

}