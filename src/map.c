#include "map.h"
#include <stdio.h>
#include <stdlib.h>

struct t_map new_map(struct t_idxlist *src_idxlist, struct t_idxlist *dst_idxlist, MPI_Comm comm) {
    int world_size;
    MPI_Comm_size(comm, &world_size);
    int world_rank;
    MPI_Comm_rank(comm, &world_rank);

    // add a check that src_idxlist do not overlap over the processes in comm

    // add a check that dst_idxlist do not overlap over the processes in comm

    // compute the src and dst bucket size
    int bucket_size;
    {
        int src_bucket_size = 0;
        {
            int max_idx_value = 0;
            for (int i = 0; i < src_idxlist->count; i++)
                if (src_idxlist->list[i] > max_idx_value)
                    max_idx_value = src_idxlist->list[i];
            MPI_Allreduce(&max_idx_value, &src_bucket_size, 1, MPI_INT, MPI_MAX, comm);
        }
        src_bucket_size++;
        src_bucket_size /= world_size;
        if (world_rank == world_size-1) src_bucket_size += (src_bucket_size % world_size);

        int dst_bucket_size;
        {
            int max_idx_value = 0;
            for (int i = 0; i < src_idxlist->count; i++)
                if (src_idxlist->list[i] > max_idx_value)
                    max_idx_value = src_idxlist->list[i];
            MPI_Allreduce(&max_idx_value, &dst_bucket_size, 1, MPI_INT, MPI_MAX, comm);
        }
        dst_bucket_size++;
        dst_bucket_size /= world_size;
        if (world_rank == world_size-1) dst_bucket_size += (dst_bucket_size % world_size);
        if (src_bucket_size != dst_bucket_size) {
            // return 1;
        } else {
            bucket_size = src_bucket_size;
        }
    }

    // each element of the idxlist is assigned to a bucket
    int src_bucket_idxlist[src_idxlist->count];
    for (int i=0; i < src_idxlist->count; i++) {
        src_bucket_idxlist[i] = src_idxlist->list[i] / world_size;
        if (src_bucket_idxlist[i] >= world_size) src_bucket_idxlist[i] = world_size - 1;
    }
    
