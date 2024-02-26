#include "bucket.h"
#include "mpi.h"

void assign_idxlist_elements_to_buckets(int *bucket_idxlist, int *idxlist, int idxlist_size, int nbuckets) {

    for (int i=0; i < idxlist_size; i++) {
        bucket_idxlist[i] = idxlist[i] / nbuckets;
        if (bucket_idxlist[i] >= nbuckets) bucket_idxlist[i] = nbuckets - 1;
    }

}

int get_n_receiver_bucket(int *bucket_idxlist, int nbuckets, int idxlist_size, MPI_Comm comm) {

    // bucket ID (rank) to send info to
    int src_comm_ranks[nbuckets];
    for (int rank = 0; rank < nbuckets; rank++)
        src_comm_ranks[rank] = 0;
    if (idxlist_size > 0) {
        for (int i = 0; i < idxlist_size; i++)
            src_comm_ranks[bucket_idxlist[i]] = 1;
    }

    // number of processes each bucket receive data from
    int count_recv;
    {
        int recv_count[nbuckets];
        for (int i=0; i<nbuckets; i++)
            recv_count[i] = 1;
        MPI_Reduce_scatter(src_comm_ranks, &count_recv, recv_count, MPI_INT, MPI_SUM, comm);
    }

    return count_recv;

}