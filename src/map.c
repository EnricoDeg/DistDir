/*
 * @file map.c
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

#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "mergesort.h"
#include "bucket.h"
#include "check.h"

t_map * new_map(t_idxlist *src_idxlist, t_idxlist *dst_idxlist, MPI_Comm comm) {

	int world_size;
	check_mpi( MPI_Comm_size(comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(comm, &world_rank) );

	// ==============================================
	// Initial checks and computation of buckets size
	// ==============================================

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
			check_mpi( MPI_Allreduce(&max_idx_value, &src_bucket_size, 1, MPI_INT, MPI_MAX, comm) );
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
			check_mpi( MPI_Allreduce(&max_idx_value, &dst_bucket_size, 1, MPI_INT, MPI_MAX, comm) );
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

	// ========================================================================================
	// At this point each process provide information to the buckets about its idxlist elements
	// ========================================================================================

	t_bucket *src_bucket;
	src_bucket = (t_bucket *)malloc(sizeof(t_bucket));
	src_bucket->idxlist = (int *)malloc(bucket_size*sizeof(int));
	src_bucket->ranks = (int *)malloc(bucket_size*sizeof(int));
	src_bucket->size = bucket_size;
	int src_idxlist_local[src_idxlist->count];

	map_idxlist_to_RD_decomp(src_bucket, src_idxlist, src_idxlist_local, world_size, comm);

	// -----> dst_idxlist
	t_bucket *dst_bucket;
	dst_bucket = (t_bucket *)malloc(sizeof(t_bucket));
	dst_bucket->idxlist = (int *)malloc(bucket_size*sizeof(int));
	dst_bucket->ranks = (int *)malloc(bucket_size*sizeof(int));
	dst_bucket->size = bucket_size;
	int dst_idxlist_local[dst_idxlist->count];

	map_idxlist_to_RD_decomp(dst_bucket, dst_idxlist, dst_idxlist_local, world_size, comm);

	// ==========================================================================================
	// At this point each bucket contains info about src and dst for each point within the bucket
	// ==========================================================================================

	map_RD_decomp_to_idxlist(src_bucket, dst_bucket, src_idxlist_local, src_idxlist->count, world_size, comm);

	map_RD_decomp_to_idxlist(dst_bucket, src_bucket, dst_idxlist_local, dst_idxlist->count, world_size, comm);

	// ========================================
	// Create and fill the t_map data structure
	// ========================================

	// group all info into data structure
	t_map *map;

	map = (t_map *)malloc(sizeof(t_map));
	map->comm = comm;
	map->exch_send = (t_map_exch *)malloc(sizeof(t_map_exch));
	map->exch_recv = (t_map_exch *)malloc(sizeof(t_map_exch));

	// number of procs the current rank has to send data to
	if (src_idxlist->count > 0) {
		map->exch_send->count = 1;
		for (int i = 0, offset=0; i < src_idxlist->count; i++)
			if (src_bucket->rank_exch[i] != src_bucket->rank_exch[offset]) {
				map->exch_send->count++;
				offset = i;
			}
	} else {
		map->exch_send->count = 0;
	}

	// fill info about each send message
	map->exch_send->exch = (t_map_exch_per_rank**)malloc(map->exch_send->count * sizeof(t_map_exch_per_rank*));

	if (src_idxlist->count > 0) {
		int count = 0;
		int offset = 0;
		int buffer_size = 0;
		for (int i = 0; i < src_idxlist->count; i++) {
			if (src_bucket->rank_exch[i] != src_bucket->rank_exch[offset]) {
				if (buffer_size > 0)
					map->exch_send->exch[count] = (t_map_exch_per_rank *)malloc(sizeof(t_map_exch_per_rank));
				map->exch_send->exch[count]->exch_rank = src_bucket->rank_exch[offset];
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
			map->exch_send->exch[count] = (t_map_exch_per_rank *)malloc(sizeof(t_map_exch_per_rank));
		map->exch_send->exch[count]->exch_rank = src_bucket->rank_exch[offset];
		map->exch_send->exch[count]->buffer_size = buffer_size;
		map->exch_send->exch[count]->buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
		for (int j=offset; j<src_idxlist->count; j++)
			map->exch_send->exch[count]->buffer_idxlist[j-offset] = src_idxlist_local[j];
	}

	// number of procs the current rank has to send data to
	if (dst_idxlist->count > 0) {
		map->exch_recv->count = 1;
		for (int i = 0, offset=0; i < dst_idxlist->count; i++)
			if (dst_bucket->rank_exch[i] != dst_bucket->rank_exch[offset]) {
				map->exch_recv->count++;
				offset = i;
			}
	} else {
		map->exch_recv->count = 0;
	}

	// fill info about each recv message
	map->exch_recv->exch = (t_map_exch_per_rank**)malloc(map->exch_recv->count * sizeof(t_map_exch_per_rank*));

	if (dst_idxlist->count > 0) {
		int count = 0;
		int offset = 0;
		int buffer_size = 0;
		for (int i = 0; i < dst_idxlist->count; i++) {
			if (dst_bucket->rank_exch[i] != dst_bucket->rank_exch[offset]) {
				if (buffer_size > 0)
					map->exch_recv->exch[count] = (t_map_exch_per_rank *)malloc(sizeof(t_map_exch_per_rank));
				map->exch_recv->exch[count]->exch_rank = dst_bucket->rank_exch[offset];
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
			map->exch_recv->exch[count] = (t_map_exch_per_rank *)malloc(sizeof(t_map_exch_per_rank));
		map->exch_recv->exch[count]->exch_rank = dst_bucket->rank_exch[offset];
		map->exch_recv->exch[count]->buffer_size = buffer_size;
		map->exch_recv->exch[count]->buffer_idxlist = (int *)malloc(buffer_size * sizeof(int));
		for (int j=offset; j<dst_idxlist->count; j++)
			map->exch_recv->exch[count]->buffer_idxlist[j-offset] = dst_idxlist_local[j];
	}

	// free buckets memory
	free(src_bucket->idxlist);
	free(src_bucket->ranks);
	if (src_bucket->count_recv > 0)
		free(src_bucket->src_recv);
	if (src_bucket->count_recv > 0)
		free(src_bucket->msg_size_recv);
	free(src_bucket->size_ranks);
	free(src_bucket->rank_exch);
	free(src_bucket);

	free(dst_bucket->idxlist);
	free(dst_bucket->ranks);
	if (dst_bucket->count_recv > 0)
		free(dst_bucket->src_recv);
	if (dst_bucket->count_recv > 0)
		free(dst_bucket->msg_size_recv);
	free(dst_bucket->size_ranks);
	free(dst_bucket->rank_exch);
	free(dst_bucket);

	return map;
}

t_map * extend_map_3d(t_map *map2d, int nlevels) {

	// group all info into data structure
	t_map *map;

	map = (t_map *)malloc(sizeof(t_map));
	map->comm = map2d->comm;
	map->exch_send = (t_map_exch *)malloc(sizeof(t_map_exch));
	map->exch_recv = (t_map_exch *)malloc(sizeof(t_map_exch));

	// number of procs the current rank has to send data to
	map->exch_send->count = map2d->exch_send->count;

	// fill info about each send message
	if (map->exch_send->count > 0) {
		map->exch_send->exch = (t_map_exch_per_rank**)malloc(map->exch_send->count * sizeof(t_map_exch_per_rank*));
		for (int count = 0; count < map->exch_send->count; count++) {
			int buffer_size2d = map2d->exch_send->exch[count]->buffer_size;
			if (buffer_size2d > 0)
				map->exch_send->exch[count] = (t_map_exch_per_rank *)malloc(sizeof(t_map_exch_per_rank));
			map->exch_send->exch[count]->exch_rank = map2d->exch_send->exch[count]->exch_rank;
			map->exch_send->exch[count]->buffer_size = buffer_size2d * nlevels;
			map->exch_send->exch[count]->buffer_idxlist = (int *)malloc(map->exch_send->exch[count]->buffer_size * sizeof(int));
			for (int level = 0; level < nlevels; level++)
				for (int i = 0; i < buffer_size2d; i++)
					map->exch_send->exch[count]->buffer_idxlist[i+level*buffer_size2d] = map2d->exch_send->exch[count]->buffer_idxlist[i] + 
					                                                                     level * map->exch_send->exch[count]->buffer_size;
		}
	}

	// number of procs the current rank has to recv data from
	map->exch_recv->count = map2d->exch_recv->count;

	// fill info about each recv message
	if (map->exch_recv->count > 0) {
		map->exch_recv->exch = (t_map_exch_per_rank**)malloc(map->exch_recv->count * sizeof(t_map_exch_per_rank*));
		for (int count = 0; count < map->exch_recv->count; count++) {
			int buffer_size2d = map2d->exch_recv->exch[count]->buffer_size;
			if (buffer_size2d > 0)
				map->exch_recv->exch[count] = (t_map_exch_per_rank *)malloc(sizeof(t_map_exch_per_rank));
			map->exch_recv->exch[count]->exch_rank = map2d->exch_recv->exch[count]->exch_rank;
			map->exch_recv->exch[count]->buffer_size = buffer_size2d * nlevels;
			map->exch_recv->exch[count]->buffer_idxlist = (int *)malloc(map->exch_recv->exch[count]->buffer_size * sizeof(int));
			for (int level = 0; level < nlevels; level++)
				for (int i = 0; i < buffer_size2d; i++)
					map->exch_recv->exch[count]->buffer_idxlist[i+level*buffer_size2d] = map2d->exch_recv->exch[count]->buffer_idxlist[i] + 
					                                                                     level * map->exch_recv->exch[count]->buffer_size;
		}
	}

	return map;
}

void delete_map(t_map *map) {

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
		if (map->exch_recv->exch[count]->buffer_size > 0)
			free(map->exch_recv->exch[count]->buffer_idxlist);
		free(map->exch_recv->exch[count]);
	}
	free(map->exch_recv->exch);
	free(map->exch_recv);

	free(map);
}