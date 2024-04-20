/*
 * @file example_basic1.c
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
 * @brief Basic example of exchange between two 2D domain decomposition.
 * 
 * @details The example uses a total of 6 MPI processes over a 4x4 global 3D domain.
 *          Processes 0-3 have the following domain decomposition:
 * 
 *          Rank: 0
 *          Indices: 0, 1, 4, 5
 *          Rank: 1
 *          Indices: 2, 3, 6, 7
 *          Rank: 2
 *          Indices: 8, 9, 12, 13
 *          Rank: 3
 *          Indices: 10, 11, 14, 15
 * 
 *          Processes 4,5 have the following domain decomposition:
 * 
 *          Rank: 4
 *          Indices: 0, 1, 4, 5, 8, 9, 12, 13
 *          Rank: 5
 *          Indices: 2, 3, 6, 7, 10, 11, 14, 15
 * 
 *          Ranks 0-3 send data to ranks 4,5
 * 
 *          Exchange of integers is tested.
 * 
 * @ingroup examples
 */
int example_basic4() {

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_role;
	int npoints_local;
	int *idxlist;
	t_idxlist *p_idxlist;
	t_idxlist *p_idxlist_empty;
	t_map *p_map;

	if (world_size != 6) return 1;

	// index list with global indices
	if (world_rank < 4) {
		world_role = I_SRC;
		npoints_local = 4;
		idxlist = (int *)malloc(npoints_local*sizeof(int));
		if (world_rank == 0) {
			idxlist[0] = 0;
			idxlist[1] = 1;
			idxlist[2] = 4;
			idxlist[3] = 5;
		} else if (world_rank == 1) {
			idxlist[0] = 2;
			idxlist[1] = 3;
			idxlist[2] = 6;
			idxlist[3] = 7;
		} else if (world_rank == 2) {
			idxlist[0] = 8;
			idxlist[1] = 9;
			idxlist[2] = 12;
			idxlist[3] = 13;
		} else if (world_rank == 3) {
			idxlist[0] = 10;
			idxlist[1] = 11;
			idxlist[2] = 14;
			idxlist[3] = 15;
		} else {
			return 1;
		}
	} else {
		world_role = I_DST;
		npoints_local = 8;
		idxlist = (int *)malloc(npoints_local*sizeof(int));
		if (world_rank == 4) {
			idxlist[0] = 0;
			idxlist[1] = 1;
			idxlist[2] = 4;
			idxlist[3] = 5;
			idxlist[4] = 8;
			idxlist[5] = 9;
			idxlist[6] = 12;
			idxlist[7] = 13;
		} else if (world_rank == 5) {
			idxlist[0] = 2;
			idxlist[1] = 3;
			idxlist[2] = 6;
			idxlist[3] = 7;
			idxlist[4] = 10;
			idxlist[5] = 11;
			idxlist[6] = 14;
			idxlist[7] = 15;
		} else {
			return 1;
		}
	}

	p_idxlist = new_idxlist(idxlist, npoints_local);
	p_idxlist_empty = new_idxlist_empty();

	if (world_role == I_SRC) {
		p_map = new_map(p_idxlist, p_idxlist_empty, MPI_COMM_WORLD);
	} else {
		p_map = new_map(p_idxlist_empty, p_idxlist, MPI_COMM_WORLD);
	}

	// test exchange
	{
		int data[npoints_local];
		// src MPI ranks fill data array
		if (world_role == I_SRC)
			for (int i = 0; i < npoints_local; i++)
				data[i] = i + npoints_local * world_rank;

		exchange_go(p_map, MPI_INT, data, data);

		printf("%d: ", world_rank);
		for (int i = 0; i < npoints_local; i++)
			printf("%d ", data[i]);
		printf("\n");
	}

	free(idxlist);
	delete_idxlist(p_idxlist);
	delete_idxlist(p_idxlist_empty);
	delete_map(p_map);

	return 0;
}

int main () {

	MPI_Init(NULL,NULL);

	int err = example_basic4();
	if (err != 0) return err;

	MPI_Finalize();
	return 0;
}