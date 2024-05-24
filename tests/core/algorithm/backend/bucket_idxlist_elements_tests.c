/*
 * @file bucket_idxlist_elements_tests.c
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

#include "src/core/algorithm/backend/backend.h"

#define SIZE 10

/**
 * @brief test01 for bucket_idxlist_elements function
 * 
 * @details 4 processes send their index list elements to bucket (rank) 0.
 *          Bucket 0 is filled and contains the elements for each bucket index.
 *          The bucket has the following elements
 *          0, 1, 2, 3, 0, 1, 2, 0, 1, 0
 * 
 * @ingroup backend_tests
 */
static int bucket_idxlist_elements_test01(MPI_Comm comm) {

	// Get the number of processes
	int world_size;
	MPI_Comm_size(comm, &world_size);

	// Get the rank of the process
	int world_rank;
	MPI_Comm_rank(comm, &world_rank);

	int bucket_max_size = 10;
	int idxlist_size = 10;
	int n_procs_sending_to_bucket = 0;
	if (world_rank == 0)
		n_procs_sending_to_bucket = world_size;

	int *n_idx_each_bucket = (int *)malloc(world_size*sizeof(int));
	int *senders_to_bucket_array;
	int *bucket_msg_size_senders;
	int *bucket_elements;
	int *idxlist_sorted = (int *)malloc((world_size-world_rank)*sizeof(int));
	int  bucket_elements_ref[SIZE] = {0, 1, 2, 3, 0, 1, 2, 0, 1, 0} ;

	for (int i=0; i<world_size-world_rank; i++)
		idxlist_sorted[i] = i;

	if (world_rank == 0) {

		senders_to_bucket_array = (int *)malloc(n_procs_sending_to_bucket*sizeof(int));
		bucket_msg_size_senders = (int *)malloc(n_procs_sending_to_bucket*sizeof(int));
		bucket_elements         = (int *)malloc(bucket_max_size*sizeof(int));
		n_procs_sending_to_bucket = world_size;

		for (int i=0; i<world_size; i++)
			senders_to_bucket_array[i] = i;

		for (int i=0; i<n_procs_sending_to_bucket; i++)
			bucket_msg_size_senders[i] = world_size - i;
	} else {

		senders_to_bucket_array = (int *)malloc(n_procs_sending_to_bucket*sizeof(int));
		bucket_msg_size_senders = (int *)malloc(n_procs_sending_to_bucket*sizeof(int));
		bucket_elements         = (int *)malloc(bucket_max_size*sizeof(int));
	}

	// buckets where the idxlist point belong
	n_idx_each_bucket[0] = world_size - world_rank;
	for (int i=1; i<world_size; i++)
		n_idx_each_bucket[i] = 0;

	// call backend function
	bucket_idxlist_elements( bucket_elements          ,
	                         idxlist_sorted           ,
	                         n_idx_each_bucket        ,
	                         bucket_msg_size_senders  ,
	                         senders_to_bucket_array  ,
	                         n_procs_sending_to_bucket,
	                         bucket_max_size          ,
	                         idxlist_size             ,
	                         comm                     );

	// check result
	int error = 0;
	if (world_rank == 0)
		for (int i=0; i<bucket_max_size; i++)
			if (bucket_elements[i] != bucket_elements_ref[i])
				error = 1;

	free(n_idx_each_bucket);
	free(senders_to_bucket_array);
	free(bucket_msg_size_senders);
	free(bucket_elements);

	return error;
}

int main() {

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	int error = 0;

	error += bucket_idxlist_elements_test01(MPI_COMM_WORLD);

	// Finalize the MPI environment.
	MPI_Finalize();
	return error;
}