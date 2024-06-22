/*
 * @file sorting_tests.c
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

#include "src/sort/quicksort.h"
#include "src/sort/mergesort.h"
#include "src/sort/timsort.h"

static void shuffle(int *array, size_t n)
{
	if (n > 1) {
		size_t i;
		for (i = 0; i < n - 1; i++) {
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

/**
 * @brief Test01 of sorting functions
 * 
 * @details Create an ordered array and shuffle it.
 *          Then use mergesort, timsort and quicksort to rebuild the original sorted array.
 * 
 * @ingroup sorting_tests
 */
static void sorting_test01(void **state __attribute__((unused))) {

	const int size = 20;

	// Create ordered array
	int *array = (int *)malloc(size*sizeof(int));

	for (int i=0; i<size; i++)
		array[i] = i;

	// Shuffle array
	shuffle(array, size);

	int *solution =  (int *)malloc(size*sizeof(int));

	// test mergesort
	for (int i=0; i<size; i++)
		solution[i] = array[i];

	mergeSort(solution, 0, size-1);

	for (int i=0; i<size; i++)
		assert_int_equal(solution[i], i);

	// test timsort
	for (int i=0; i<size; i++)
		solution[i] = array[i];

	timSort(solution, 0, size-1);

	for (int i=0; i<size; i++)
		assert_int_equal(solution[i], i);

	// test quicksort
	for (int i=0; i<size; i++)
		solution[i] = array[i];

	quickSort(solution, 0, size-1);

	for (int i=0; i<size; i++)
		assert_int_equal(solution[i], i);

	free(solution);
	free(array);
}

/**
 * @brief Test02 of sorting functions
 * 
 * @details Create an unordered array.
 *          Then use mergesort, timsort and quicksort to sort the array and 
 *          keep track of the indices.
 * 
 * @ingroup sorting_tests
 */
static void sorting_test02(void **state __attribute__((unused))) {

	int array[10] = {20, 19, 4, 7, 99, 3, 5, 6, 21, 0};
	int array_order[10] = {0, 3, 4, 5, 6, 7, 19, 20, 21, 99};
	int indices_order[10] = {9, 5, 2, 6, 7, 3, 1, 0, 8, 4};
	int indices[10];

	int *solution =  (int *)malloc(10*sizeof(int));

	// test mergesort
	for (int i=0; i<10; i++)
		solution[i] = array[i];

	for (int i=0; i<10; i++)
		indices[i] = i;

	mergeSort_with_idx(solution, indices, 0, 9);

	for (int i=0; i<10; i++)
		assert_int_equal(solution[i], array_order[i]);
	
	for (int i=0; i<10; i++)
		assert_int_equal(indices[i], indices_order[i]);

	// test timsort
	for (int i=0; i<10; i++)
		solution[i] = array[i];

	for (int i=0; i<10; i++)
		indices[i] = i;

	timSort_with_idx(solution, indices, 0, 9);

	for (int i=0; i<10; i++)
		assert_int_equal(solution[i], array_order[i]);
	
	for (int i=0; i<10; i++)
		assert_int_equal(indices[i], indices_order[i]);

	// test quicksort
	for (int i=0; i<10; i++)
		solution[i] = array[i];

	for (int i=0; i<10; i++)
		indices[i] = i;

	quickSort_with_idx(solution, indices, 0, 9);

	for (int i=0; i<10; i++)
		assert_int_equal(solution[i], array_order[i]);
	
	for (int i=0; i<10; i++)
		assert_int_equal(indices[i], indices_order[i]);

	free(solution);
}

/**
 * @brief Test03 of sorting functions
 * 
 * @details Create an unordered array.
 *          Then use mergesort, timsort and quicksort to sort the array and 
 *          keep track of two associated index lists.
 * 
 * @ingroup sorting_tests
 */
static void sorting_test03(void **state __attribute__((unused))) {

	int array[10] = {20, 19, 4, 7, 99, 3, 5, 6, 21, 0};
	int array_order[10] = {0, 3, 4, 5, 6, 7, 19, 20, 21, 99};
	int indices_order[10] = {9, 5, 2, 6, 7, 3, 1, 0, 8, 4};
	int indices1[10];
	int indices2[10];

	int *solution =  (int *)malloc(10*sizeof(int));

	// test mergesort
	for (int i=0; i<10; i++)
		solution[i] = array[i];

	for (int i=0; i<10; i++)
		indices1[i] = i;

	for (int i=0; i<10; i++)
		indices2[i] = i;

	mergeSort_with_idx2(solution, indices1, indices2, 0, 9);

	for (int i=0; i<10; i++)
		assert_int_equal(solution[i], array_order[i]);

	for (int i=0; i<10; i++)
		assert_int_equal(indices1[i], indices_order[i]);

	for (int i=0; i<10; i++)
		assert_int_equal(indices2[i], indices_order[i]);

	// test timsort
	for (int i=0; i<10; i++)
		solution[i] = array[i];

	for (int i=0; i<10; i++)
		indices1[i] = i;

	for (int i=0; i<10; i++)
		indices2[i] = i;

	timSort_with_idx2(solution, indices1, indices2, 0, 9);

	for (int i=0; i<10; i++)
		assert_int_equal(solution[i], array_order[i]);

	for (int i=0; i<10; i++)
		assert_int_equal(indices1[i], indices_order[i]);

	for (int i=0; i<10; i++)
		assert_int_equal(indices2[i], indices_order[i]);

	// test quicksort
	for (int i=0; i<10; i++)
		solution[i] = array[i];

	for (int i=0; i<10; i++)
		indices1[i] = i;

	for (int i=0; i<10; i++)
		indices2[i] = i;

	quickSort_with_idx2(solution, indices1, indices2, 0, 9);

	for (int i=0; i<10; i++)
		assert_int_equal(solution[i], array_order[i]);

	for (int i=0; i<10; i++)
		assert_int_equal(indices1[i], indices_order[i]);

	for (int i=0; i<10; i++)
		assert_int_equal(indices2[i], indices_order[i]);

	free(solution);
}

int main(void) {

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(sorting_test01),
		cmocka_unit_test(sorting_test02),
		cmocka_unit_test(sorting_test03),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
