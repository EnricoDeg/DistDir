/*
 * @file example_basic5.c
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
#include "mpi.h"
#include "src/distdir.h"

#define I_SRC 0
#define I_DST 1
#define NCOLS 4
#define NROWS 4

/**
 * @brief Basic example of exchange between two 2D domain decomposition each using the same 2 MPI processes.
 * 
 * @details The example uses a total of 2 MPI processes over a 4x4 global 2D domain.
 *          The transposition is tested, assuming that the MPI processes hold two domain decompositions
 *          transposed between each others.
 *          Processes 0,1 have the following first domain decomposition:
 * 
 *          Rank: 0
 *          Indices: 0, 1, 4, 5, 8, 9, 12, 13
 *          Rank: 1
 *          Indices: 2, 3, 6, 7, 10, 11, 14, 15
 * 
 *          Processes 0,1 have the following second domain decomposition:
 * 
 *          Rank: 0
 *          Indices: 0, 1, 2, 3, 4, 5, 6, 7, 8
 *          Rank: 1
 *          Indices: 9, 10, 11, 12, 13, 14, 15
 * 
 *          Ranks 0,1 send data from the first to the second domain decomposition.
 * 
 *          Exchange of integers is tested.
 * 
 * @ingroup examples
 */
int example_basic5() {

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int npoints_local = NCOLS * NROWS / (world_size);
	int src_idxlist[npoints_local];
	int dst_idxlist[npoints_local];
	t_idxlist *p_src_idxlist;
	t_idxlist *p_dst_idxlist;
	t_map *p_map;

	if (world_size != 2) return 1;

	// index list with global indices
	int ncols_local = NCOLS / (world_size);
	for (int i=0; i < NROWS; i++)
		for (int j=0; j < ncols_local; j++)
			src_idxlist[j+i*ncols_local] = j + i * NCOLS + world_rank * (NCOLS - ncols_local);

	int nrows_local = NROWS / (world_size);
	for (int i=0; i < nrows_local; i++)
		for (int j=0; j < NCOLS; j++)
			dst_idxlist[j+i*NCOLS] = j + i * NCOLS + world_rank * (NROWS - nrows_local) * NCOLS;

	p_src_idxlist = new_idxlist(src_idxlist, npoints_local);
	p_dst_idxlist = new_idxlist(dst_idxlist, npoints_local);

	p_map = new_map(p_src_idxlist, p_dst_idxlist, -1, MPI_COMM_WORLD);

	// test exchange
	{
		t_exchanger *exchanger = new_exchanger(p_map, MPI_INT);

		int src_data[npoints_local];
		int dst_data[npoints_local];
		// MPI ranks fill src_data array
		for (int i = 0; i < npoints_local; i++)
			src_data[i] = i + npoints_local * world_rank;

		printf("%d: ", world_rank);
		for (int i = 0; i < npoints_local; i++)
			printf("%d ", src_data[i]);
		printf("\n");

		exchanger_go(exchanger, src_data, dst_data);

		printf("%d: ", world_rank);
		for (int i = 0; i < npoints_local; i++)
			printf("%d ", dst_data[i]);
		printf("\n");

		delete_exchanger(exchanger);
	}

	delete_idxlist(p_src_idxlist);
	delete_idxlist(p_dst_idxlist);
	delete_map(p_map);

	return 0;
}

int main () {

	MPI_Init(NULL,NULL);

	int err = example_basic5();
	if (err != 0) return err;

	MPI_Finalize();
	return 0;
}