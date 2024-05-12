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
#include "mpi.h"

#include "src/core/algorithm/bucket.h"
#include "src/core/algorithm/backend/backend.h"
#include "src/sort/mergesort.h"
#include "src/utils/check.h"

void map_idxlist_to_RD_decomp(t_bucket  *bucket       ,
                              t_idxlist *idxlist      ,
                              int       *idxlist_local,
                              int        nbuckets     ,
                              MPI_Comm comm           ) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	// each element of the idxlist is assigned to a bucket
	int bucket_idxlist[idxlist->count];
	if (bucket->stride < 0) {
		assign_idxlist_elements_to_buckets(bucket_idxlist, idxlist->list, bucket->min_size, idxlist->count, world_size);
	} else {
		assign_idxlist_elements_to_buckets2(bucket_idxlist, idxlist->list,
		                                   bucket->min_size_stride, idxlist->count,
		                                   world_size, bucket->stride);
	}

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

	bucket->count_recv = num_procs_send_to_each_bucket(bucket_idxlist, world_size, idxlist->count, comm);

	// number of indices to be sent to each bucket
	bucket->size_ranks = (int *)malloc(nbuckets*sizeof(int));
	num_indices_to_send_to_each_bucket(bucket->size_ranks, bucket_idxlist, idxlist->count, world_size);

	// source of each message
	if (bucket->count_recv > 0)
		bucket->src_recv = (int *)malloc(bucket->count_recv*sizeof(int));
	senders_to_bucket(bucket->src_recv, bucket->size_ranks, 
                         bucket->count_recv, bucket->max_size, idxlist->count, comm);

	// size of each message that each bucket receive
	if (bucket->count_recv > 0)
		bucket->msg_size_recv = (int *)malloc(bucket->count_recv*sizeof(int));
	num_indices_to_bucket_from_each_rank(bucket->msg_size_recv, 
                                    bucket->size_ranks, bucket->src_recv,
                                    bucket->count_recv, bucket->max_size,
                                    idxlist->count, comm);

	// gather bucket info (idxlist and associated MPI procs)
	bucket_idxlist_procs(bucket->ranks,
	                     bucket->size_ranks, bucket->msg_size_recv,
	                     bucket->src_recv,
	                     bucket->count_recv, bucket->max_size,
	                     idxlist->count, comm);

	bucket_idxlist_elements(bucket->idxlist,
	                        src_idxlist_sort,
	                        bucket->size_ranks, bucket->msg_size_recv,
	                        bucket->src_recv,
	                        bucket->count_recv, bucket->max_size,
	                        idxlist->count, comm);

}

void map_RD_decomp_to_idxlist(t_bucket *src_bucket   ,
                              t_bucket *dst_bucket   ,
                              int      *idxlist_local,
                              int       idxlist_size ,
                              int       nbuckets     ,
                              MPI_Comm  comm         ) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	int dst_bucket_sort_src[src_bucket->size];
	for (int i=0; i<src_bucket->size; i++)
		for (int j=0; j<src_bucket->size; j++)
			if (dst_bucket->idxlist[j] == src_bucket->idxlist[i])
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
