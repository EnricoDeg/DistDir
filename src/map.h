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

struct t_map_exch {
    int count;
    struct t_map_exch_per_rank **exch;
};

struct t_map {
	MPI_Comm comm;
    struct t_map_exch *exch_send;
    struct t_map_exch *exch_recv;
};

struct t_map * new_map(struct t_idxlist *src_idxlist, struct t_idxlist *dst_idxlist, MPI_Comm comm);
struct t_map * extend_map_3d(struct t_map *map2d, int nlevels);

void delete_map(struct t_map *map);

#endif