#ifndef BUCKET_H
#define BUCKET_H

#include "mpi.h"
#include "idxlist.h"

struct t_bucket {
    int size;
    int *idxlist;
    int *ranks;
    int count_recv;
    int *src_recv;
    int *msg_size_recv;
    int *size_ranks;
    int *rank_exch;
};

void assign_idxlist_elements_to_buckets(int *bucket_idxlist, int *idxlist, int idxlist_size, int buckets);
int get_n_receiver_bucket(int *bucket_idxlist, int nbuckets, int idxlist_size, MPI_Comm comm);
void get_n_indices_for_each_bucket(int *size_ranks, int *bucket_idxlist, int idxlist_size, int nranks);
void map_idxlist_to_RD_decomp(struct t_bucket *bucket, struct t_idxlist *idxlist, int *idxlist_local, int nbuckets, MPI_Comm comm);
void map_RD_decomp_to_idxlist(struct t_bucket *src_bucket, struct t_bucket *dst_bucket, int *idxlist_local,
                              int idxlist_size, int nbuckets, MPI_Comm comm);

#endif