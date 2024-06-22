/*
 * @file exchange_tests.c
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

#define LSIZE 8

/**
 * @brief test01 for exchange module
 * 
 * @details The test uses a total of 4 MPI processes over a 4x4 global 2D domain.
 *          Processes 0,1 have the following domain decomposition:
 * 
 *          Rank: 0
 *          Indices: 0, 1, 4, 5, 8, 9, 12, 13
 *          Rank: 1
 *          Indices: 2, 3, 6, 7, 10, 11, 14, 15
 * 
 *          Processes 2,3 have the following domain decomposition:
 * 
 *          Rank: 2
 *          Indices: 0, 1, 2, 3, 4, 5, 6, 7, 8
 *          Rank: 3
 *          Indices: 9, 10, 11, 12, 13, 14, 15
 * 
 *          Ranks 0,1 are sender processes and ranks 2,3 are receivers processes.
 * 
 *          All the exchanger types are tested with int type.
 * 
 *          The test is based on example_basic1.
 * 
 * @ingroup exchange_tests
 */
static int exchange_test01(MPI_Comm comm) {

	distdir_initialize();

	const int I_SRC = 0;
	const int I_DST = 1;
	const int NCOLS = 4;
	const int NROWS = 4;

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
	int error = 0;

	if (world_size != 4) error = 1;

	// index list with global indices
	if (world_rank < 2) {
		world_role = I_SRC;
		int ncols_local = NCOLS / (world_size / 2);
		for (int i=0; i < NROWS; i++)
			for (int j=0; j < ncols_local; j++)
				idxlist[j+i*ncols_local] = j + i * NCOLS + world_rank * (NCOLS - ncols_local);
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
		p_map = new_map(p_idxlist, p_idxlist_empty, -1, MPI_COMM_WORLD);
	} else {
		p_map = new_map(p_idxlist_empty, p_idxlist, -1, MPI_COMM_WORLD);
	}

	// solutions for processes 2 and 3
	int solution2[LSIZE] = {0, 1, 8, 9, 2, 3, 10, 11};
	int solution3[LSIZE] = {4, 5, 12, 13, 6, 7, 14, 15};

#ifdef CUDA
	distdir_hardware hw = GPU_NVIDIA;
#else
	distdir_hardware hw = CPU;
#endif
	// test exchange with all exchanger types
	for (int exchanger_type = 0; exchanger_type < 8; exchanger_type++) {

		set_config_exchanger(exchanger_type);
		if (get_config_exchanger() != exchanger_type)
			error = 1;
		t_exchanger *exchanger = new_exchanger(p_map, MPI_INT, hw);
		int data[npoints_local];
		// src MPI ranks fill data array
		if (world_role == I_SRC)
			for (int i = 0; i < npoints_local; i++)
				data[i] = i + npoints_local * world_rank;

#pragma acc enter data copyin(data[0:npoints_local])
#pragma acc host_data use_device(data)
		exchanger_go(exchanger, data, data);
#pragma acc update host(data[0:npoints_local])

#pragma acc exit data delete(data[0:npoints_local])
		delete_exchanger(exchanger);

		if (world_rank == 2) {
			for (int i = 0; i < npoints_local; i++)
				if (data[i] != solution2[i])
					error = 1;
		}

		if (world_rank == 3) {
			for (int i = 0; i < npoints_local; i++)
				if (data[i] != solution3[i])
					error = 1;
		}
	}

	delete_idxlist(p_idxlist);
	delete_idxlist(p_idxlist_empty);
	delete_map(p_map);

	// synch error among processes
	MPI_Allreduce(&error, &error, 1, MPI_INT, MPI_SUM, comm);

	distdir_finalize();

	return error;
}

int main() {

	int error = 0;

	error += exchange_test01(MPI_COMM_WORLD);

	return error;
}