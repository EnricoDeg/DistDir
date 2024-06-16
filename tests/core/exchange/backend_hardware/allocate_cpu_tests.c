/*
 * @file allocate_cpu_tests.c
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
 * @brief Test01 of cpu allocator/deallocator
 * 
 * @details Simply allocate and deallocate memory with
 *          allocator and deallocator vtable functions.
 * 
 * @ingroup backend_cpu_tests
 */
static void allocate_cpu_test01(void **state __attribute__((unused))) {

	const int size = 10;
	t_kernels *vtable = new_vtable_cpu(MPI_INT);
	int *array = (int *)vtable->allocator(size*sizeof(int));
	assert_true(array != NULL);
	vtable->deallocator(array);
	delete_vtable(vtable);
}

int main(void) {

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(allocate_cpu_test01),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
