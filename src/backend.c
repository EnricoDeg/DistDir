/*
 * @file backend.c
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

#include "backend.h"
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
void assign_idxlist_elements_to_buckets(int *bucket_idxlist, const int *idxlist,
                                        int bucket_min_size,
                                        int idxlist_size, int nbuckets) {

#ifdef ERROR_CHECK
	assert(bucket_idxlist != NULL);
#endif

	for (int i = 0; i < idxlist_size; i++) {
		bucket_idxlist[i] = idxlist[i] / bucket_min_size;
		if (bucket_idxlist[i] >= nbuckets)
			bucket_idxlist[i] = nbuckets - 1;
	}
}


/**
 * @brief Return the number of processes that send information to this bucket.
 * 
 * @details TODO
 * 
 * @param[in] bucket_idxlist    integer array with the bucket location for each element of the index list
 * @param[in] nbuckets          number of buckets
 * @param[in] idxlist_size      size of the idxlist array
 * @param[in] comm              MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @ingroup bucket
 */
int get_n_procs_send_to_bucket(const int *bucket_idxlist, int nbuckets, int idxlist_size, MPI_Comm comm) {

	// bucket ID (rank) to send info to
	int send_to_buckets[nbuckets];
	for (int i = 0; i < nbuckets; i++)
		send_to_buckets[i] = 0;

	if (idxlist_size > 0)
		for (int i = 0; i < idxlist_size; i++)
			send_to_buckets[bucket_idxlist[i]] = 1;

	// number of processes each bucket receive data from
	int n_procs_sending_to_bucket;
	{
		int recv_count[nbuckets];
		for (int i = 0; i < nbuckets; i++)
			recv_count[i] = 1;
		check_mpi( MPI_Reduce_scatter(send_to_buckets, &n_procs_sending_to_bucket, recv_count, MPI_INT, MPI_SUM, comm) );
	}

	return n_procs_sending_to_bucket;
}

void num_indices_to_send_to_each_bucket(int *size_ranks, int *bucket_idxlist, int idxlist_size, int nranks) {

	// Initialize to zero
	for (int rank = 0; rank < nranks; rank++)
		size_ranks[rank] = 0;

	if (idxlist_size > 0) {
		for (int i = 0, offset = 0, count = 1; i < idxlist_size; i++, count++) {
			if (bucket_idxlist[i] != bucket_idxlist[offset]) {
				count = 1;
				offset = i;
			}
			size_ranks[bucket_idxlist[offset]] = count;
		}
	}
}

void get_receivers_bucket(int *bucket_src_recv, const int *bucket_size_ranks, 
                          int  bucket_count_recv, int bucket_max_size, int idxlist_count, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_count+bucket_max_size];
	MPI_Status stat[idxlist_count+bucket_max_size];
	int nreq = 0;

	for (int i=0; i<world_size; i++)
		if (bucket_size_ranks[i] > 0) {
			check_mpi( MPI_Isend(&world_rank, 1, MPI_INT, i, i+bucket_max_size*(i+1), comm, &req[nreq]) );
			nreq++;
		}

	for (int i=0; i<bucket_count_recv; i++) {
		check_mpi( MPI_Irecv(&bucket_src_recv[i], 1, MPI_INT, MPI_ANY_SOURCE,
		                      world_rank+bucket_max_size*(world_rank+1), comm, &req[nreq]) );
		nreq++;
	}
	check_mpi( MPI_Waitall(nreq, req, stat) );

	// sort by process number
	mergeSort(bucket_src_recv, 0, bucket_count_recv-1);
}

void get_n_indices_from_each_process(int *bucket_msg_size_recv, 
                                     const int *bucket_size_ranks, const int *bucket_src_recv,
                                     int bucket_count_recv, int bucket_max_size,
                                     int idxlist_count, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_count+bucket_max_size];
	MPI_Status stat[idxlist_count+bucket_max_size];
	int nreq = 0;

	for (int i=0; i<world_size; i++)
		if (bucket_size_ranks[i] > 0) {
			check_mpi( MPI_Isend(&bucket_size_ranks[i], 1, MPI_INT, i,
			                      world_rank+bucket_max_size*i, comm, &req[nreq]) );
			nreq++;
		}

	for (int i=0; i<bucket_count_recv; i++) {
		check_mpi( MPI_Irecv(&bucket_msg_size_recv[i], 1, MPI_INT, bucket_src_recv[i],
		                      bucket_src_recv[i]+bucket_max_size*world_rank, comm, &req[nreq]) );
		nreq++;
	}
	check_mpi( MPI_Waitall(nreq, req, stat) );
}

void bucket_idxlist_procs(int *bucket_ranks,
                          const int *bucket_size_ranks, const int *bucket_msg_size_recv,
                          const int *bucket_src_recv,
                          int bucket_count_recv, int bucket_max_size,
                          int idxlist_count, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_count+bucket_max_size];
	MPI_Status stat[idxlist_count+bucket_max_size];
	int nreq = 0;

	// recv bucket idxlist procs
	int offset = 0;
	for (int i = 0; i < bucket_count_recv; i++) {
		check_mpi( MPI_Irecv(&bucket_ranks[offset], bucket_msg_size_recv[i], MPI_INT, bucket_src_recv[i],
		                      world_rank + bucket_max_size * (world_rank + 1), comm, &req[nreq]) );
		offset +=  bucket_msg_size_recv[i];
		nreq++;
	}

	//  MPI ranks send ID info to buckets
	for (int i = 0; i < world_size; i++) {
		if (bucket_size_ranks[i] > 0) {
			int myrank_arr[bucket_size_ranks[i]];
			for (int j = 0; j < bucket_size_ranks[i]; j++)
				myrank_arr[j] = world_rank;
			check_mpi( MPI_Send(myrank_arr, bucket_size_ranks[i], MPI_INT, i, i + bucket_max_size * (i + 1), comm) );
		}
	}

	check_mpi( MPI_Waitall(nreq, req, stat) );
}

void bucket_idxlist_elements(int *bucket_idxlist,
                             const int *src_idxlist_sort,
                             const int *bucket_size_ranks, const int *bucket_msg_size_recv,
                             const int *bucket_src_recv,
                             int bucket_count_recv, int bucket_max_size,
                             int idxlist_count, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_count+bucket_max_size];
	MPI_Status stat[idxlist_count+bucket_max_size];
	int nreq = 0;

	//  MPI ranks send info to src bucket
	for (int i = 0, offset = 0; i < world_size; i++) {
		if (bucket_size_ranks[i] > 0) {
			check_mpi( MPI_Isend(&src_idxlist_sort[offset], bucket_size_ranks[i], MPI_INT,
			                      i, i+bucket_max_size*(i+1), comm, &req[nreq]) );
			offset += bucket_size_ranks[i];
			nreq++;
		}
	}

	// recv src for each bucket
	int offset = 0;
	for (int i = 0; i < bucket_count_recv; i++) {
		check_mpi( MPI_Irecv(&bucket_idxlist[offset], bucket_msg_size_recv[i], MPI_INT,
		                      bucket_src_recv[i], world_rank+bucket_max_size*(world_rank+1), comm, &req[nreq]) );
		offset +=  bucket_msg_size_recv[i];
		nreq++;
	}

	check_mpi( MPI_Waitall(nreq, req, stat) );
}