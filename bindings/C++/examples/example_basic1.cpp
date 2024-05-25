/*
 * @file example_basic1.cpp
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

#include <vector>
#include "mpi.h"
#include "bindings/C++/distdir.hpp"

#define I_SRC 0
#define I_DST 1
#define NCOLS 4
#define NROWS 4

/**
 * @brief Basic example of exchange between two 2D domain decomposition each using 2 MPI processes.
 * 
 * @details The example uses a total of 4 MPI processes over a 4x4 global 2D domain.
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
 *          Ranks 0,1 send data to ranks 2,3
 * 
 *          Exchange of integers and doubles are tested.
 * 
 * @ingroup examples
 */
int example_basic1() {

	distdir_initialize();

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_role;
	int npoints_local = NCOLS * NROWS / (world_size / 2);

	if (world_size != 4) return 1;

	std::vector<int> list;
	// index list with global indices
	if (world_rank < 2) {
		world_role = I_SRC;
		int ncols_local = NCOLS / (world_size / 2);
		for (int i=0; i < NROWS; i++)
			for (int j=0; j < ncols_local; j++)
				list.push_back( j + i * NCOLS + world_rank * (NCOLS - ncols_local) );
	} else {
		world_role = I_DST;
		int nrows_local = NROWS / (world_size / 2);
		for (int i=0; i < nrows_local; i++)
			for (int j=0; j < NCOLS; j++)
				list.push_back( j + i * NCOLS + (world_rank - (world_size / 2)) * (NROWS - nrows_local) * NCOLS );
	}

	distdir::idxlist::Ptr idxlist_empty( new distdir::idxlist() );
	distdir::idxlist::Ptr idxlist( new distdir::idxlist(list) );

	distdir::map::Ptr map( new distdir::map(world_role == I_SRC ? idxlist : idxlist_empty,
	                                        world_role == I_SRC ? idxlist_empty : idxlist,
	                                        MPI_COMM_WORLD) );


	map.reset();
	idxlist.reset();
	idxlist_empty.reset();

	distdir_finalize();

	return 0;
}

int main() {

	int err = example_basic1();
	if (err != 0) return err;

	return 0;
}