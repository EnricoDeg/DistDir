/*
 * @file backend_cuda.h
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
#ifndef BACKEND_CUDA_H
#define BACKEND_CUDA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mpi.h"
#include "src/core/exchange/backend_hardware/backend_hw.h"

/**
 * @brief Create new t_kernels data structure
 * 
 * @param[in] type MPI datatype of the exchanged fields
 * 
 * @return pointer to t_kernels structure
 * 
 * @ingroup backend_cuda
 */
t_kernels * new_vtable_cuda(MPI_Datatype type);

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
 * @param[in]  transform      array of indices to transform the memory layout
 * 
 * @ingroup backend_cuda
 */
void pack_cuda_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset, int *transform);

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
 * @param[in]  transform      array of indices to transform the memory layout
 * 
 * @ingroup backend_cuda
 */
void pack_cuda_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset, int *transform);

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
 * @param[in]  transform      array of indices to transform the memory layout
 * 
 * @ingroup backend_cuda
 */
void pack_cuda_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset, int *transform);

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
 * @param[in]  transform      array of indices to transform the memory layout
 * 
 * @ingroup backend_cuda
 */
void unpack_cuda_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset, int *transform);

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
 * @param[in]  transform      array of indices to transform the memory layout
 * 
 * @ingroup backend_cuda
 */
void unpack_cuda_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset, int *transform);

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
 * @param[in]  transform      array of indices to transform the memory layout
 * 
 * @ingroup backend_cuda
 */
void unpack_cuda_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset, int *transform);

/**
 * @brief Allocate array.
 *  
 * @param[in]  buffer_size byte size of the array to be allocated
 *
 * @return pointer to the allocated memory
 * 
 * @ingroup backend_cuda
 */
void* allocator_cuda(size_t buffer_size);

/**
 * @brief Deallocate array.
 *  
 * @param[inout] buffer Pointer to the memory to be deallocated
 * 
 * @ingroup backend_cuda
 */
void deallocator_cuda(void *buffer);

/**
 * @brief Copy memory from host to device.
 *  
 * @param[out] buffer_cuda pointer to the destination GPU memory
 * @param[in]  buffer_cpu  pointer to the source CPU memory
 * @param[in]  buffer_size byte size to be copied
 * 
 * @ingroup backend_cuda
 */
void memcpy_h2d(int *buffer_cuda, int *buffer_cpu, int buffer_size);

#ifdef __cplusplus
}
#endif

#endif
