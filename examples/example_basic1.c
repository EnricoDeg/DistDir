#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "src/distdir.h"

#define I_SRC 0
#define I_DST 1
#define NCOLS 4
#define NROWS 4

/**********************************************************************************************************************
 * The example uses a total of 4 MPI processes over a 4x4 global 2D domain.
 * Processes 0,1 have the following domain decomposition:
 * 
 * Rank: 0
 * Indices: 0, 1, 2, 3, 4, 5, 6, 7
 * Rank: 1
 * Indices: 8, 9, 10, 11, 12, 13, 14
 * 
 * Processes 2,3 have the following domain decomposition:
 * 
 * Rank: 2
 * Indices: 0, 1, 8, 9, 2, 3, 10, 11
 * Rank: 3
 * Indices: 4, 5, 12, 13, 7, 7, 14, 15
 * 
 * Ranks 0,1 send data to ranks 2,3
 * 
 * Exchange of integers and doubles are tested.
 *
 *********************************************************************************************************************/

int main () {

    MPI_Init(NULL,NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_role;
    int npoints_local = NCOLS * NROWS / (world_size / 2);
    int idxlist[npoints_local];
    t_idxlist *p_idxlist;
    t_idxlist *p_idxlist_empty;
    t_map *p_map;

    if (world_size != 4) return 1;

    // index list with global indices
    if (world_rank < 2) {
        world_role = I_SRC;
        int ncols_local = NCOLS / (world_size / 2);
        for (int i=0; i < NROWS; i++)
            for (int j=0; j < ncols_local; j++)
                idxlist[j+i*ncols_local] = j + i * NCOLS + world_rank * (NCOLS - ncols_local);
    } else {
        world_role = I_DST;
        int nrows_local = NROWS / (world_size / 2);
        for (int i=0; i < nrows_local; i++)
            for (int j=0; j < NCOLS; j++)
                idxlist[j+i*NCOLS] = j + i * NCOLS + (world_rank - (world_size / 2)) * (NROWS - nrows_local) * NCOLS;
    }


    p_idxlist = new_idxlist(idxlist, npoints_local);
    p_idxlist_empty = new_idxlist_empty();

    if (world_role == I_SRC) {
        p_map = new_map(p_idxlist, p_idxlist_empty, MPI_COMM_WORLD);
    } else {
        p_map = new_map(p_idxlist_empty, p_idxlist, MPI_COMM_WORLD);
    }

    // test exchange
    {
        int data[npoints_local];
        // src MPI ranks fill data array
        if (world_role == I_SRC)
            for (int i = 0; i < npoints_local; i++)
                data[i] = i + npoints_local * world_rank;

        exchange_go(p_map, MPI_INT, data, data);

        printf("%d: ", world_rank);
        for (int i = 0; i < npoints_local; i++)
            printf("%d ", data[i]);
        printf("\n");
    }

    {
        double data[npoints_local];
        // src MPI ranks fill data array
        if (world_role == I_SRC)
            for (int i = 0; i < npoints_local; i++)
                data[i] = (i + npoints_local * world_rank) * 1.0;

        exchange_go(p_map, MPI_DOUBLE, data, data);

        printf("%d: ", world_rank);
        for (int i = 0; i < npoints_local; i++)
            printf("%f ", data[i]);
        printf("\n");
    }

    delete_idxlist(p_idxlist);
    delete_idxlist(p_idxlist_empty);
    delete_map(p_map);

    MPI_Finalize();
    return 0;
}
