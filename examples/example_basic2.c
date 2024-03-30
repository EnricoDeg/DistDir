/*
 * @file example_basic2.c
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

int main () {

	MPI_Init(NULL,NULL);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_role;
	int npoints_local = NCOLS * NROWS / (world_size / 2);
	int idxlist[npoints_local];
	t_idxlist *p_idxlist;
	t_idxlist *p_idxlist_empty;
	t_map *p_map;

	if (world_size != 4) return 1;

	// index list with global indices
	if (world_rank < 2) {
		world_role = I_SRC;
		int ncols_local = NCOLS / (world_size / 2);
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

	{
		double data[npoints_local];
		// src MPI ranks fill data array
		if (world_role == I_SRC)
			for (int i = 0; i < npoints_local; i++)
				data[i] = (i + npoints_local * world_rank) * 1.0;

		exchange_go(p_map, MPI_DOUBLE, data, data);

		printf("%d: ", world_rank);
		for (int i = 0; i < npoints_local; i++)
			printf("%f ", data[i]);
		printf("\n");
	}

	delete_idxlist(p_idxlist);
	delete_idxlist(p_idxlist_empty);
	delete_map(p_map);

	MPI_Finalize();
	return 0;
}