#ifndef BUCKET_H
#define BUCKET_H

#include "mpi.h"

void assign_idxlist_elements_to_buckets(int *bucket_idxlist, int *idxlist, int idxlist_size, int buckets);
int get_n_receiver_bucket(int *bucket_idxlist, int nbuckets, int idxlist_size, MPI_Comm comm);
void get_n_indices_for_each_bucket(int *size_ranks, int *bucket_idxlist, int idxlist_size, int nranks);

#endif