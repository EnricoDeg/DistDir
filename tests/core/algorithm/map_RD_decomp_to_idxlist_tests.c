/*
 * @file map_RD_decomp_to_idxlist_tests.c
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
 * @brief test01 for map_RD_decomp_to_idxlist function
 * 
 * @details Map RD decomposition to idxlist.
 *          2 MPI sending processes with indices:
 *          Process 0:
 *          0, 2, 3, 6, 8, 10, 12, 14
 *          Process 1:
 *          1, 3, 5, 7, 9, 11, 13, 15
 *          2 MPI receiving process with indices:
 *          Process 2:
 *          0, 1, 2, 3, 4, 5, 6, 7
 *          Process 3:
 *          8, 9, 10, 11, 12, 13, 14, 15
 * 
 * @ingroup bucket_tests
 */
static int map_RD_decomp_to_idxlist_test01(MPI_Comm comm) {

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

	t_bucket *bucket1;
	bucket1 = (t_bucket *)malloc(sizeof(t_bucket));
	bucket1->idxlist = (int *)malloc(bucket_size*sizeof(int));
	bucket1->ranks = (int *)malloc(bucket_size*sizeof(int));
	bucket1->size = bucket_size;
	bucket1->min_size = bucket_min_size;
	bucket1->max_size = bucket_max_size;
	bucket1->size_stride = bucket_size_stride;
	bucket1->min_size_stride = bucket_min_size_stride;
	bucket1->max_size_stride = bucket_max_size_stride;
	bucket1->stride = stride;
	int *idxlist_local1;

	t_bucket *bucket2;
	bucket2 = (t_bucket *)malloc(sizeof(t_bucket));
	bucket2->idxlist = (int *)malloc(bucket_size*sizeof(int));
	bucket2->ranks = (int *)malloc(bucket_size*sizeof(int));
	bucket2->size = bucket_size;
	bucket2->min_size = bucket_min_size;
	bucket2->max_size = bucket_max_size;
	bucket2->size_stride = bucket_size_stride;
	bucket2->min_size_stride = bucket_min_size_stride;
	bucket2->max_size_stride = bucket_max_size_stride;
	bucket2->stride = stride;
	int *idxlist_local2;

	if (world_role == I_SRC) {
		idxlist_local1 = (int *)malloc(p_idxlist->count*sizeof(int));
		map_idxlist_to_RD_decomp(bucket1, p_idxlist, idxlist_local1, nbuckets, comm );
		idxlist_local2 = (int *)malloc(p_idxlist_empty->count*sizeof(int));
		map_idxlist_to_RD_decomp(bucket2, p_idxlist_empty, idxlist_local2, nbuckets, comm );

		map_RD_decomp_to_idxlist(bucket1, bucket2, idxlist_local1, p_idxlist->count      , nbuckets, comm );
		map_RD_decomp_to_idxlist(bucket2, bucket1, idxlist_local2, p_idxlist_empty->count, nbuckets, comm);
	} else {
		idxlist_local1 = (int *)malloc(p_idxlist_empty->count*sizeof(int));
		map_idxlist_to_RD_decomp(bucket1, p_idxlist_empty, idxlist_local1, nbuckets, comm );
		idxlist_local2 = (int *)malloc(p_idxlist->count*sizeof(int));
		map_idxlist_to_RD_decomp(bucket2, p_idxlist, idxlist_local2, nbuckets, comm );

		map_RD_decomp_to_idxlist(bucket1, bucket2, idxlist_local1, p_idxlist_empty->count, nbuckets, comm );
		map_RD_decomp_to_idxlist(bucket2, bucket1, idxlist_local2, p_idxlist->count      , nbuckets, comm);
	}

	int error = 0;

	if (world_role == I_SRC) {
		// test the exchange ranks (to send indices to)
		for (int i=0; i<p_idxlist->count; i++)
			if (bucket1->rank_exch[i] != i / 4 + 2)
				error = 1;

		// test idxlist_local1
		for (int i=0; i<p_idxlist->count; i++)
			if (idxlist_local1[i] != i)
				error = 1;
	} else {
		// test the exchange ranks (to receive indices from)
		for (int i=0; i<p_idxlist->count; i++)
			if (bucket2->rank_exch[i] != i / 4)
				error = 1;

		// test idxlist_local2
		for (int i=0; i<p_idxlist->count; i++)
			if (idxlist_local2[i] != (2*(i - i/4 * 4) + i/4))
				error = 1;
	}

	// free buckets memory
	free(bucket1->idxlist);
	free(bucket1->ranks);
	if (bucket1->count_recv > 0)
		free(bucket1->src_recv);
	if (bucket1->count_recv > 0)
		free(bucket1->msg_size_recv);
	free(bucket1->size_ranks);
	free(bucket1->rank_exch);
	free(bucket1);

	free(bucket2->idxlist);
	free(bucket2->ranks);
	if (bucket2->count_recv > 0)
		free(bucket2->src_recv);
	if (bucket2->count_recv > 0)
		free(bucket2->msg_size_recv);
	free(bucket2->size_ranks);
	free(bucket2->rank_exch);
	free(bucket2);

	// free memory
	delete_idxlist(p_idxlist);
	delete_idxlist(p_idxlist_empty);
	free(idxlist_local1);
	free(idxlist_local2);
	free(idxlist);

	return error;
}

int main() {

	distdir_initialize();

	int error = 0;

	error += map_RD_decomp_to_idxlist_test01(MPI_COMM_WORLD);

	distdir_finalize();
	return error;
}