#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "src/distdir.h"

#define I_SRC 0
#define I_DST 1
#define NCOLS 4
#define NROWS 4
#define NLEVS 2

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
    t_map *p_map2d;

    if (world_size != 4) return 1;

    // index list with global indices


    if (world_rank < 2) {
        world_role = I_SRC;
        int ncols_local = NCOLS / (world_size / 2);
        for (int i=0; i<npoints_local; i++)
            idxlist[i] = world_rank + i*2;
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
        p_map2d = new_map(p_idxlist, p_idxlist_empty, MPI_COMM_WORLD);
    } else {
        p_map2d = new_map(p_idxlist_empty, p_idxlist, MPI_COMM_WORLD);
    }
    p_map = extend_map_3d(p_map2d, NLEVS);

    // test exchange
    {
        int data[npoints_local*NLEVS];
        // src MPI ranks fill data array
        if (world_role == I_SRC)
            for (int level = 0; level < NLEVS; level++)
                for (int i = 0; i < npoints_local; i++)
                    data[i+level*npoints_local] = i + level*npoints_local + npoints_local * NLEVS * world_rank;

        exchange_go(p_map, MPI_INT, data, data);

        printf("%d: ", world_rank);
        for (int level = 0; level < NLEVS; level++)
            for (int i = 0; i < npoints_local; i++)
                printf("%d ", data[i+level*npoints_local]);
        printf("\n");
    }

    delete_idxlist(p_idxlist);
    delete_idxlist(p_idxlist_empty);
    delete_map(p_map2d);
    delete_map(p_map);

    MPI_Finalize();
    return 0;
}
