#include "bucket.h"
#include "mpi.h"
#include "mergesort.h"
#include <stdlib.h>
#include <stdio.h>

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

void get_n_indices_for_each_bucket(int *size_ranks, int *bucket_idxlist, int idxlist_size, int nranks) {
    for (int rank = 0; rank < nranks; rank++)
        size_ranks[rank] = 0;
    if (idxlist_size > 0) {
        for (int i = 0, offset=0, count = 1; i < idxlist_size; i++, count++) {
            if (bucket_idxlist[i] != bucket_idxlist[offset]) {
                count = 1;
                offset = i;
            }
            size_ranks[bucket_idxlist[offset]] = count;
        }
    }
}

void map_idxlist_to_RD_decomp(struct t_bucket *bucket, struct t_idxlist *idxlist, int *idxlist_local, int nbuckets, MPI_Comm comm) {
    int world_size;
    MPI_Comm_size(comm, &world_size);
    int world_rank;
    MPI_Comm_rank(comm, &world_rank);

    // each element of the idxlist is assigned to a bucket
    int bucket_idxlist[idxlist->count];
    assign_idxlist_elements_to_buckets(bucket_idxlist, idxlist->list, idxlist->count, world_size);

    // sort bucket_idxlist -> idxlist and idxlist_local accordingly
    int src_idxlist_sort[idxlist->count];
    if (idxlist->count > 0) {
        for (int i=0; i < idxlist->count; i++) {
            src_idxlist_sort[i] = idxlist->list[i];
            idxlist_local[i] = i;
        }
        mergeSort_with_idx2(bucket_idxlist, src_idxlist_sort, idxlist_local,
                            0, idxlist->count - 1);
    }

    bucket->count_recv = get_n_receiver_bucket(bucket_idxlist, world_size, idxlist->count, comm);

    // number of indices to be sent to each bucket
    bucket->size_ranks = (int *)malloc(nbuckets*sizeof(int));
    get_n_indices_for_each_bucket(bucket->size_ranks, bucket_idxlist, idxlist->count, world_size);

    // source of each message
    if (bucket->count_recv > 0)
        bucket->src_recv = (int *)malloc(bucket->count_recv*sizeof(int));
    {
        MPI_Request req[idxlist->count+bucket->size];
        MPI_Status stat[idxlist->count+bucket->size];
        int nreq = 0;

        for (int i=0; i<world_size; i++)
            if (bucket->size_ranks[i] > 0) {
                MPI_Isend(&world_rank, 1, MPI_INT, i, i+bucket->size*(i+1), comm, &req[nreq]);
                nreq++;
            }

        for (int i=0; i<bucket->count_recv; i++) {
            MPI_Irecv(&bucket->src_recv[i], 1, MPI_INT, MPI_ANY_SOURCE, world_rank+bucket->size*(world_rank+1), comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);

        // sort by process number
        mergeSort(bucket->src_recv, 0, bucket->count_recv-1);
    }


    // size of each message that each bucket receive
    if (bucket->count_recv > 0)
        bucket->msg_size_recv = (int *)malloc(bucket->count_recv*sizeof(int));
    {
        MPI_Request req[idxlist->count+bucket->size];
        MPI_Status stat[idxlist->count+bucket->size];
        int nreq = 0;

        for (int i=0; i<world_size; i++)
            if (bucket->size_ranks[i] > 0) {
                MPI_Isend(&bucket->size_ranks[i], 1, MPI_INT, i, world_rank+bucket->size*i, comm, &req[nreq]);
                nreq++;
            }

        for (int i=0; i<bucket->count_recv; i++) {
            MPI_Irecv(&bucket->msg_size_recv[i], 1, MPI_INT, bucket->src_recv[i], bucket->src_recv[i]+bucket->size*world_rank, comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);
    }

    // gather src_bucket info
    {
        MPI_Request req[idxlist->count+bucket->size];
        MPI_Status stat[idxlist->count+bucket->size];
        int nreq = 0;

        // recv src for each bucket
        int offset = 0;
        for (int i = 0; i < bucket->count_recv; i++) {
            MPI_Irecv(&bucket->ranks[offset], bucket->msg_size_recv[i], MPI_INT, bucket->src_recv[i],
                      world_rank+bucket->size*(world_rank+1), comm, &req[nreq]);
            offset +=  bucket->msg_size_recv[i];
            nreq++;
        }

        //  MPI ranks send info to src bucket
        for (int i = 0; i < world_size; i++) {
            if (bucket->size_ranks[i] > 0) {
                int myrank_arr[bucket->size_ranks[i]];
                for (int j=0; j<bucket->size_ranks[i]; j++)
                    myrank_arr[j] = world_rank;
                MPI_Send(myrank_arr, bucket->size_ranks[i], MPI_INT, i, i+bucket->size*(i+1), comm);
            }
        }

        MPI_Waitall(nreq, req, stat);
    }


    {
        MPI_Request req[world_size*bucket->size];
        MPI_Status stat[world_size*bucket->size];
        int nreq = 0;

        //  MPI ranks send info to src bucket
        for (int i = 0, offset = 0; i < world_size; i++) {
            if (bucket->size_ranks[i] > 0) {
                MPI_Isend(&src_idxlist_sort[offset], bucket->size_ranks[i], MPI_INT,
                           i, i+bucket->size*(i+1), comm, &req[nreq]);
                offset += bucket->size_ranks[i];
                nreq++;
            }
        }

        // recv src for each bucket
        int offset = 0;
        for (int i = 0; i < bucket->count_recv; i++) {
            MPI_Irecv(&bucket->idxlist[offset], bucket->msg_size_recv[i], MPI_INT,
                       bucket->src_recv[i], world_rank+bucket->size*(world_rank+1), comm, &req[nreq]);
            offset +=  bucket->msg_size_recv[i];
            nreq++;
        }

        MPI_Waitall(nreq, req, stat);
    }

}

void map_RD_decomp_to_idxlist(struct t_bucket *src_bucket, struct t_bucket *dst_bucket, int *idxlist_local,
                              int idxlist_size, int nbuckets, MPI_Comm comm) {
    int world_size;
    MPI_Comm_size(comm, &world_size);
    int world_rank;
    MPI_Comm_rank(comm, &world_rank);

    int dst_bucket_sort_src[src_bucket->size];
    for (int i=0; i<src_bucket->size; i++) {
        for (int j=0; j<src_bucket->size; j++) {
            if (dst_bucket->idxlist[i] == src_bucket->idxlist[j]) {
                dst_bucket_sort_src[i] = dst_bucket->ranks[j];
            }
        }
    }

    src_bucket->rank_exch = (int *)malloc(idxlist_size*sizeof(int));
    {
        MPI_Request req[nbuckets*nbuckets];
        MPI_Status stat[nbuckets*nbuckets];
        int nreq = 0;
        // send dst info to MPI ranks
        for (int i = 0, offset=0; i < src_bucket->count_recv; i++) {
            MPI_Isend(&dst_bucket_sort_src[offset], src_bucket->msg_size_recv[i], MPI_INT, src_bucket->src_recv[i],
                      world_rank+nbuckets*(1+src_bucket->src_recv[i]), comm, &req[nreq]);
            offset += src_bucket->msg_size_recv[i];
            nreq++;
        }

        // MPI ranks receive the dst MPI proc for each idxlist point
        for (int i = 0, offset = 0; i < nbuckets; i++) {
            if (src_bucket->size_ranks[i] > 0) {
                MPI_Irecv(&src_bucket->rank_exch[offset], src_bucket->size_ranks[i], MPI_INT, i,
                          i+nbuckets*(1+world_rank), comm, &req[nreq]);
                offset += src_bucket->size_ranks[i];
                nreq++;
            }
        }
        MPI_Waitall(nreq, req, stat);
    }

    if (idxlist_size > 0) mergeSort_with_idx(src_bucket->rank_exch, idxlist_local, 0, idxlist_size - 1);

}