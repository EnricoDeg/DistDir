/*
 * @file timsort.c
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

#include "timsort.h"

#define RUN 32

#define max(a,b)                 \
    ({ __typeof__ (a) _a = (a);  \
       __typeof__ (b) _b = (b);  \
         _a > _b ? _a : _b; })

#define min(a,b)                 \
    ({ __typeof__ (a) _a = (a);  \
       __typeof__ (b) _b = (b);  \
         _a < _b ? _a : _b; })

/* This function sorts array from left index to to right index which is
 * of size atmost RUN */
void insertionSort(int *arr, int left, int right)
{

	for (int i = left + 1; i <= right; i++) {
		int temp = arr[i];
		int j = i - 1;
		while (j >= left && arr[j] > temp) {
			arr[j + 1] = arr[j];
			j--;
		}
		arr[j + 1] = temp;
	}
}

void insertionSort_with_idx(int *arr, int *arr_idx, int left, int right)
{

	for (int i = left + 1; i <= right; i++) {
		int temp = arr[i];
		int temp1 = arr_idx[i];
		int j = i - 1;
		while (j >= left && arr[j] > temp) {
			arr[j + 1] = arr[j];
			arr_idx[j + 1] = arr_idx[j];
			j--;
		}
		arr[j + 1] = temp;
		arr_idx[j + 1] = temp1;
	}
}

/* Merge function merges the sorted runs */
/* TODO: this function can be deleted and the one in mergesort.c used instead */
void merge_array(int *arr, int l, int m, int r)
{

	// Original array is broken in two parts left and right array
	int len1 = m - l + 1, len2 = r - m;
	int left[len1], right[len2];
	for (int i = 0; i < len1; i++)
		left[i] = arr[l + i];
	for (int i = 0; i < len2; i++)
		right[i] = arr[m + 1 + i];

	int i = 0;
	int j = 0;
	int k = l;

	// After comparing, we merge those two array in larger sub array
	while (i < len1 && j < len2) {
		if (left[i] <= right[j]) {
			arr[k] = left[i];
			i++;
		} else {
			arr[k] = right[j];
			j++;
		}
		k++;
	}

	// Copy remaining elements of left, if any
	while (i < len1) {
		arr[k] = left[i];
		k++;
		i++;
	}

	// Copy remaining element of right, if any
	while (j < len2) {
		arr[k] = right[j];
		k++;
		j++;
	}
}

void merge_array_with_idx(int *arr, int *arr_idx, int l, int m, int r)
{

	// Original array is broken in two parts left and right array
	int len1 = m - l + 1, len2 = r - m;
	
	int left[len1], right[len2];
	int left_idx[len1], right_idx[len2];

	for (int i = 0; i < len1; i++)
		left[i] = arr[l + i];
	
	for (int i = 0; i < len2; i++)
		right[i] = arr[m + 1 + i];

	for (int i = 0; i < len1; i++)
		left_idx[i] = arr_idx[l + i];
	
	for (int i = 0; i < len2; i++)
		right_idx[i] = arr_idx[m + 1 + i];

	int i = 0;
	int j = 0;
	int k = l;

	// After comparing, we merge those two array in larger sub array
	while (i < len1 && j < len2) {
		if (left[i] <= right[j]) {
			arr[k] = left[i];
			arr_idx[k] = left_idx[i];
			i++;
		} else {
			arr[k] = right[j];
			arr_idx[k] = right_idx[j];
			j++;
		}
		k++;
	}

	// Copy remaining elements of left, if any
	while (i < len1) {
		arr[k] = left[i];
		arr_idx[k] = left_idx[i];
		k++;
		i++;
	}

	// Copy remaining element of right, if any
	while (j < len2) {
		arr[k] = right[j];
		arr_idx[k] = right_idx[j];
		k++;
		j++;
	}
}

/* Iterative Timsort function to sort the array[0...n-1] (similar to merge sort) */
void timSort(int *arr, int l, int r)
{

	int n = r - l + 1;
	// Sort individual subarrays of size RUN
	for (int i = 0; i < n; i += RUN)
		insertionSort(arr, i, min((i + RUN - 1), (n - 1)));

	// Start merging from size RUN (or 32). It will merge to form size 64, then 128, 256 and so on ...
	for (int size = RUN; size < n; size = 2 * size) {

		// pick starting point of left sub array. We are going to merge arr[left..left+size-1]
		// and arr[left+size, left+2*size-1]. After every merge, we increase left by 2*size
		for (int left = 0; left < n; left += 2 * size) {

			// Find ending point of left sub array mid+1 is starting point of right sub array
			int mid = left + size - 1;
			int right = min((left + 2 * size - 1), (n - 1));

			// merge sub array arr[left.....mid] & arr[mid+1....right]
			if (mid < right)
				merge_array(arr, left, mid, right);
		}
	}
}

void timSort_with_idx(int *arr, int *arr_idx, int l, int r) {

	int n = r - l + 1;
	// Sort individual subarrays of size RUN
	for (int i = 0; i < n; i += RUN)
		insertionSort_with_idx(arr, arr_idx, i, min((i + RUN - 1), (n - 1)));

	// Start merging from size RUN (or 32). It will merge to form size 64, then 128, 256 and so on ...
	for (int size = RUN; size < n; size = 2 * size) {

		// pick starting point of left sub array. We are going to merge arr[left..left+size-1]
		// and arr[left+size, left+2*size-1]. After every merge, we increase left by 2*size
		for (int left = 0; left < n; left += 2 * size) {

			// Find ending point of left sub array mid+1 is starting point of right sub array
			int mid = left + size - 1;
			int right = min((left + 2 * size - 1), (n - 1));

			// merge sub array arr[left.....mid] & arr[mid+1....right]
			if (mid < right)
				merge_array_with_idx(arr, arr_idx, left, mid, right);
		}
	}
}