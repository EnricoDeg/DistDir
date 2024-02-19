#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "map.h"

static inline void pack_int(int *buffer, int i, int *data, int idx);
void exchange_go(struct t_map *map, MPI_Datatype type, void *src_data, void* dst_data);

#endif