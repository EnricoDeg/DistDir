/*
 * @file example_multi_grid1.c
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

	distdir_initialize();

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int npoints_local;
	int *idxlist;
	int *idxlist_global;
	t_idxlist *p_idxlist;
	t_idxlist *p_idxlist_global;
	t_idxlist *p_idxlist_empty;
	t_map *p_map;

	if (world_size != 8) return 1;

	// split communicator
	int color_id = 1;
	if (world_rank >= 4) color_id = 2;
	MPI_Comm group_comm;
	new_group(&group_comm, MPI_COMM_WORLD, color_id);
	int group_rank;
	MPI_Comm_rank(group_comm, &group_rank);
	int group_size;
	MPI_Comm_size(group_comm, &group_size);

//	printf("%d: rank %d of %d\n", world_rank, group_rank, group_size);

	npoints_local = 4;
	idxlist = (int *)malloc(npoints_local*sizeof(int));

	// index list with global indices
	npoints_local = 4;
	idxlist = (int *)malloc(npoints_local*sizeof(int));
	if (group_rank == 0) {
		idxlist[0] = 0;
		idxlist[1] = 1;
		idxlist[2] = 4;
		idxlist[3] = 5;
	} else if (group_rank == 1) {
		idxlist[0] = 2;
		idxlist[1] = 3;
		idxlist[2] = 6;
		idxlist[3] = 7;
	} else if (group_rank == 2) {
		idxlist[0] = 8;
		idxlist[1] = 9;
		idxlist[2] = 12;
		idxlist[3] = 13;
	} else if (group_rank == 3) {
		idxlist[0] = 10;
		idxlist[1] = 11;
		idxlist[2] = 14;
		idxlist[3] = 15;
	} else {
		return 1;
	}

	if (group_rank == 0) {
		idxlist_global = (int *)malloc(NROWS*NCOLS*sizeof(int));
		for (int i=0; i<NROWS*NCOLS; i++)
			idxlist_global[i] = i;
	}

	p_idxlist = new_idxlist(idxlist, npoints_local);
	if (group_rank == 0)
		p_idxlist_global = new_idxlist(idxlist_global, NROWS*NCOLS);
	p_idxlist_empty = new_idxlist_empty();

	if (group_rank > 0) {
		p_map = new_map(p_idxlist, p_idxlist_empty, -1, group_comm);
	} else {
		p_map = new_map(p_idxlist, p_idxlist_global, -1, group_comm);
	}

#ifdef CUDA
	distdir_hardware hw = GPU_NVIDIA;
#else
	distdir_hardware hw = CPU;
#endif
	// test exchange
	{
		t_exchanger *exchanger = new_exchanger(p_map, MPI_INT, hw);

		int data[npoints_local];
		int *data_global = NULL;
		if (group_rank==0)
			data_global = (int *)malloc(NROWS*NCOLS*sizeof(int));
		// src MPI ranks fill data array
		for (int i = 0; i < npoints_local; i++)
			data[i] = i + npoints_local * world_rank;

#pragma acc enter data copyin(data[0:npoints_local])
		if (group_rank == 0) {
#pragma acc enter data copyin(data_global[0:NROWS*NCOLS])
		}

#pragma acc host_data use_device(data, data_global)
		exchanger_go(exchanger, data, data_global);

#pragma acc update host(data[0:npoints_local])
		if (group_rank == 0) {
#pragma acc update host(data_global[0:NROWS*NCOLS])
		}

		printf("%d: ", world_rank);
		for (int i = 0; i < npoints_local; i++)
			printf("%d ", data[i]);
		printf("\n");

		if (group_rank == 0) {
			printf("global -- %d: ", world_rank);
			for (int i = 0; i < NROWS*NCOLS; i++)
				printf("%d ", data_global[i]);
			printf("\n");
		}

#pragma acc exit data delete(data[0:npoints_local])
		if (group_rank == 0) {
#pragma acc exit data delete(data_global[0:NROWS*NCOLS])
			free(data_global);
		}

		delete_exchanger(exchanger);
	}

	if (group_rank == 0) {
		free(idxlist_global);
		delete_idxlist(p_idxlist_global);
	}
	free(idxlist);
	delete_idxlist(p_idxlist);
	delete_idxlist(p_idxlist_empty);
	delete_map(p_map);

	distdir_finalize();

	return 0;
}