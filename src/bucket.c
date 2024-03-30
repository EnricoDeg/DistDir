/*
 * @file bucket.c
 *
 * @copyright Copyright (C) 2024 Enrico Degregori <enrico.degregori@gmail.com>
 *
 * @author Enrico Degregori <enrico.degregori@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.

 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mpi.h"

#include "bucket.h"
#include "mergesort.h"
#include "check.h"

/**
 * @brief Assign each element of a given index list to a bucket.
 * 
 * @details The assignment of the index list elements to a bucket is straightforward.
 *          Given N total global indices and M total buckets, bucket 0 owns elements 0 - N/M-1,
 *          bucket 1 owns elements N/M - 2*N/M-1, etc.
 *  
 * @param[out] bucket_idxlist    integer array with the bucket location for each element of the index list
 * @param[in]  idxlist           integer array with the values of the index list (global indices)
 * @param[in]  idxlist_size      size of the idxlist array
 * @param[in]  nbuckets          number of buckets
 * 
 * @ingroup bucket
 */
static void assign_idxlist_elements_to_buckets(int *bucket_idxlist, const int *idxlist, int idxlist_size, int nbuckets) {

#ifdef ERROR_CHECK
	assert(bucket_idxlist != NULL);
#endif

	for (int i = 0; i < idxlist_size; i++) {
		bucket_idxlist[i] = idxlist[i] / nbuckets;
		if (bucket_idxlist[i] >= nbuckets)
			bucket_idxlist[i] = nbuckets - 1;
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
		check_mpi( MPI_Reduce_scatter(src_comm_ranks, &count_recv, recv_count, MPI_INT, MPI_SUM, comm) );
	}

	return count_recv;
}

static void get_receivers_bucket(int *bucket_src_recv, const int *bucket_size_ranks, 
                                 int  bucket_count_recv, int bucket_size, int idxlist_count, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_count+bucket_size];
	MPI_Status stat[idxlist_count+bucket_size];
	int nreq = 0;

	for (int i=0; i<world_size; i++)
		if (bucket_size_ranks[i] > 0) {
			check_mpi( MPI_Isend(&world_rank, 1, MPI_INT, i, i+bucket_size*(i+1), comm, &req[nreq]) );
			nreq++;
		}

	for (int i=0; i<bucket_count_recv; i++) {
		check_mpi( MPI_Irecv(&bucket_src_recv[i], 1, MPI_INT, MPI_ANY_SOURCE,
		                      world_rank+bucket_size*(world_rank+1), comm, &req[nreq]) );
		nreq++;
	}
	check_mpi( MPI_Waitall(nreq, req, stat) );

	// sort by process number
	mergeSort(bucket_src_recv, 0, bucket_count_recv-1);
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

