/*
 * @file assign_idxlist_elements_to_buckets_tests.c
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

#include "src/backend.h"

#define num_indices 5

/**
 * @brief Test01 of assign_idxlist_elements_to_buckets function
 * 
 * @details One process having indices 0, 7, 9, 25, 47 checks the assignment to buckets in a setup with 
 *          10 buckets having a minimum size of 9 (first 9 buckets have a size of 9 and the last one has a 
 *          size between 9 and 17).
 * 
 * @ingroup backend_tests
 */
static void assign_idxlist_elements_to_buckets_test01(void **state __attribute__((unused))) {

	const int bucket_min_size =  9;
	const int nbuckets        =  5;
	int idxlist[num_indices] = {0, 7, 9, 25, 47};
	int bucket_idxlist_solution[num_indices] = {0, 0, 1, 2, 4};
	int bucket_idxlist[num_indices] ;

	assign_idxlist_elements_to_buckets( bucket_idxlist ,
                                        idxlist        ,
                                        bucket_min_size,
                                        num_indices    ,
                                        nbuckets       );

	for (int i = 0; i < num_indices; ++i)
		assert_true(bucket_idxlist[i] == bucket_idxlist_solution[i]);

}

int main(void) {

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(assign_idxlist_elements_to_buckets_test01),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}