    // send src_idxlist bucket info to the RD decomposition
    int src_bucket[bucket_size];
    {
        MPI_Request req[src_idxlist->count+bucket_size];
        MPI_Status stat[src_idxlist->count+bucket_size];
        int nreq = 0;

        //  MPI ranks send info to src bucket
        for (int i = 0; i < src_idxlist->count; i++) {
            MPI_Isend(&world_rank, 1, MPI_INT, src_bucket_idxlist[i], src_idxlist->list[i], comm, &req[nreq]);
            nreq++;
        }

        // recv src for each bucket
        for (int i = 0; i < bucket_size; i++) {
            MPI_Irecv(&src_bucket[i], 1, MPI_INT, MPI_ANY_SOURCE, i+bucket_size*(world_rank), comm, &req[nreq]);
            nreq++;
        }

        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", src_bucket[i]);
    printf("\n");
*/
    int dst_bucket_idxlist[dst_idxlist->count];
    for (int i=0; i < dst_idxlist->count; i++) {
        dst_bucket_idxlist[i] = dst_idxlist->list[i] / world_size;
        if (dst_bucket_idxlist[i] >= world_size) dst_bucket_idxlist[i] = world_size - 1;
    }

    // send src_idxlist bucket info to the RD decomposition
    int dst_bucket[bucket_size];
    {
    	MPI_Request req[dst_idxlist->count+bucket_size];
        MPI_Status stat[dst_idxlist->count+bucket_size];
        int nreq = 0;
        // dst MPI ranks send info to bucket
        for (int i = 0; i < dst_idxlist->count; i++) {
            MPI_Isend(&world_rank, 1, MPI_INT, dst_bucket_idxlist[i], dst_idxlist->list[i], comm, &req[nreq]);
            nreq++;
        }

        // recv dst for each bucket
        for (int i = 0; i < bucket_size; i++) {
            MPI_Irecv(&dst_bucket[i], 1, MPI_INT, MPI_ANY_SOURCE, i+bucket_size*(world_rank), comm, &req[nreq]);
            nreq++;
        }

        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", dst_bucket[i]);
    printf("\n");
*/
    // At this point each bucket contains info about src and dst for each point within the bucket

    int idxlist_exch_dst[src_idxlist->count];
    {
        MPI_Request req[src_idxlist->count+bucket_size];
        MPI_Status stat[src_idxlist->count+bucket_size];
        int nreq = 0;
        // send dst info to MPI ranks
        for (int i = 0; i < bucket_size; i++) {
            MPI_Isend(&dst_bucket[i], 1, MPI_INT, src_bucket[i], i+bucket_size*(world_rank), comm, &req[nreq]);
            nreq++;
        }

        // MPI ranks receive the dst MPI proc for each idxlist point
        for (int i = 0; i < src_idxlist->count; i++) {
            MPI_Irecv(&idxlist_exch_dst[i], 1, MPI_INT, src_idxlist->list[i]/world_size, src_idxlist->list[i], comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d -- idxlist_exch_dst: ", world_rank);
    for (int i = 0; i < src_idxlist->count; i++)
        printf("%d ", idxlist_exch_dst[i]);
    printf("\n");
*/
    int idxlist_exch_src[dst_idxlist->count];
    {
        MPI_Request req[dst_idxlist->count+bucket_size];
        MPI_Status stat[dst_idxlist->count+bucket_size];
        int nreq = 0;
        // send src info to MPI ranks
        for (int i = 0; i < bucket_size; i++) {
            MPI_Isend(&src_bucket[i], 1, MPI_INT, dst_bucket[i], i+bucket_size*(world_rank), comm, &req[nreq]);
            nreq++;
        }

        // MPI ranks receive the dst MPI proc for each idxlist point
        for (int i = 0; i < dst_idxlist->count; i++) {
            MPI_Irecv(&idxlist_exch_src[i], 1, MPI_INT, dst_idxlist->list[i]/world_size, dst_idxlist->list[i], comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d -- idxlist_exch_src: ", world_rank);
    for (int i = 0; i < dst_idxlist->count; i++)
        printf("%d ", idxlist_exch_src[i]);
    printf("\n");
*/
    // group all info into data structure
    struct t_map map;
    map.comm = comm;

    // number of process the current rank has to send data to
    map.exch_send.count = 0;
    for (int i = 0; i < world_size; i++)
        for (int j = 0; j < src_idxlist->count; j++)
            if (idxlist_exch_dst[j] == i) {
                map.exch_send.count++;
                break;
            }

    // fill info about each send message (this should be significantly improved)
    map.exch_send.exch = (struct t_map_exch_per_rank*)malloc(map.exch_send.count * sizeof(struct t_map_exch_per_rank));
    int count = 0;
    for (int i = 0; i < world_size; i++) {
        int buffer_size = 0;
        for (int j = 0; j < src_idxlist->count; j++)
            if (idxlist_exch_dst[j] == i)
                buffer_size++;
        if (buffer_size > 0) {
            map.exch_send.exch[count].exch_rank = i;
            map.exch_send.exch[count].buffer_size = buffer_size;
            map.exch_send.exch[count].buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
            int buffer_idx = 0;
            for (int j = 0; j < src_idxlist->count; j++)
                if (idxlist_exch_dst[j] == i) {
                    map.exch_send.exch[count].buffer_idxlist[buffer_idx] = j;
                    buffer_idx++;
                }
            count++;
        }
    }

    // number of process the current rank has to receive data from
    map.exch_recv.count = 0;
    for (int i = 0; i < world_size; i++)
        for (int j = 0; j < dst_idxlist->count; j++)
            if (idxlist_exch_src[j] == i) {
                map.exch_recv.count++;
                break;
            }

    // fill info about each recv message (this should be significantly improved)
    map.exch_recv.exch = (struct t_map_exch_per_rank*)malloc(map.exch_recv.count * sizeof(struct t_map_exch_per_rank));
    count = 0;
    for (int i = 0; i < world_size; i++) {
        int buffer_size = 0;
        for (int j = 0; j < dst_idxlist->count; j++)
            if (idxlist_exch_src[j] == i)
                buffer_size++;
        if (buffer_size > 0) {
            map.exch_recv.exch[count].exch_rank = i;
            map.exch_recv.exch[count].buffer_size = buffer_size;
            map.exch_recv.exch[count].buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
            int buffer_idx = 0;
            for (int j = 0; j < dst_idxlist->count; j++)
                if (idxlist_exch_src[j] == i) {
                    map.exch_recv.exch[count].buffer_idxlist[buffer_idx] = j;
                    buffer_idx++;
                }
            count++;
        }
    }

    return map;

}