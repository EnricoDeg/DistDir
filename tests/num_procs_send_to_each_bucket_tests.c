/*
 * @file num_procs_send_to_each_bucket_tests.c
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

#include "src/backend.h"

/**
 * @brief test01 for num_procs_send_to_each_bucket function
 * 
 * @details The test is run with 4 MPI ranks which define the following
 *          bucket_idxlist based on the following idxlist:
 *          
 *           - Rank 0: 0, 4, 8 , 12
 *           - Rank 1: 1, 5, 9 , 13
 *           - Rank 2: 2, 6, 10, 14
 *           - Rank 4: 3, 7, 11, 15
 *          
 *          Each rank owns a bucket and it is expected to receive indices
 *          information from 4 MPI ranks.
 * 
 * @ingroup backend_tests
 */
static int test01(MPI_Comm comm) {

	const int num_points = 16;

	// Get the number of processes
	int world_size;
	MPI_Comm_size(comm, &world_size);

	// Get the rank of the process
	int world_rank;
	MPI_Comm_rank(comm, &world_rank);

	int *bucket_idxlist = (int *)malloc(num_points/world_size*sizeof(int));

	// buckets where the idxlist point belong
	for (int i=0; i<num_points/world_size; i++) {
		bucket_idxlist[i] = (world_rank + world_size * i) / world_size;
	}

	// each rank (bucket) gets the number of process to receive message from
	int num_procs = num_procs_send_to_each_bucket(bucket_idxlist, world_size , num_points/world_size , MPI_COMM_WORLD );
	
	// check result
	int error = 0;
	if (num_procs != 4)
		error = 1;

	free(bucket_idxlist);

	return error;
}

int main() {

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	int error = 0;

	error += test01(MPI_COMM_WORLD);

	// Finalize the MPI environment.
	MPI_Finalize();
	return error;
}