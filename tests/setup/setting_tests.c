/*
 * @file setting_tests.c
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
 * @brief test01 for setting MPI
 * 
 * @details 
 * 
 * @ingroup setting_tests
 */
static int setting_test01(MPI_Comm comm) {

	// test MPI initialization
	distdir_initialize();
	int error = 0;
	int mpi_initialized;
	check_mpi( MPI_Initialized( &mpi_initialized ) );
	if (!mpi_initialized)
		error = 1;
	
	// test exchanger type configuration
	int exchanger_type = get_config_exchanger();
	if (exchanger_type != IsendIrecv1)
		error = 1;

	set_config_exchanger(IsendIrecv1NoWait);
	exchanger_type = get_config_exchanger();
	if (exchanger_type != IsendIrecv1NoWait)
		error = 1;

	// test verbose type configuration
	int verbose_type = get_config_verbose();
	if (verbose_type != verbose_false)
		error = 1;
	
	set_config_verbose(verbose_true);
	verbose_type = get_config_verbose();
	if (verbose_type != verbose_true)
		error = 1;

	// check library finalization
	distdir_finalize();
	int mpi_finalized;
	check_mpi( MPI_Finalized( &mpi_finalized ) );
	if (!mpi_finalized)
		error = 1;

	return error;
}

int main() {

	int error = 0;
	error += setting_test01(MPI_COMM_WORLD);

	return error;
}
