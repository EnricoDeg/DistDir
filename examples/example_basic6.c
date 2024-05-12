/*
 * @file example_basic6.c
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
#define NLEVS 2

/**
 * @brief Basic example of exchange between two 3D domain decomposition each using 2 MPI processes.
 * 
 * @details The example uses a total of 4 MPI processes over a 4x4x2 global 3D domain.
 *          The example should give the same results of example_basic3 but a different method is 
 *          used to generate the mapping.
 *          Processes 0,1 have the following domain decomposition:
 * 
 *          Rank: 0
 *          Indices: 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30
 *          Rank: 1
 *          Indices: 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31
 * 
 *          Processes 2,3 have the following domain decomposition:
 * 
 *          Rank: 2
 *          Indices: 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23
 *          Rank: 3
 *          Indices: 8, 9, 10, 11, 12, 13, 14, 15, 24, 25, 26, 27, 28, 29, 30, 31
 * 
 *          Ranks 0,1 send data to ranks 2,3
 * 
 *          Exchange of integers is tested.
 * 
 * @ingroup examples
 */
int example_basic6() {

	distdir_initialize();

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_role;
	int npoints_local = NCOLS * NROWS / (world_size / 2);
	int idxlist[npoints_local*NLEVS];
	t_idxlist *p_idxlist;
	t_idxlist *p_idxlist_empty;
	t_map *p_map;

	if (world_size != 4) return 1;

	// index list with global indices
	if (world_rank < 2) {
		world_role = I_SRC;
		for (int k=0; k<NLEVS; k++)
			for (int i=0; i<npoints_local; i++)
				idxlist[i+k*npoints_local] = world_rank + i*2 + k * NROWS * NCOLS;
	} else {
		world_role = I_DST;
		int nrows_local = NROWS / (world_size / 2);
		for (int k=0; k<NLEVS; k++)
			for (int i=0; i < nrows_local; i++)
				for (int j=0; j < NCOLS; j++)
					idxlist[j+i*NCOLS+k*NCOLS*nrows_local] =  j + i * NCOLS +
					                     (world_rank - (world_size / 2)) * (NROWS - nrows_local) * NCOLS +
					                      k * NROWS * NCOLS;
	}

	p_idxlist = new_idxlist(idxlist, npoints_local*NLEVS);
	p_idxlist_empty = new_idxlist_empty();

	if (world_role == I_SRC) {
		p_map = new_map(p_idxlist, p_idxlist_empty, NCOLS*NROWS, MPI_COMM_WORLD);
	} else {
		p_map = new_map(p_idxlist_empty, p_idxlist, NCOLS*NROWS, MPI_COMM_WORLD);
	}

#ifdef CUDA
	distdir_hardware hw = GPU_NVIDIA;
#else
	distdir_hardware hw = CPU;
#endif
	// test exchange
	{
		t_exchanger *exchanger = new_exchanger(p_map, MPI_INT, hw);

		int data[npoints_local*NLEVS];
		// src MPI ranks fill data array
		if (world_role == I_SRC)
			for (int level = 0; level < NLEVS; level++)
				for (int i = 0; i < npoints_local; i++)
					data[i+level*npoints_local] = i + level*npoints_local + npoints_local * NLEVS * world_rank;

#pragma acc enter data copyin(data[0:npoints_local*NLEVS])
#pragma acc host_data use_device(data)
		exchanger_go(exchanger, data, data);
#pragma acc update host(data[0:npoints_local*NLEVS])

		printf("%d: ", world_rank);
		for (int level = 0; level < NLEVS; level++)
			for (int i = 0; i < npoints_local; i++)
				printf("%d ", data[i+level*npoints_local]);
		printf("\n");

#pragma acc exit data delete(data[0:npoints_local*NLEVS])
		delete_exchanger(exchanger);
	}

	delete_idxlist(p_idxlist);
	delete_idxlist(p_idxlist_empty);
	delete_map(p_map);

	distdir_finalize();

	return 0;
}

int main () {

	int err = example_basic6();
	if (err != 0) return err;

	return 0;
}