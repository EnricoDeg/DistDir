/*
 * @file backend_MPI_tests.c
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

static void test_num_procs_send_to_each_bucket(void **state __attribute__((unused))) {

	int err = system("mpirun --allow-run-as-root -n 4 ./num_procs_send_to_each_bucket_tests");
	assert_int_equal(err, 0);
}

static void test_senders_to_bucket(void **state __attribute__((unused))) {

	int err = system("mpirun --allow-run-as-root -n 4 ./senders_to_bucket_tests");
	assert_int_equal(err, 0);
}

static void test_num_indices_to_bucket_from_each_rank(void **state __attribute__((unused))) {

	int err = system("mpirun --allow-run-as-root -n 4 ./num_indices_to_bucket_from_each_rank_tests");
	assert_int_equal(err, 0);
}

static void test_bucket_idxlist_procs(void **state __attribute__((unused))) {

	int err = system("mpirun --allow-run-as-root -n 4 ./bucket_idxlist_procs_tests");
	assert_int_equal(err, 0);
}

static void test_bucket_idxlist_elements(void **state __attribute__((unused))) {

	int err = system("mpirun --allow-run-as-root -n 4 ./bucket_idxlist_elements_tests");
	assert_int_equal(err, 0);
}

int main() {

	const struct CMUnitTest tests[] =
	{
		cmocka_unit_test(test_num_procs_send_to_each_bucket),
		cmocka_unit_test(test_senders_to_bucket),
		cmocka_unit_test(test_num_indices_to_bucket_from_each_rank),
		cmocka_unit_test(test_bucket_idxlist_procs),
		cmocka_unit_test(test_bucket_idxlist_elements),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
