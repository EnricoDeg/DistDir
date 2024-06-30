/*
 * @file map_idxlist_to_RD_decomp_tests.c
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

#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "src/distdir.h"
#include "src/core/algorithm/bucket.h"

/**
 * @brief test01 for map_idxlist_to_RD_decomp function
 * 
 * @details Map idxlists from 2 MPI processes with indices:
 *          Process 0:
 *          0, 2, 4, 6, 8, 10, 12, 14
 *          Process 1:
 *          1, 3, 5, 7, 9, 11, 13, 15
 *          To the RD decomposition with 4 processes
 * 
 * @ingroup bucket_tests
 */
static int map_idxlist_to_RD_decomp_test01(MPI_Comm comm) {

	const int num_points = 16;
	const int I_SRC = 0;
	const int I_DST = 1;
	const int NCOLS = 4;
	const int NROWS = 4;

	// Get the number of processes
	int world_size;
	MPI_Comm_size(comm, &world_size);

	// Get the rank of the process
	int world_rank;
	MPI_Comm_rank(comm, &world_rank);

	int world_role;
	int npoints_local = NCOLS * NROWS / (world_size / 2);
	int *idxlist = (int *)malloc(npoints_local*sizeof(int));
	t_idxlist *p_idxlist;
	t_idxlist *p_idxlist_empty;

	// index list with global indices
	if (world_rank < 2) {
		world_role = I_SRC;
		for (int i=0; i<npoints_local; i++)
			idxlist[i] = world_rank + i*2;
	} else {
		world_role = I_DST;
		int nrows_local = NROWS / (world_size / 2);
		for (int i=0; i < nrows_local; i++)
			for (int j=0; j < NCOLS; j++)
				idxlist[j+i*NCOLS] = j + i * NCOLS + (world_rank - (world_size / 2)) * (NROWS - nrows_local) * NCOLS;
	}

	p_idxlist = new_idxlist(idxlist, npoints_local);
	p_idxlist_empty = new_idxlist_empty();

	int bucket_size = NCOLS * NROWS / world_size;
	int bucket_min_size = bucket_size;
	int bucket_max_size = bucket_size;
	int stride = -1;
	int bucket_size_stride = 0;
	int bucket_min_size_stride = bucket_size_stride;
	int bucket_max_size_stride = bucket_size_stride;

	int nbuckets = world_size;

	t_bucket *bucket;
	bucket = (t_bucket *)malloc(sizeof(t_bucket));
	bucket->idxlist = (int *)malloc(bucket_size*sizeof(int));
	bucket->ranks = (int *)malloc(bucket_size*sizeof(int));
	bucket->size = bucket_size;
	bucket->min_size = bucket_min_size;
	bucket->max_size = bucket_max_size;
	bucket->size_stride = bucket_size_stride;
	bucket->min_size_stride = bucket_min_size_stride;
	bucket->max_size_stride = bucket_max_size_stride;
	bucket->stride = stride;
	int *idxlist_local;

	if (world_role == I_SRC) {
		idxlist_local = (int *)malloc(p_idxlist->count*sizeof(int));
		map_idxlist_to_RD_decomp(bucket, p_idxlist, idxlist_local, nbuckets, MPI_COMM_WORLD );
	} else {
		idxlist_local = (int *)malloc(p_idxlist_empty->count*sizeof(int));
		map_idxlist_to_RD_decomp(bucket, p_idxlist_empty, idxlist_local, nbuckets, MPI_COMM_WORLD );
	}

	int error = 0;

	int solution[4] = {0, 2, 1, 3};
	for (int i=0; i<bucket_size; i++)
		if (bucket->idxlist[i] != solution[i] + world_rank*bucket_size)
			error = 1;

	for (int i=0; i<bucket_size; i++)
		if (bucket->ranks[i] != i / 2)
			error = 1;

	// free buckets memory
	free(bucket->idxlist);
	free(bucket->ranks);
	if (bucket->count_recv > 0)
		free(bucket->src_recv);
	if (bucket->count_recv > 0)
		free(bucket->msg_size_recv);
	free(bucket->size_ranks);
	free(bucket->rank_exch);
	free(bucket);

	delete_idxlist(p_idxlist);
	delete_idxlist(p_idxlist_empty);
	free(idxlist_local);
	free(idxlist);

	return error;
}

