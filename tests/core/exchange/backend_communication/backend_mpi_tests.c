/*
 * @file backend_mpi_tests.c
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

/**
 * @brief test01 for backend_mpi module
 * 
 * @details Two processes initialize the mpi_exchange object and then
 *          process 0 send a integer to process 1.
 *          Isend and Irecv are used.
 * 
 * @ingroup backend_mpi_tests
 */
static int backend_mpi_test01(MPI_Comm comm) {

	int world_rank, world_size;
	t_mpi_exchange *mpi_exchange;

	mpi_exchange = new_mpi_exchanger(MPI_INT, 1);

	MPI_Comm_rank(comm, &world_rank);
	MPI_Comm_size(comm, &world_size);

	int value = 0;
	if (world_rank == 0) {

		value = 10;
		mpi_exchange->isend(&value, 1, mpi_exchange->type, 1, 0, comm, mpi_exchange->req, 0);
	} else {

		mpi_exchange->irecv(&value, 1, mpi_exchange->type, 0, 0, comm, mpi_exchange->req, 0);
	}

	mpi_exchange->wait(1, mpi_exchange->req, mpi_exchange->stat);

	delete_mpi_exchanger(mpi_exchange);

	int error = 0;
	if (value != 10)
		error = 1;
	return error;
}

int main() {

	distdir_initialize();

	int error = 0;

	error += backend_mpi_test01(MPI_COMM_WORLD);

	distdir_finalize();

	return error;
}