void map_idxlist_to_RD_decomp(t_bucket *bucket, t_idxlist *idxlist, int *idxlist_local, int nbuckets, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

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
	get_receivers_bucket(bucket->src_recv, bucket->size_ranks, 
                         bucket->count_recv, bucket->size, idxlist->count, comm);

	// size of each message that each bucket receive
	if (bucket->count_recv > 0)
		bucket->msg_size_recv = (int *)malloc(bucket->count_recv*sizeof(int));
	{
		MPI_Request req[idxlist->count+bucket->size];
		MPI_Status stat[idxlist->count+bucket->size];
		int nreq = 0;

		for (int i=0; i<world_size; i++)
			if (bucket->size_ranks[i] > 0) {
				check_mpi( MPI_Isend(&bucket->size_ranks[i], 1, MPI_INT, i,
				                      world_rank+bucket->size*i, comm, &req[nreq]) );
				nreq++;
			}

		for (int i=0; i<bucket->count_recv; i++) {
			check_mpi( MPI_Irecv(&bucket->msg_size_recv[i], 1, MPI_INT, bucket->src_recv[i],
			                      bucket->src_recv[i]+bucket->size*world_rank, comm, &req[nreq]) );
			nreq++;
		}
		check_mpi( MPI_Waitall(nreq, req, stat) );
	}

	// gather src_bucket info
	{
		MPI_Request req[idxlist->count+bucket->size];
		MPI_Status stat[idxlist->count+bucket->size];
		int nreq = 0;

		// recv src for each bucket
		int offset = 0;
		for (int i = 0; i < bucket->count_recv; i++) {
			check_mpi( MPI_Irecv(&bucket->ranks[offset], bucket->msg_size_recv[i], MPI_INT, bucket->src_recv[i],
			          world_rank+bucket->size*(world_rank+1), comm, &req[nreq]) );
			offset +=  bucket->msg_size_recv[i];
			nreq++;
		}

		//  MPI ranks send info to src bucket
		for (int i = 0; i < world_size; i++) {
			if (bucket->size_ranks[i] > 0) {
				int myrank_arr[bucket->size_ranks[i]];
				for (int j=0; j<bucket->size_ranks[i]; j++)
					myrank_arr[j] = world_rank;
				check_mpi( MPI_Send(myrank_arr, bucket->size_ranks[i], MPI_INT, i, i+bucket->size*(i+1), comm) );
			}
		}

		check_mpi( MPI_Waitall(nreq, req, stat) );
	}


	{
		MPI_Request req[world_size*bucket->size];
		MPI_Status stat[world_size*bucket->size];
		int nreq = 0;

		//  MPI ranks send info to src bucket
		for (int i = 0, offset = 0; i < world_size; i++) {
			if (bucket->size_ranks[i] > 0) {
				check_mpi( MPI_Isend(&src_idxlist_sort[offset], bucket->size_ranks[i], MPI_INT,
				                      i, i+bucket->size*(i+1), comm, &req[nreq]) );
				offset += bucket->size_ranks[i];
				nreq++;
			}
		}

		// recv src for each bucket
		int offset = 0;
		for (int i = 0; i < bucket->count_recv; i++) {
			check_mpi( MPI_Irecv(&bucket->idxlist[offset], bucket->msg_size_recv[i], MPI_INT,
			                      bucket->src_recv[i], world_rank+bucket->size*(world_rank+1), comm, &req[nreq]) );
			offset +=  bucket->msg_size_recv[i];
			nreq++;
		}

		check_mpi( MPI_Waitall(nreq, req, stat) );
	}
}

void map_RD_decomp_to_idxlist(t_bucket *src_bucket, t_bucket *dst_bucket, int *idxlist_local,
                              int idxlist_size, int nbuckets, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	int dst_bucket_sort_src[src_bucket->size];
	for (int i=0; i<src_bucket->size; i++)
		for (int j=0; j<src_bucket->size; j++)
			if (dst_bucket->idxlist[i] == src_bucket->idxlist[j])
				dst_bucket_sort_src[i] = dst_bucket->ranks[j];

	src_bucket->rank_exch = (int *)malloc(idxlist_size*sizeof(int));
	{
		MPI_Request req[nbuckets*nbuckets];
		MPI_Status stat[nbuckets*nbuckets];
		int nreq = 0;
		// send dst info to MPI ranks
		for (int i = 0, offset=0; i < src_bucket->count_recv; i++) {
			check_mpi( MPI_Isend(&dst_bucket_sort_src[offset], src_bucket->msg_size_recv[i], MPI_INT,
			                      src_bucket->src_recv[i], world_rank+nbuckets*(1+src_bucket->src_recv[i]),
			                      comm, &req[nreq]) );
			offset += src_bucket->msg_size_recv[i];
			nreq++;
		}

		// MPI ranks receive the dst MPI proc for each idxlist point
		for (int i = 0, offset = 0; i < nbuckets; i++) {
			if (src_bucket->size_ranks[i] > 0) {
				check_mpi( MPI_Irecv(&src_bucket->rank_exch[offset], src_bucket->size_ranks[i], MPI_INT, i,
				                      i+nbuckets*(1+world_rank), comm, &req[nreq]) );
				offset += src_bucket->size_ranks[i];
				nreq++;
			}
		}
		check_mpi( MPI_Waitall(nreq, req, stat) );
	}

	if (idxlist_size > 0) mergeSort_with_idx(src_bucket->rank_exch, idxlist_local, 0, idxlist_size - 1);
}