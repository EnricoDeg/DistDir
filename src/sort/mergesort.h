/*
 * @file mergesort.h
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

#ifndef MERGESORT_H
#define MERGESORT_H

/**
 * @brief Sort array
 * 
 * @details Sort array using mergesort algorithm
 * 
 * @param[in] arr Array to be sorted
 * @param[in] l   First index of the array to be sorted
 * @param[in] r   Last index of the array to be sorted
 * 
 * @ingroup sorting
 */
void mergeSort(int *arr, int l, int r);

/**
 * @brief Sort array and an associated index list
 * 
 * @details Sort array and an associated index list using mergesort algorithm
 * 
 * @param[in] arr     Array to be sorted
 * @param[in] arr_idx Index list array to be sorted
 * @param[in] l       First index of the array to be sorted
 * @param[in] r       Last index of the array to be sorted
 * 
 * @ingroup sorting
 */
void mergeSort_with_idx(int *arr, int *arr_idx, int l, int r);

/**
 * @brief Sort array and two associated index lists
 * 
 * @details Sort array and two associated index lists using mergesort algorithm
 * 
 * @param[in] arr      Array to be sorted
 * @param[in] arr_idx1 First Index list array to be sorted
 * @param[in] arr_idx2 Second Index list array to be sorted
 * @param[in] l        First index of the array to be sorted
 * @param[in] r        Last index of the array to be sorted
 * 
 * @ingroup sorting
 */
void mergeSort_with_idx2(int *arr, int *arr_idx1, int *arr_idx2, int l, int r);

#endif