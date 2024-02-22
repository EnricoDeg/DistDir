#include "map.h"
#include "quicksort.h"
#include "mergesort.h"
#include <stdio.h>
#include <stdlib.h>

struct t_map * new_map(struct t_idxlist *src_idxlist, struct t_idxlist *dst_idxlist, MPI_Comm comm) {
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

/*
    if (src_idxlist->count > 0) {
        printf("%d -- src_bucket_idxlist: ", world_rank);
        for (int i=0; i < src_idxlist->count; i++)
            printf("%d ", src_bucket_idxlist[i]);
        printf("\n");
    }
*/

    // qsort src_bucket_idxlist
    // local copy (to be deleted)
    int src_bucket_idxlist_sort[src_idxlist->count];
    int src_idxlist_sort[src_idxlist->count];
    int src_idxlist_local[src_idxlist->count];
    if (src_idxlist->count > 0) {
        for (int i=0; i < src_idxlist->count; i++) {
            src_bucket_idxlist_sort[i] = src_bucket_idxlist[i];
            src_idxlist_sort[i] = src_idxlist->list[i];
            src_idxlist_local[i] = i;
        }
        quickSort(src_bucket_idxlist_sort, src_idxlist_sort, 0, src_idxlist->count - 1);
        quickSort(src_bucket_idxlist, src_idxlist_local, 0, src_idxlist->count - 1);

/*
        printf("%d -- src_bucket_idxlist_sort: ", world_rank);
        for (int i=0; i < src_idxlist->count; i++)
            printf("%d ", src_bucket_idxlist_sort[i]);
        printf("\n");

        printf("%d -- src_idxlist_sort: ", world_rank);
        for (int i=0; i < src_idxlist->count; i++)
            printf("%d ", src_idxlist_sort[i]);
        printf("\n");

        printf("%d -- src_idxlist_local: ", world_rank);
        for (int i=0; i < src_idxlist->count; i++)
            printf("%d ", src_idxlist_local[i]);
        printf("\n");
*/

    }

    int src_comm_ranks[world_size];
    for (int rank = 0; rank < world_size; rank++)
        src_comm_ranks[rank] = 0;
    if (src_idxlist->count > 0) {
        for (int i = 0; i < src_idxlist->count; i++)
            src_comm_ranks[src_bucket_idxlist_sort[i]] = 1;

/*
        printf("%d -- src_comm_ranks: ", world_rank);
        for (int rank = 0; rank < world_size; rank++)
            printf("%d ", src_comm_ranks[rank]);
        printf("\n");
*/

    }

    int src_size_ranks[world_size];
    for (int rank = 0; rank < world_size; rank++)
        src_size_ranks[rank] = 0;
    if (src_idxlist->count > 0) {
        for (int i = 0, offset=0, count = 1; i < src_idxlist->count; i++, count++) {
            if (src_bucket_idxlist_sort[i] != src_bucket_idxlist_sort[offset]) {
                count = 1;
                offset = i;
            }
            src_size_ranks[src_bucket_idxlist_sort[offset]] = count;
        }       
/*
        printf("%d -- src_size_ranks: ", world_rank);
        for (int rank = 0; rank < world_size; rank++)
            printf("%d ", src_size_ranks[rank]);
        printf("\n");
*/
    }

    // number of processes each bucket receive data from
    int src_count_recv;
    {
        int recv_count[world_size];
        for (int i=0; i<world_size; i++)
            recv_count[i] = 1;
        MPI_Reduce_scatter(src_comm_ranks, &src_count_recv, recv_count, MPI_INT, MPI_SUM, comm);
//        printf("%d -- src_count_recv: ", world_rank);
//        printf("%d\n", src_count_recv);
    }

