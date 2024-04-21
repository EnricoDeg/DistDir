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

void assign_idxlist_elements_to_buckets(      int *bucket_idxlist,
                                        const int *idxlist       ,
                                              int bucket_min_size,
                                              int idxlist_size   ,
                                              int nbuckets       ) {

#ifdef ERROR_CHECK
	assert(bucket_idxlist != NULL);
#endif

	for (int i = 0; i < idxlist_size; i++) {
		bucket_idxlist[i] = idxlist[i] / bucket_min_size;
		if (bucket_idxlist[i] >= nbuckets)
			bucket_idxlist[i] = nbuckets - 1;
	}
}

void assign_idxlist_elements_to_buckets2(     int *bucket_idxlist       ,
                                        const int *idxlist              ,
                                              int bucket_min_size_stride,
                                              int idxlist_size          ,
                                              int nbuckets              ,
                                              int bucket_stride         ) {

#ifdef ERROR_CHECK
	assert(bucket_idxlist != NULL);
#endif

	for (int i = 0; i < idxlist_size; i++) {
		int n_stride = idxlist[i] / bucket_stride;
		int mapped_idxlist_stride = idxlist[i] - bucket_stride * n_stride;
		bucket_idxlist[i] = mapped_idxlist_stride / bucket_min_size_stride;
		if (bucket_idxlist[i] >= nbuckets)
			bucket_idxlist[i] = nbuckets - 1;
	}

}

int num_procs_send_to_each_bucket(const int      *bucket_idxlist,
                                        int       nbuckets      ,
                                        int       idxlist_size  ,
                                        MPI_Comm  comm          ) {

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

void num_indices_to_send_to_each_bucket(      int *n_idx_each_bucket,
                                        const int *bucket_idxlist   ,
                                              int  idxlist_size     ,
                                              int  nbuckets         ) {

#ifdef ERROR_CHECK
	assert(n_idx_each_bucket != NULL);
#endif

	// Initialize to zero
	for (int rank = 0; rank < nbuckets; rank++)
		n_idx_each_bucket[rank] = 0;

	if (idxlist_size > 0) {
		for (int i = 0, offset = 0, count = 1; i < idxlist_size; i++, count++) {
			if (bucket_idxlist[i] != bucket_idxlist[offset]) {
				count = 1;
				offset = i;
			}
			n_idx_each_bucket[bucket_idxlist[offset]] = count;
		}
	}
}

void senders_to_bucket(      int      *senders_to_bucket        ,
                       const int      *n_idx_each_bucket        , 
                             int       n_procs_sending_to_bucket,
                             int       bucket_max_size          ,
                             int       idxlist_size            ,
                             MPI_Comm  comm                     ) {

#ifdef ERROR_CHECK
	assert(n_idx_each_bucket != NULL);
#endif

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_size+bucket_max_size];
	MPI_Status stat[idxlist_size+bucket_max_size];
	int nreq = 0;

	for (int i=0; i<world_size; i++)
		if (n_idx_each_bucket[i] > 0) {
			check_mpi( MPI_Isend(&world_rank, 1, MPI_INT, i, i+bucket_max_size*(i+1), comm, &req[nreq]) );
			nreq++;
		}

	for (int i=0; i<n_procs_sending_to_bucket; i++) {
		check_mpi( MPI_Irecv(&senders_to_bucket[i], 1, MPI_INT, MPI_ANY_SOURCE,
		                      world_rank+bucket_max_size*(world_rank+1), comm, &req[nreq]) );
		nreq++;
	}
	check_mpi( MPI_Waitall(nreq, req, stat) );

	// sort by process number
	mergeSort(senders_to_bucket, 0, n_procs_sending_to_bucket-1);
}