/**
 * @brief test02 for map_idxlist_to_RD_decomp function
 * 
 * @details Map idxlists from 2 MPI processes with indices:
 *          Process 0:
 *          0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24
 *          Process 1:
 *          1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23
 *          To the RD decomposition with 4 processes
 * 
 * @ingroup bucket_tests
 */
static int map_idxlist_to_RD_decomp_test02(MPI_Comm comm) {

	const int num_points = 25;
	const int I_SRC = 0;
	const int I_DST = 1;
	const int NCOLS = 5;
	const int NROWS = 5;

	// Get the number of processes
	int world_size;
	MPI_Comm_size(comm, &world_size);

	// Get the rank of the process
	int world_rank;
	MPI_Comm_rank(comm, &world_rank);

	int world_role;
	int npoints_local;
	t_idxlist *p_idxlist;
	t_idxlist *p_idxlist_empty;
	int *idxlist;

	// index list with global indices
	if (world_rank < 2) {
		world_role = I_SRC;
		npoints_local = NCOLS * NROWS / (world_size / 2) + (1 - world_rank);
		idxlist = (int *)malloc(npoints_local*sizeof(int));
		for (int i=0; i<npoints_local; i++)
			idxlist[i] = world_rank + i*2;
		p_idxlist = new_idxlist(idxlist, npoints_local);
	} else {
		p_idxlist = new_idxlist_empty();
	}

	int bucket_size_min = NCOLS * NROWS / world_size;
	int bucket_size = (world_rank == world_size-1) ?
	                  bucket_size_min + ((NCOLS * NROWS) % world_size) :
	                  bucket_size_min;
	int bucket_min_size = bucket_size_min;
	int bucket_max_size = bucket_size_min + ((NCOLS * NROWS) % world_size) ;
	int stride = -1;
	int bucket_size_stride = 0;
	int bucket_min_size_stride = bucket_size_stride;
	int bucket_max_size_stride = bucket_size_stride;

	int nbuckets = world_size;

	t_bucket *bucket;
	bucket = (t_bucket *)malloc(sizeof(t_bucket));
	bucket->idxlist = (int *)malloc(bucket_size*sizeof(int));
	bucket->ranks = (int *)malloc(bucket_size*sizeof(int));
	bucket->size = bucket_size;
	bucket->min_size = bucket_min_size;
	bucket->max_size = bucket_max_size;
	bucket->size_stride = bucket_size_stride;
	bucket->min_size_stride = bucket_min_size_stride;
	bucket->max_size_stride = bucket_max_size_stride;
	bucket->stride = stride;
	int *idxlist_local;

	idxlist_local = (int *)malloc(p_idxlist->count*sizeof(int));
	map_idxlist_to_RD_decomp(bucket, p_idxlist, idxlist_local, nbuckets, MPI_COMM_WORLD );

	int error = 0;

	int mid_bucket = world_rank == 3 ?
	                 bucket_size / 2 + 1 :
	                 bucket_size / 2;
	for (int i=0; i<mid_bucket; i++)
		if (bucket->idxlist[i] != 2 * i + world_rank * bucket_min_size)
			error = 1;
	for (int i=mid_bucket; i<bucket_size; i++)
		if (bucket->idxlist[i] != 2 * (i-mid_bucket) + 1 + world_rank * bucket_min_size)
			error = 1;

	// free buckets memory
	free(bucket->idxlist);
	free(bucket->ranks);
	if (bucket->count_recv > 0)
		free(bucket->src_recv);
	if (bucket->count_recv > 0)
		free(bucket->msg_size_recv);
	free(bucket->size_ranks);
	free(bucket->rank_exch);
	free(bucket);

	delete_idxlist(p_idxlist);
	free(idxlist_local);
	if (world_rank < 2)
		free(idxlist);

	return error;
}

int main() {

	distdir_initialize();

	int error = 0;

	error += map_idxlist_to_RD_decomp_test01(MPI_COMM_WORLD);
	error += map_idxlist_to_RD_decomp_test02(MPI_COMM_WORLD);

	distdir_finalize();
	return error;
}