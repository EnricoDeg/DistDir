#include <stdlib.h>

#include "exchange.h"
#include "check.h"

typedef void (*kernel_func) (void*, int, void*, int);

struct xt_un_pack_kernels {;
    kernel_func pack;
    kernel_func unpack;
};
typedef struct xt_un_pack_kernels xt_un_pack_kernels;

static inline void pack_int(int *buffer, int i, int *data, int idx) {
    buffer[i] = data[idx];
}

static inline void unpack_int(int *data, int idx, int *buffer, int i) {
    data[idx] = buffer[i];
}

static inline void pack_float(float *buffer, int i, float *data, int idx) {
    buffer[i] = data[idx];
}

static inline void unpack_float(float *data, int idx, float *buffer, int i) {
    data[idx] = buffer[i];
}

static inline void pack_double(double *buffer, int i, double *data, int idx) {
    buffer[i] = data[idx];
}

static inline void unpack_double(double *data, int idx, double *buffer, int i) {
    data[idx] = buffer[i];
}

static void select_un_pack_kernels(xt_un_pack_kernels *table_kernels, MPI_Datatype type) {
    if (type == MPI_INT) {
        table_kernels->pack = (kernel_func)pack_int;
        table_kernels->unpack = (kernel_func)unpack_int;
    } else if (type == MPI_REAL) {
        table_kernels->pack = (kernel_func)pack_float;
        table_kernels->unpack = (kernel_func)unpack_float;
    } else if (type == MPI_DOUBLE) {
        table_kernels->pack = (kernel_func)pack_double;
        table_kernels->unpack = (kernel_func)unpack_double;
    }
}

void exchange_go(t_map *map, MPI_Datatype type, void *src_data, void* dst_data) {
    int world_size;
    check_mpi( MPI_Comm_size(map->comm, &world_size) );
    int world_rank;
    check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

    xt_un_pack_kernels vtable_kernels;
    select_un_pack_kernels(&vtable_kernels, type);

    // exchange data array
    MPI_Request req[map->exch_send->count + map->exch_recv->count];
    MPI_Status stat[map->exch_send->count + map->exch_recv->count];
    int nreq = 0;

    MPI_Aint type_size;
    MPI_Aint type_lb;
    check_mpi( MPI_Type_get_extent(type, &type_lb, &type_size) );

    // send step
    for (int count = 0; count < map->exch_send->count; count++) {
        map->exch_send->exch[count]->buffer = malloc(map->exch_send->exch[count]->buffer_size * type_size);
        for (int i = 0; i < map->exch_send->exch[count]->buffer_size; i++) {
            int data_idx = map->exch_send->exch[count]->buffer_idxlist[i];
            vtable_kernels.pack(map->exch_send->exch[count]->buffer, i, src_data, data_idx);
        }
        check_mpi( MPI_Isend(map->exch_send->exch[count]->buffer, map->exch_send->exch[count]->buffer_size, type,
                             map->exch_send->exch[count]->exch_rank,
                             world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
                             map->comm, &req[nreq]) );
        nreq++;
    }

    // recv step
    for (int count = 0; count < map->exch_recv->count; count++) {
        map->exch_recv->exch[count]->buffer = malloc(map->exch_recv->exch[count]->buffer_size * type_size);
        check_mpi( MPI_Irecv(map->exch_recv->exch[count]->buffer, map->exch_recv->exch[count]->buffer_size, type,
                             map->exch_recv->exch[count]->exch_rank,
                             map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
                             map->comm, &req[nreq]) );
        nreq++;
    }

    check_mpi( MPI_Waitall(nreq, req, stat) );

    // unpack recv buffers
    for (int count = 0; count < map->exch_recv->count; count++) {
        for (int i = 0; i < map->exch_recv->exch[count]->buffer_size; i++) {
            int data_idx = map->exch_recv->exch[count]->buffer_idxlist[i];
            vtable_kernels.unpack(dst_data, data_idx, map->exch_recv->exch[count]->buffer, i);
        }
    }

    // free memory
    for (int count = 0; count < map->exch_send->count; count++)
        free(map->exch_send->exch[count]->buffer);

    for (int count = 0; count < map->exch_recv->count; count++)
        free(map->exch_recv->exch[count]->buffer);

}