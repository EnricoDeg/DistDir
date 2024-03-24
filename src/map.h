#ifndef MAP_H
#define MAP_H

#include "idxlist.h"
#include "mpi.h"

struct t_map_exch_per_rank {
    int exch_rank;
    int buffer_size;
    int *buffer_idxlist;
    void *buffer;
};
typedef struct t_map_exch_per_rank t_map_exch_per_rank;

struct t_map_exch {
    int count;
    t_map_exch_per_rank **exch;
};
typedef struct t_map_exch t_map_exch;

struct t_map {
	MPI_Comm comm;
    t_map_exch *exch_send;
    t_map_exch *exch_recv;
};
typedef struct t_map t_map;

t_map * new_map(t_idxlist *src_idxlist, t_idxlist *dst_idxlist, MPI_Comm comm);
t_map * extend_map_3d(t_map *map2d, int nlevels);

void delete_map(t_map *map);

#endif