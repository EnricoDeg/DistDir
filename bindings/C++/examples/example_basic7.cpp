/*
 * @file example_basic7.cpp
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

#include <iostream>
#include <vector>
#include "mpi.h"
#include "bindings/C++/distdir.hpp"

#define I_SRC 0
#define I_DST 1
#define NCOLS 4
#define NROWS 4
#define NLEVS 5

/**
 * @brief Basic example of exchange between two 3D domain decomposition each using 2 MPI processes.
 * 
 * @details The example uses a total of 4 MPI processes over a 4x4x5 global 3D domain.
 *          Processes 0-3 have the following domain decomposition:
 * 
 *          Rank: 0
 *          Indices: 0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 21, 24, 25, 28, 29
 *          Rank: 1
 *          Indices: 2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23, 26, 27, 30, 31
 *          Rank: 2
 *          Indices: 32, 33, 36, 37, 40, 41, 44, 45, 48, 49, 52, 53, 56, 57, 60, 61, 64, 65, 68, 69, 72, 73, 76, 77
 *          Rank: 3
 *          Indices: 34, 35, 38, 39, 42, 43, 46, 47, 50, 51, 54, 55, 58, 59, 62, 63, 66, 67, 70, 71, 74, 75, 78, 79
 * 
 *          Processes 4-7 have the following domain decomposition:
 * 
 *          Rank: 4
 *          Indices: 0, 1, 2, 3, 4, 5, 6, 7
 *          Rank: 5
 *          Indices: 8, 9, 10, 11, 12, 13, 14, 15
 *          Rank: 6
 *          Indices: 16, 17, 18, 19, 20, 21, 22, 23, 32, 33, 34, 35, 36, 37, 38, 39, 48, 49, 50, 51, 52, 53, 54, 55, 
 *                   64, 65, 66, 67, 68, 69, 70, 71
 *          Rank: 7
 *          Indices: 24, 25, 26, 27, 28, 29, 30, 31, 40, 41, 42, 43, 44, 45, 46, 47, 56, 57, 58, 59, 60, 61, 62, 63, 
 *                   72, 73, 74, 75, 76, 77, 78 79
 * 
 *          Ranks 0-3 send data to ranks 4-7
 * 
 *          Exchange of integers is tested.
 * 
 * @ingroup examples
 */