    // size of each message that each bucket receive
    int src_msg_size_recv[src_count_recv];
    {
        MPI_Request req[src_idxlist->count+bucket_size];
        MPI_Status stat[src_idxlist->count+bucket_size];
        int nreq = 0;

        for (int i=0; i<world_size; i++)
            if (src_size_ranks[i] > 0) {
                MPI_Isend(&src_size_ranks[i], 1, MPI_INT, i, world_rank+bucket_size*i, comm, &req[nreq]);
                nreq++;
            }

        for (int i=0; i<src_count_recv; i++) {
            MPI_Irecv(&src_msg_size_recv[i], 1, MPI_INT, MPI_ANY_SOURCE, i+bucket_size*world_rank, comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d -- src_msg_size_recv: ", world_rank);
    for (int i = 0; i < src_count_recv; i++)
        printf("%d ", src_msg_size_recv[i]);
    printf("\n");
*/

    // source of each message
    int src_src_recv[src_count_recv];
    {
        MPI_Request req[src_idxlist->count+bucket_size];
        MPI_Status stat[src_idxlist->count+bucket_size];
        int nreq = 0;

        for (int i=0; i<world_size; i++)
            if (src_size_ranks[i] > 0) {
                MPI_Isend(&world_rank, 1, MPI_INT, i, world_rank+bucket_size*i, comm, &req[nreq]);
                nreq++;
            }

        for (int i=0; i<src_count_recv; i++) {
            MPI_Irecv(&src_src_recv[i], 1, MPI_INT, MPI_ANY_SOURCE, i+bucket_size*world_rank, comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d -- src_src_recv: ", world_rank);
    for (int i = 0; i < src_count_recv; i++)
        printf("%d ", src_src_recv[i]);
    printf("\n");
*/

    // gather src_bucket info
    int src_bucket_sort[bucket_size];
    {
        MPI_Request req[src_idxlist->count+bucket_size];
        MPI_Status stat[src_idxlist->count+bucket_size];
        int nreq = 0;

        // recv src for each bucket
        int offset = 0;
        for (int i = 0; i < src_count_recv; i++) {
            MPI_Irecv(&src_bucket_sort[offset], src_msg_size_recv[i], MPI_INT, src_src_recv[i], i+bucket_size*(world_rank), comm, &req[nreq]);
            offset +=  src_msg_size_recv[i];
            nreq++;
        }

        //  MPI ranks send info to src bucket
        for (int i = 0; i < world_size; i++) {
            if (src_size_ranks[i] > 0) {
                int myrank_arr[src_size_ranks[i]];
                for (int j=0; j<src_size_ranks[i]; j++)
                    myrank_arr[j] = world_rank;
                MPI_Send(myrank_arr, src_size_ranks[i], MPI_INT, i, world_rank+bucket_size*i, comm);
                // nreq++;
            }
        }

        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d -- src_bucket_sort: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", src_bucket_sort[i]);
    printf("\n");
*/

    int src_rank_bucket_idxlist_sort[bucket_size];
    {
        MPI_Request req[world_size*bucket_size];
        MPI_Status stat[world_size*bucket_size];
        int nreq = 0;

        //  MPI ranks send info to src bucket
        for (int i = 0, offset = 0; i < world_size; i++) {
            if (src_size_ranks[i] > 0) {
                MPI_Isend(&src_idxlist_sort[offset], src_size_ranks[i], MPI_INT,
                           i, world_rank+bucket_size*i, comm, &req[nreq]);
                offset += src_size_ranks[i];
                nreq++;
            }
        }

        // recv src for each bucket
        int offset = 0;
        for (int i = 0; i < src_count_recv; i++) {
            MPI_Irecv(&src_rank_bucket_idxlist_sort[offset], src_msg_size_recv[i], MPI_INT,
                       src_src_recv[i], i+bucket_size*(world_rank), comm, &req[nreq]);
            offset +=  src_msg_size_recv[i];
            nreq++;
        }

        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d -- src_rank_bucket_idxlist_sort: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", src_rank_bucket_idxlist_sort[i]);
    printf("\n");
*/

// ---------------------------------------------------------------

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
    // ------------------------------------------------------

    int dst_bucket_idxlist[dst_idxlist->count];
    for (int i=0; i < dst_idxlist->count; i++) {
        dst_bucket_idxlist[i] = dst_idxlist->list[i] / world_size;
        if (dst_bucket_idxlist[i] >= world_size) dst_bucket_idxlist[i] = world_size - 1;
    }

    // qsort dst_bucket_idxlist
    // local copy (to be deleted)
    int dst_bucket_idxlist_sort[dst_idxlist->count];
    int dst_idxlist_sort[dst_idxlist->count];
    int dst_idxlist_local[dst_idxlist->count];
    if (dst_idxlist->count > 0) {
        for (int i=0; i < dst_idxlist->count; i++) {
            dst_bucket_idxlist_sort[i] = dst_bucket_idxlist[i];
            dst_idxlist_sort[i] = dst_idxlist->list[i];
            dst_idxlist_local[i] = i;
        }

        quickSort(dst_bucket_idxlist_sort, dst_idxlist_sort, 0, dst_idxlist->count - 1);
        quickSort(dst_bucket_idxlist, dst_idxlist_local, 0, dst_idxlist->count - 1);
/*
        printf("%d -- dst_bucket_idxlist_sort: ", world_rank);
        for (int i=0; i < dst_idxlist->count; i++)
            printf("%d ", dst_bucket_idxlist_sort[i]);
        printf("\n");

        printf("%d -- dst_idxlist_sort: ", world_rank);
        for (int i=0; i < dst_idxlist->count; i++)
            printf("%d ", dst_idxlist_sort[i]);
        printf("\n");

        printf("%d -- dst_idxlist_local: ", world_rank);
        for (int i=0; i < dst_idxlist->count; i++)
            printf("%d ", dst_idxlist_local[i]);
        printf("\n");
*/
    }

    int dst_comm_ranks[world_size];
    for (int rank = 0; rank < world_size; rank++)
        dst_comm_ranks[rank] = 0;
    if (dst_idxlist->count > 0) {
        for (int i = 0; i < dst_idxlist->count; i++)
            dst_comm_ranks[dst_bucket_idxlist_sort[i]] = 1;
/*
        printf("%d -- dst_comm_ranks: ", world_rank);
        for (int rank = 0; rank < world_size; rank++)
            printf("%d ", dst_comm_ranks[rank]);
        printf("\n");
*/
    }

    int dst_size_ranks[world_size];
    for (int rank = 0; rank < world_size; rank++)
        dst_size_ranks[rank] = 0;
    if (dst_idxlist->count > 0) {
        for (int i = 0, offset=0, count = 1; i < dst_idxlist->count; i++, count++) {
            if (dst_bucket_idxlist_sort[i] != dst_bucket_idxlist_sort[offset]) {
                count = 1;
                offset = i;
            }
            dst_size_ranks[dst_bucket_idxlist_sort[offset]] = count;
        }
/*
        printf("%d -- dst_size_ranks: ", world_rank);
        for (int rank = 0; rank < world_size; rank++)
            printf("%d ", dst_size_ranks[rank]);
        printf("\n");
*/
    }

    int dst_count_recv;
    {
        int recv_count[world_size];
        for (int i=0; i<world_size; i++)
            recv_count[i] = 1;
        MPI_Reduce_scatter(dst_comm_ranks, &dst_count_recv, recv_count, MPI_INT, MPI_SUM, comm);
//        printf("%d -- dst_count_recv: ", world_rank);
//        printf("%d\n", dst_count_recv);
    }

    // size of each message that each bucket receive
    int dst_msg_size_recv[dst_count_recv];
    {
        MPI_Request req[world_size*bucket_size];
        MPI_Status stat[world_size*bucket_size];
        int nreq = 0;

        for (int i=0; i<world_size; i++)
            if (dst_size_ranks[i] > 0) {
                MPI_Isend(&dst_size_ranks[i], 1, MPI_INT, i, i+bucket_size*(i+1), comm, &req[nreq]);
                nreq++;
            }

        for (int i=0; i<dst_count_recv; i++) {
            MPI_Irecv(&dst_msg_size_recv[i], 1, MPI_INT, MPI_ANY_SOURCE, world_rank+bucket_size*(world_rank+1), comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);
    }
/*
    printf("%d -- dst_msg_size_recv: ", world_rank);
    for (int i = 0; i < dst_count_recv; i++)
        printf("%d ", dst_msg_size_recv[i]);
    printf("\n");
*/

    // source of each message
    int dst_src_recv[dst_count_recv];
    {
        MPI_Request req[dst_idxlist->count+bucket_size];
        MPI_Status stat[dst_idxlist->count+bucket_size];
        int nreq = 0;

        for (int i=0; i<world_size; i++)
            if (dst_size_ranks[i] > 0) {
                MPI_Isend(&world_rank, 1, MPI_INT, i, i+bucket_size*(i+1), comm, &req[nreq]);
                nreq++;
            }

        for (int i=0; i<dst_count_recv; i++) {
            MPI_Irecv(&dst_src_recv[i], 1, MPI_INT, MPI_ANY_SOURCE, world_rank+bucket_size*(world_rank+1), comm, &req[nreq]);
            nreq++;
        }
        MPI_Waitall(nreq, req, stat);
    }
    int dummy[bucket_size];
    quickSort(dst_src_recv, dummy, 0, dst_count_recv-1);

/*
    printf("%d -- dst_src_recv: ", world_rank);
    for (int i = 0; i < dst_count_recv; i++)
        printf("%d ", dst_src_recv[i]);
    printf("\n");
*/

    // gather dst_bucket info
    int dst_bucket_sort[bucket_size];
    {
        MPI_Request req[dst_idxlist->count+bucket_size];
        MPI_Status stat[dst_idxlist->count+bucket_size];
        int nreq = 0;

        // recv dst for each bucket
        int offset = 0;
        for (int i = 0; i < dst_count_recv; i++) {
            MPI_Irecv(&dst_bucket_sort[offset], dst_msg_size_recv[i], MPI_INT, dst_src_recv[i], world_rank+bucket_size*(world_rank+1), comm, &req[nreq]);
            offset +=  dst_msg_size_recv[i];
            nreq++;
        }

        //  MPI ranks send info to dst bucket
        for (int i = 0; i < world_size; i++) {
            if (dst_size_ranks[i] > 0) {
                int myrank_arr[dst_size_ranks[i]];
                for (int j=0; j<dst_size_ranks[i]; j++)
                    myrank_arr[j] = world_rank;
                MPI_Send(myrank_arr, dst_size_ranks[i], MPI_INT, i, i+bucket_size*(i+1), comm);
            }
        }

        MPI_Waitall(nreq, req, stat);
    }

/*
    printf("%d -- dst_bucket_sort: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", dst_bucket_sort[i]);
    printf("\n");
*/

    int dst_rank_bucket_idxlist_sort[bucket_size];
    {
        MPI_Request req[world_size*bucket_size];
        MPI_Status stat[world_size*bucket_size];
        int nreq = 0;

        // recv src for each bucket
        for (int i = 0, offset=0; i < dst_count_recv; i++) {
            MPI_Irecv(&dst_rank_bucket_idxlist_sort[offset], dst_msg_size_recv[i], MPI_INT, dst_src_recv[i], world_rank+bucket_size*(world_rank+1), comm, &req[nreq]);
            offset +=  dst_msg_size_recv[i];
            nreq++;
        }

        //  MPI ranks send info to src bucket
        for (int i = 0, offset = 0; i < world_size; i++) {
            if (dst_size_ranks[i] > 0) {
                MPI_Isend(&dst_idxlist_sort[offset], dst_size_ranks[i], MPI_INT, i, i+bucket_size*(i+1), comm, &req[nreq]);
                offset += dst_size_ranks[i];
                nreq++;
            }
        }

        MPI_Waitall(nreq, req, stat);
    }

/*
    printf("----> %d -- dst_rank_bucket_idxlist_sort: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", dst_rank_bucket_idxlist_sort[i]);
    printf("\n");
*/

    // ==========================================================================================
    // At this point each bucket contains info about src and dst for each point within the bucket
    // ==========================================================================================
    int dst_rank_bucket_idxlist_sort_src[bucket_size];
    int dst_bucket_sort_src[bucket_size];
    for (int i=0; i<bucket_size; i++) {
        for (int j=0; j<bucket_size; j++) {
            if (dst_rank_bucket_idxlist_sort[i] == src_rank_bucket_idxlist_sort[j]) {
                dst_bucket_sort_src[i] = dst_bucket_sort[j];
            }
        }
    }

/*
    printf("%d  --  dst_bucket_sort_src: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", dst_bucket_sort_src[i]);
    printf("\n");
*/

    int rank_exch_dst_sort[src_idxlist->count];
    {
        MPI_Request req[world_size*world_size];
        MPI_Status stat[world_size*world_size];
        int nreq = 0;
        // send dst info to MPI ranks
        for (int i = 0, offset=0; i < src_count_recv; i++) {
            MPI_Isend(&dst_bucket_sort_src[offset], src_msg_size_recv[i], MPI_INT, src_src_recv[i], world_rank+world_size*(1+src_src_recv[i]), comm, &req[nreq]);
            offset += src_msg_size_recv[i];
            nreq++;
        }

        // MPI ranks receive the dst MPI proc for each idxlist point
        for (int i = 0, offset = 0; i < world_size; i++) {
            if (src_size_ranks[i] > 0) {
                MPI_Irecv(&rank_exch_dst_sort[offset], src_size_ranks[i], MPI_INT, i, i+world_size*(1+world_rank), comm, &req[nreq]);
                offset += src_size_ranks[i];
                nreq++;
            }
        }
        MPI_Waitall(nreq, req, stat);
    }

/*
    printf("%d before sort-- rank_exch_dst_sort: ", world_rank);
    for (int i=0; i<src_idxlist->count; i++)
        printf("%d ", rank_exch_dst_sort[i]);
    printf("\n");
*/

    if (src_idxlist->count > 0) mergeSort(rank_exch_dst_sort, src_idxlist_local, 0, src_idxlist->count - 1);

/*
    printf("%d after sort-- rank_exch_dst_sort: ", world_rank);
    for (int i=0; i<src_idxlist->count; i++)
        printf("%d ", rank_exch_dst_sort[i]);
    printf("\n");

    printf("%d after sort-- src_idxlist_local: ", world_rank);
    for (int i=0; i<src_idxlist->count; i++)
        printf("%d ", src_idxlist_local[i]);
    printf("\n");
*/

    int src_rank_bucket_idxlist_sort_dst[bucket_size];
    int src_bucket_sort_dst[bucket_size];
    for (int i=0; i<bucket_size; i++) {
        for (int j=0; j<bucket_size; j++) {
            if (src_rank_bucket_idxlist_sort[i] == dst_rank_bucket_idxlist_sort[j]) {
                src_bucket_sort_dst[i] = src_bucket_sort[j];
            }
        }
    }

/*
    printf("%d  --  src_bucket_sort_dst: ", world_rank);
    for (int i = 0; i < bucket_size; i++)
        printf("%d ", src_bucket_sort_dst[i]);
    printf("\n");
*/

    int rank_exch_src_sort[dst_idxlist->count];
    {
        MPI_Request req[world_size*world_size];
        MPI_Status stat[world_size*world_size];
        int nreq = 0;
        // send dst info to MPI ranks
        for (int i = 0, offset=0; i < dst_count_recv; i++) {
            MPI_Isend(&src_bucket_sort_dst[offset], dst_msg_size_recv[i], MPI_INT, dst_src_recv[i], world_rank+world_size*(1+dst_src_recv[i]), comm, &req[nreq]);
            offset += dst_msg_size_recv[i];
            nreq++;
        }

        // MPI ranks receive the dst MPI proc for each idxlist point
        for (int i = 0, offset = 0; i < world_size; i++) {
            if (dst_size_ranks[i] > 0) {
                MPI_Irecv(&rank_exch_src_sort[offset], dst_size_ranks[i], MPI_INT, i, i+world_size*(1+world_rank), comm, &req[nreq]);
                offset += dst_size_ranks[i];
                nreq++;
            }
        }
        MPI_Waitall(nreq, req, stat);
    }

/*
    printf("%d before sort-- rank_exch_src_sort: ", world_rank);
    for (int i=0; i<dst_idxlist->count; i++)
        printf("%d ", rank_exch_src_sort[i]);
    printf("\n");
*/

    if (dst_idxlist->count > 0) mergeSort(rank_exch_src_sort, dst_idxlist_local, 0, dst_idxlist->count - 1);
    
/*
    printf("%d after sort-- rank_exch_src_sort: ", world_rank);
    for (int i=0; i<dst_idxlist->count; i++)
        printf("%d ", rank_exch_src_sort[i]);
    printf("\n");

    printf("%d after sort-- dst_idxlist_local: ", world_rank);
    for (int i=0; i<dst_idxlist->count; i++)
        printf("%d ", dst_idxlist_local[i]);
    printf("\n");
*/

    // group all info into data structure
    struct t_map *map;

    map = (struct t_map *)malloc(sizeof(struct t_map));
    map->comm = comm;
    map->exch_send = (struct t_map_exch *)malloc(sizeof(struct t_map_exch));
    map->exch_recv = (struct t_map_exch *)malloc(sizeof(struct t_map_exch));

    // number of procs the current rank has to send data to
    if (src_idxlist->count > 0) {
        map->exch_send->count = 1;
        for (int i = 0, offset=0; i < src_idxlist->count; i++)
            if (rank_exch_dst_sort[i] != rank_exch_dst_sort[offset]) {
                map->exch_send->count++;
                offset = i;
            }
    } else {
        map->exch_send->count = 0;
    }

    // fill info about each send message
    map->exch_send->exch = (struct t_map_exch_per_rank**)malloc(map->exch_send->count * sizeof(struct t_map_exch_per_rank*));

    if (src_idxlist->count > 0) {
        int count = 0;
        int offset = 0;
        int buffer_size = 0;
        for (int i = 0; i < src_idxlist->count; i++) {
            if (rank_exch_dst_sort[i] != rank_exch_dst_sort[offset]) {
                if (buffer_size > 0)
                    map->exch_send->exch[count] = (struct t_map_exch_per_rank *)malloc(sizeof(struct t_map_exch_per_rank));
                map->exch_send->exch[count]->exch_rank = rank_exch_dst_sort[offset];
                map->exch_send->exch[count]->buffer_size = buffer_size;
                map->exch_send->exch[count]->buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
                for (int j=offset; j<i; j++)
                    map->exch_send->exch[count]->buffer_idxlist[j-offset] = src_idxlist_local[j];
                offset = i;
                buffer_size = 0;
                count++;
            }
            buffer_size++;
        }
        if (buffer_size > 0)
            map->exch_send->exch[count] = (struct t_map_exch_per_rank *)malloc(sizeof(struct t_map_exch_per_rank));
        map->exch_send->exch[count]->exch_rank = rank_exch_dst_sort[offset];
        map->exch_send->exch[count]->buffer_size = buffer_size;
        map->exch_send->exch[count]->buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
        for (int j=offset; j<src_idxlist->count; j++)
            map->exch_send->exch[count]->buffer_idxlist[j-offset] = src_idxlist_local[j];
    }

    // number of procs the current rank has to send data to
    if (dst_idxlist->count > 0) {
        map->exch_recv->count = 1;
        for (int i = 0, offset=0; i < dst_idxlist->count; i++)
            if (rank_exch_src_sort[i] != rank_exch_src_sort[offset]) {
                map->exch_recv->count++;
                offset = i;
            }
    } else {
        map->exch_recv->count = 0;
    }

    // fill info about each recv message
    map->exch_recv->exch = (struct t_map_exch_per_rank**)malloc(map->exch_recv->count * sizeof(struct t_map_exch_per_rank*));

    if (dst_idxlist->count > 0) {
        int count = 0;
        int offset = 0;
        int buffer_size = 0;
        for (int i = 0; i < dst_idxlist->count; i++) {
            if (rank_exch_src_sort[i] != rank_exch_src_sort[offset]) {
                if (buffer_size > 0)
                    map->exch_recv->exch[count] = (struct t_map_exch_per_rank *)malloc(sizeof(struct t_map_exch_per_rank));
                map->exch_recv->exch[count]->exch_rank = rank_exch_src_sort[offset];
                map->exch_recv->exch[count]->buffer_size = buffer_size;
                map->exch_recv->exch[count]->buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
                for (int j=offset; j<i; j++)
                    map->exch_recv->exch[count]->buffer_idxlist[j-offset] = dst_idxlist_local[j];
                offset = i;
                buffer_size = 0;
                count++;
            }
            buffer_size++;
        }
        if (buffer_size > 0)
            map->exch_recv->exch[count] = (struct t_map_exch_per_rank *)malloc(sizeof(struct t_map_exch_per_rank));
        map->exch_recv->exch[count]->exch_rank = rank_exch_src_sort[offset];
        map->exch_recv->exch[count]->buffer_size = buffer_size;
        map->exch_recv->exch[count]->buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
        for (int j=offset; j<dst_idxlist->count; j++)
            map->exch_recv->exch[count]->buffer_idxlist[j-offset] = dst_idxlist_local[j];
    }

    return map;

}

void delete_map(struct t_map *map) {
	// map send info
    for (int count = 0; count < map->exch_send->count; count++) {
        if (map->exch_send->exch[count]->buffer_size > 0) {
            free(map->exch_send->exch[count]->buffer_idxlist);
        }
        free(map->exch_send->exch[count]);
    }
    free(map->exch_send->exch);
    free(map->exch_send);

    // map recv info
    for (int count = 0; count < map->exch_recv->count; count++) {
        if (map->exch_recv->exch[count]->buffer_size > 0) {
            free(map->exch_recv->exch[count]->buffer_idxlist);
        }
        free(map->exch_recv->exch[count]);
    }
    free(map->exch_recv->exch);
    free(map->exch_recv);

    free(map);
}