/*
 * @file num_indices_to_send_to_each_bucket_tests.c
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

#include "src/core/algorithm/backend/backend.h"

#define nbuckets 4
#define idxlist_size 9

/**
 * @brief Test01 of num_indices_to_send_to_each_bucket function
 * 
 * @details One process having bucket_idxlist = 0, 0, 0, 1, 2, 2, 3, 3, 3 checks the 
 *          number of indices to send to each bucket. In this case, it is
 *          n_idx_each_bucket = 3, 1, 2, 3
 * 
 * @ingroup backend_tests
 */
static void num_indices_to_send_to_each_bucket_test01(void **state __attribute__((unused))) {

	int bucket_idxlist[idxlist_size] = {0, 0, 0, 1, 2, 2, 3, 3, 3};
	int n_idx_each_bucket[nbuckets];
	int n_idx_each_bucket_ref[nbuckets] = {3, 1, 2, 3};

	num_indices_to_send_to_each_bucket( n_idx_each_bucket,
	                                    bucket_idxlist   ,
	                                    idxlist_size     ,
	                                    nbuckets         );

	int flag = 0;
	for (int i = 0; i < nbuckets; i++)
		if (n_idx_each_bucket[i] != n_idx_each_bucket_ref[i])
			flag++;

	assert_true(flag == 0);

}

int main(void) {

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(num_indices_to_send_to_each_bucket_test01),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}