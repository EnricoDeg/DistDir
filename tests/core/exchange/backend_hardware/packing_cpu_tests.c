/*
 * @file packing_cpu_tests.c
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
#include <cmocka.h>
#include <stdlib.h>

#include "src/core/exchange/backend_hardware/backend_cpu.h"

/**
 * @brief Test01 of cpu packing
 * 
 * @details Fill buffer with trivial buffer index list.
 *          The packing functions just copy the data array into the buffer array.
 *          Integers, floats and doubles are tested.
 * 
 * @ingroup backend_cpu_tests
 */
static void packing_cpu_test01(void **state __attribute__((unused))) {

	const int size = 10;
	// create and fill buffer idxlist array
	int * buffer_idxlist = (int *)malloc(size*sizeof(int));
	for (int i=0; i<size; i++)
		buffer_idxlist[i] = i;

	// Integer tests
	{
		t_kernels *vtable = new_vtable_cpu(MPI_INT);
		// create and fill data array
		int *data = (int *)malloc(size*sizeof(int));
		for (int i=0; i<size; i++)
			data[i] = i;
		// create buffer array
		int *buffer = (int *)malloc(size*sizeof(int));

		vtable->pack(buffer, data, buffer_idxlist, size, 0, NULL);
		for (int i=0; i<size; i++)
			assert_true(buffer[i] == i);

		free(buffer);
		free(data);
		delete_vtable(vtable);
	}

	// Float tests
	{
		t_kernels *vtable = new_vtable_cpu(MPI_REAL);
		// create and fill data array
		float *data = (float *)malloc(size*sizeof(float));
		for (int i=0; i<size; i++)
			data[i] = (float)i;
		// create buffer array
		float *buffer = (float *)malloc(size*sizeof(float));

		vtable->pack(buffer, data, buffer_idxlist, size, 0, NULL);
		for (int i=0; i<size; i++)
			assert_true(buffer[i] == (float)i);

		free(buffer);
		free(data);
		delete_vtable(vtable);
	}

	// Double tests
	{
		t_kernels *vtable = new_vtable_cpu(MPI_DOUBLE);
		// create and fill data array
		double *data = (double *)malloc(size*sizeof(double));
		for (int i=0; i<size; i++)
			data[i] = (double)i;
		// create buffer array
		double *buffer = (double *)malloc(size*sizeof(double));

		vtable->pack(buffer, data, buffer_idxlist, size, 0, NULL);
		for (int i=0; i<size; i++)
			assert_true(buffer[i] == (double)i);

		free(buffer);
		free(data);
		delete_vtable(vtable);
	}

	free(buffer_idxlist);
}

int main(void) {

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(packing_cpu_test01),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}