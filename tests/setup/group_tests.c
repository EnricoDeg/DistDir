/*
 * @file group_tests.c
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
#include "src/utils/check.h"

/**
 * @brief test01 for group module
 * 
 * @details Each MPI process creates its own group
 * 
 * @ingroup group_tests
 */
static int group_test01(MPI_Comm comm) {

	int error = 0;

	int world_rank;
	MPI_Comm_rank(comm, &world_rank);
	int world_size;
	MPI_Comm_size(comm, &world_size);

	int id = world_rank;
	MPI_Comm group_comm;
	new_group(&group_comm, comm, id);
	int group_rank;
	MPI_Comm_rank(group_comm, &group_rank);
	int group_size;
	MPI_Comm_size(group_comm, &group_size);

	if (group_rank != 0)
		error = 1;
	if (group_size != 1)
		error = 1;

	return error;
}

int main() {

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	int error = 0;
	error += group_test01(MPI_COMM_WORLD);

	// Finalize the MPI environment.
	MPI_Finalize();
	return error;
}
