/*
 * @file example_basic5.cpp
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

	distdir::distdir::Ptr distdir( new distdir::distdir() );

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_role;
	int npoints_local = NCOLS * NROWS / (world_size);

	if (world_size != 2) return 1;

	std::vector<int> src_list;
	std::vector<int> dst_list;
	// index list with global indices
	int ncols_local = NCOLS / (world_size);
	for (int i=0; i < NROWS; i++)
		for (int j=0; j < ncols_local; j++)
			src_list.push_back( j + i * NCOLS + world_rank * (NCOLS - ncols_local) );

	int nrows_local = NROWS / (world_size);
	for (int i=0; i < nrows_local; i++)
		for (int j=0; j < NCOLS; j++)
			dst_list.push_back( j + i * NCOLS + world_rank * (NROWS - nrows_local) * NCOLS );

	distdir::idxlist::Ptr src_idxlist( new distdir::idxlist(src_list) );
	distdir::idxlist::Ptr dst_idxlist( new distdir::idxlist(dst_list) );

	distdir::map::Ptr map( new distdir::map(src_idxlist, dst_idxlist, MPI_COMM_WORLD) );

	distdir::exchanger<int>::Ptr exchanger ( new distdir::exchanger<int>(map, MPI_INT) );

	std::vector<int> src_data;
	src_data.resize(npoints_local);
	std::vector<int> dst_data;
	dst_data.resize(npoints_local);
	// src MPI ranks fill data array
	for (int i = 0; i < npoints_local; i++)
		src_data[i] = i + npoints_local * world_rank;

	exchanger->go(src_data, dst_data);

	std::cout << world_rank << ": ";
	for (int i = 0; i < npoints_local; i++)
		std::cout << src_data[i] << " ";
	std::cout << std::endl;

	std::cout << world_rank << ": ";
	for (int i = 0; i < npoints_local; i++)
		std::cout << dst_data[i] << " ";
	std::cout << std::endl;

	exchanger.reset();
	map.reset();
	src_idxlist.reset();
	dst_idxlist.reset();
	distdir.reset();

	return 0;
}

int main() {

	int err = example_basic5();
	if (err != 0) return err;

	return 0;
}
