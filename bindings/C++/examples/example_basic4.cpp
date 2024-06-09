/*
 * @file example_basic4.cpp
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

	distdir::distdir::Ptr distdir( new distdir::distdir() );

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_role;
	int npoints_local = NCOLS * NROWS / (world_size / 2);

	if (world_size != 6) return 1;

	std::vector<int> list;

	// index list with global indices
	if (world_rank < 4) {
		world_role = I_SRC;
		npoints_local = 4;
		if (world_rank == 0) {
			list.push_back(0);
			list.push_back(1);
			list.push_back(4);
			list.push_back(5);
		} else if (world_rank == 1) {
			list.push_back(2);
			list.push_back(3);
			list.push_back(6);
			list.push_back(7);
		} else if (world_rank == 2) {
			list.push_back(8);
			list.push_back(9);
			list.push_back(12);
			list.push_back(13);
		} else if (world_rank == 3) {
			list.push_back(10);
			list.push_back(11);
			list.push_back(14);
			list.push_back(15);
		} else {
			return 1;
		}
	} else {
		world_role = I_DST;
		npoints_local = 8;
		if (world_rank == 4) {
			list.push_back(0);
			list.push_back(1);
			list.push_back(4);
			list.push_back(5);
			list.push_back(8);
			list.push_back(9);
			list.push_back(12);
			list.push_back(13);
		} else if (world_rank == 5) {
			list.push_back(2);
			list.push_back(3);
			list.push_back(6);
			list.push_back(7);
			list.push_back(10);
			list.push_back(11);
			list.push_back(14);
			list.push_back(15);
		} else {
			return 1;
		}
	}

	distdir::idxlist::Ptr idxlist_empty( new distdir::idxlist() );
	distdir::idxlist::Ptr idxlist( new distdir::idxlist(list) );

	distdir::map::Ptr map( new distdir::map(world_role == I_SRC ? idxlist : idxlist_empty,
	                                        world_role == I_SRC ? idxlist_empty : idxlist,
	                                        MPI_COMM_WORLD) );

	distdir::exchanger<int>::Ptr exchanger ( new distdir::exchanger<int>(map, MPI_INT) );

	std::vector<int> data;
	data.resize(npoints_local);
	// src MPI ranks fill data array
	if (world_role == I_SRC)
		for (int i = 0; i < npoints_local; i++)
			data[i] = i + npoints_local * world_rank;

	exchanger->go(data, data);

	std::cout << world_rank << ": ";
	for (int i = 0; i < npoints_local; i++)
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

	int err = example_basic4();
	if (err != 0) return err;

	return 0;
}