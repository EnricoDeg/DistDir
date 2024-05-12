/*
 * @file backend_cpu.h
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

#ifndef BACKEND_CPU_H
#define BACKEND_CPU_H

#include <stdlib.h>

/**
 * @brief Packing function for int arrays.
 * 
 * @details Pack array data into a buffer using the buffer_idxlist
 *  
 * @param[out] buffer         integer array to be filled
 * @param[in]  data           integer array with the data
 * @param[in]  buffer_idxlist integer array with the information to fill the buffer
 * @param[in]  buffer_size    size of the buffer to be filled (it can be a subset of the buffer array)
 * @param[in]  offset         buffer offset to start the filling of the buffer array
 * 
 * @ingroup backend_cpu
 */
void pack_cpu_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset);

/**
 * @brief Unpacking function for int arrays.
 * 
 * @details Unpack beffer into array data using the buffer_idxlist
 *  
 * @param[in]  buffer         integer array with the data
 * @param[out] data           integer array to be filled
 * @param[in]  buffer_idxlist integer array with the information to fill the data array
 * @param[in]  buffer_size    size of the buffer to unpack (it can be a subset of the buffer array)
 * @param[in]  offset         buffer offset to start the unpacking of the buffer array
 * 
 * @ingroup backend_cpu
 */
void unpack_cpu_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset);

/**
 * @brief Packing function for float arrays.
 * 
 * @details Pack array data into a buffer using the buffer_idxlist
 *  
 * @param[out] buffer         float array to be filled
 * @param[in]  data           float array with the data
 * @param[in]  buffer_idxlist integer array with the information to fill the buffer
 * @param[in]  buffer_size    size of the buffer to be filled (it can be a subset of the buffer array)
 * @param[in]  offset         buffer offset to start the filling of the buffer array
 * 
 * @ingroup backend_cpu
 */
void pack_cpu_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset);

/**
 * @brief Unpacking function for float arrays.
 * 
 * @details Unpack beffer into array data using the buffer_idxlist
 *  
 * @param[in]  buffer         float array with the data
 * @param[out] data           float array to be filled
 * @param[in]  buffer_idxlist integer array with the information to fill the data array
 * @param[in]  buffer_size    size of the buffer to unpack (it can be a subset of the buffer array)
 * @param[in]  offset         buffer offset to start the unpacking of the buffer array
 * 
 * @ingroup backend_cpu
 */
void unpack_cpu_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset);

/**
 * @brief Packing function for double arrays.
 * 
 * @details Pack array data into a buffer using the buffer_idxlist
 *  
 * @param[out] buffer         double array to be filled
 * @param[in]  data           double array with the data
 * @param[in]  buffer_idxlist integer array with the information to fill the buffer
 * @param[in]  buffer_size    size of the buffer to be filled (it can be a subset of the buffer array)
 * @param[in]  offset         buffer offset to start the filling of the buffer array
 * 
 * @ingroup backend_cpu
 */
void pack_cpu_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset);

/**
 * @brief Unpacking function for double arrays.
 * 
 * @details Unpack beffer into array data using the buffer_idxlist
 *  
 * @param[in]  buffer         double array with the data
 * @param[out] data           double array to be filled
 * @param[in]  buffer_idxlist integer array with the information to fill the data array
 * @param[in]  buffer_size    size of the buffer to unpack (it can be a subset of the buffer array)
 * @param[in]  offset         buffer offset to start the unpacking of the buffer array
 * 
 * @ingroup backend_cpu
 */
void unpack_cpu_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset);

/**
 * @brief Allocate array.
 *  
 * @param[in]  buffer_size byte size of the array to be allocated
 *
 * @return pointer to the allocated memory
 * 
 * @ingroup backend_cpu
 */
void* allocator_cpu(size_t buffer_size);

/**
 * @brief Deallocate array.
 *  
 * @param[inout] buffer Pointer to the memory to be deallocated
 * 
 * @ingroup backend_cpu
 */
void deallocator_cpu(void *buffer);

#endif