int example_basic7() {

	distdir::distdir::Ptr distdir( new distdir::distdir() );

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_role, total_size, nlevs_local;
	int npoints_local = NCOLS * NROWS / (world_size / 4);

	if (world_size != 8) return 1;

	std::vector<int> list;
	// index list with global indices
	if (world_rank < 4) {
		world_role = I_SRC;
		int ncols_local = NCOLS / (world_size / 4);
		if (world_rank ==0) {
			nlevs_local = 2;
			total_size = npoints_local * 2;
			for (int k=0; k<2; k++)
				for (int i=0; i < NROWS; i++)
					for (int j=0; j < ncols_local; j++)
						list.push_back( j + i * NCOLS + world_rank * (NCOLS - ncols_local) +
						                                           k * NCOLS*NROWS );
		} else if (world_rank==1) {
			nlevs_local = 2;
			total_size = npoints_local * 2;
			for (int k=0; k<2; k++)
				for (int i=0; i < NROWS; i++)
					for (int j=0; j < ncols_local; j++)
						list.push_back( j + i * NCOLS + world_rank * (NCOLS - ncols_local) +
						                                           k * NCOLS*NROWS );
		} else if (world_rank==2) {
			nlevs_local = 3;
			total_size = npoints_local * 3;
			for (int k=2; k<NLEVS; k++)
				for (int i=0; i < NROWS; i++)
					for (int j=0; j < ncols_local; j++)
						list.push_back( j + i * NCOLS + (world_rank-2) * (NCOLS - ncols_local) +
						                                           k * NCOLS*NROWS );
		} else {
			nlevs_local = 3;
			total_size = npoints_local * 3;
			for (int k=2; k<NLEVS; k++)
				for (int i=0; i < NROWS; i++)
					for (int j=0; j < ncols_local; j++)
						list.push_back( j + i * NCOLS + (world_rank-2) * (NCOLS - ncols_local) +
						                                           k * NCOLS*NROWS );
		}
	} else {
		world_role = I_DST;
		int nrows_local = NROWS / (world_size / 4);
		if (world_rank==4) {
			nlevs_local = 1;
			total_size = npoints_local;
			for (int k=0; k<1; k++)
				for (int i=0; i < nrows_local; i++)
					for (int j=0; j < NCOLS; j++)
						list.push_back( j + i * NCOLS );
		} else if (world_rank==5) {
			nlevs_local = 1;
			total_size = npoints_local;
			for (int k=0; k<1; k++)
				for (int i=0; i < nrows_local; i++)
					for (int j=0; j < NCOLS; j++)
						list.push_back( j + i * NCOLS +
						                (world_rank - (world_size / 2) ) * (NROWS - nrows_local) * NCOLS );
		} else if (world_rank==6) {
			nlevs_local = 4;
			total_size = npoints_local * 4;
			for (int k=1; k<NLEVS; k++)
				for (int i=0; i < nrows_local; i++)
					for (int j=0; j < NCOLS; j++)
						list.push_back( j + i * NCOLS + k * NROWS * NCOLS );
		} else {
			nlevs_local = 4;
			total_size = npoints_local * 4;
			for (int k=1; k<NLEVS; k++)
				for (int i=0; i < nrows_local; i++)
					for (int j=0; j < NCOLS; j++)
						list.push_back( j + i * NCOLS +
						               (world_rank - 6) * (NROWS - nrows_local) * NCOLS +
						                k * NROWS * NCOLS );
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	distdir::idxlist::Ptr idxlist_empty( new distdir::idxlist() );
	distdir::idxlist::Ptr idxlist( new distdir::idxlist(list) );

	distdir::map::Ptr map( new distdir::map(world_role == I_SRC ? idxlist : idxlist_empty,
	                                        world_role == I_SRC ? idxlist_empty : idxlist,
	                                        NCOLS * NROWS                                ,
	                                        MPI_COMM_WORLD) );

	distdir::exchanger<int>::Ptr exchanger ( new distdir::exchanger<int>(map, MPI_INT) );

	std::vector<int> data;
	data.resize(total_size);
	// src MPI ranks fill data array
	if (world_role == I_SRC) {
		if (world_rank==0) {
			for (int k=0; k<2; k++)
				for (int i = 0; i < npoints_local; i++)
					data[i+k*npoints_local] = i + k * npoints_local;
		} else if (world_rank == 1) {
			for (int k=0; k<2; k++)
				for (int i = 0; i < npoints_local; i++)
					data[i+k*npoints_local] = i + k * npoints_local + npoints_local * 2 * world_rank;
		} else if (world_rank == 2) {
			for (int k=2; k<NLEVS; k++)
				for (int i = 0; i < npoints_local; i++)
					data[i+(k-2)*npoints_local] = i + (k-2) * npoints_local +
					                              NROWS * NCOLS * 2;
		} else {
			for (int k=2; k<NLEVS; k++)
				for (int i = 0; i < npoints_local; i++)
					data[i+(k-2)*npoints_local] = i + NROWS * NCOLS * 2 + (k-2) * npoints_local +
					                              npoints_local * 3 * (world_rank-2);
		}
	}

	exchanger->go(data, data);

	std::cout << world_rank << ": ";
	for (int i = 0; i < total_size; i++)
		std::cout << data[i] << " ";
	std::cout << std::endl;

	exchanger.reset();
	map.reset();
	idxlist.reset();
	idxlist_empty.reset();
	distdir.reset();

	return 0;
}

int main() {

	int err = example_basic7();
	if (err != 0) return err;

	return 0;
}