void num_indices_to_bucket_from_each_rank(      int      *bucket_msg_size_senders     ,
                                          const int      *n_idx_each_bucket        ,
                                          const int      *senders_to_bucket        ,
                                                int       n_procs_sending_to_bucket,
                                                int       bucket_max_size          ,
                                                int       idxlist_size            ,
                                                MPI_Comm  comm                     ) {

#ifdef ERROR_CHECK
	assert(n_idx_each_bucket != NULL);
	assert(senders_to_bucket != NULL);
#endif

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_size+bucket_max_size];
	MPI_Status stat[idxlist_size+bucket_max_size];
	int nreq = 0;

	for (int i=0; i<world_size; i++)
		if (n_idx_each_bucket[i] > 0) {
			check_mpi( MPI_Isend(&n_idx_each_bucket[i], 1, MPI_INT, i,
			                      world_rank+bucket_max_size*i, comm, &req[nreq]) );
			nreq++;
		}

	for (int i=0; i<n_procs_sending_to_bucket; i++) {
		check_mpi( MPI_Irecv(&bucket_msg_size_senders[i], 1, MPI_INT, senders_to_bucket[i],
		                      senders_to_bucket[i]+bucket_max_size*world_rank, comm, &req[nreq]) );
		nreq++;
	}
	check_mpi( MPI_Waitall(nreq, req, stat) );
}

void bucket_idxlist_procs(      int      *bucket_ranks             ,
                          const int      *n_idx_each_bucket        ,
                          const int      *bucket_msg_size_senders  ,
                          const int      *senders_to_bucket        ,
                                int       n_procs_sending_to_bucket,
                                int       bucket_max_size          ,
                                int       idxlist_size            ,
                                MPI_Comm  comm                     ) {

#ifdef ERROR_CHECK
	assert(n_idx_each_bucket       != NULL);
	assert(bucket_msg_size_senders != NULL);
	assert(senders_to_bucket       != NULL);
#endif

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_size+bucket_max_size];
	MPI_Status stat[idxlist_size+bucket_max_size];
	int nreq = 0;

	// recv bucket idxlist procs
	int offset = 0;
	for (int i = 0; i < n_procs_sending_to_bucket; i++) {
		check_mpi( MPI_Irecv(&bucket_ranks[offset], bucket_msg_size_senders[i], MPI_INT, senders_to_bucket[i],
		                      world_rank + bucket_max_size * (world_rank + 1), comm, &req[nreq]) );
		offset +=  bucket_msg_size_senders[i];
		nreq++;
	}

	//  MPI ranks send ID info to buckets
	for (int i = 0; i < world_size; i++) {
		if (n_idx_each_bucket[i] > 0) {
			int myrank_arr[n_idx_each_bucket[i]];
			for (int j = 0; j < n_idx_each_bucket[i]; j++)
				myrank_arr[j] = world_rank;
			check_mpi( MPI_Send(myrank_arr, n_idx_each_bucket[i], MPI_INT, i, i + bucket_max_size * (i + 1), comm) );
		}
	}

	check_mpi( MPI_Waitall(nreq, req, stat) );
}

void bucket_idxlist_elements(      int      *bucket_indices           ,
                             const int      *original_idxlist_sorted  ,
                             const int      *n_idx_each_bucket        ,
                             const int      *bucket_msg_size_senders  ,
                             const int      *senders_to_bucket        ,
                                   int       n_procs_sending_to_bucket,
                                   int       bucket_max_size          ,
                                   int       idxlist_size             ,
                                   MPI_Comm  comm                     ) {

#ifdef ERROR_CHECK
	assert(n_idx_each_bucket       != NULL);
	assert(bucket_msg_size_senders != NULL);
	assert(senders_to_bucket       != NULL);
#endif

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	MPI_Request req[idxlist_size+bucket_max_size];
	MPI_Status stat[idxlist_size+bucket_max_size];
	int nreq = 0;

	//  MPI ranks send info to src bucket
	for (int i = 0, offset = 0; i < world_size; i++) {
		if (n_idx_each_bucket[i] > 0) {
			check_mpi( MPI_Isend(&original_idxlist_sorted[offset], n_idx_each_bucket[i], MPI_INT,
			                      i, i+bucket_max_size*(i+1), comm, &req[nreq]) );
			offset += n_idx_each_bucket[i];
			nreq++;
		}
	}

	// recv src for each bucket
	int offset = 0;
	for (int i = 0; i < n_procs_sending_to_bucket; i++) {
		check_mpi( MPI_Irecv(&bucket_indices[offset], bucket_msg_size_senders[i], MPI_INT,
		                      senders_to_bucket[i], world_rank+bucket_max_size*(world_rank+1), comm, &req[nreq]) );
		offset +=  bucket_msg_size_senders[i];
		nreq++;
	}

	check_mpi( MPI_Waitall(nreq, req, stat) );
}