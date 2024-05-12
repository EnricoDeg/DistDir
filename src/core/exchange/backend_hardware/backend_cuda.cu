/*
 * @file backend_cuda.cu
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

#include <stdlib.h>
#include <stdio.h>
#include <cuda.h>
#include "src/core/exchange/backend_hardware/backend_cuda.h"

__global__ void pack_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset) {

	int id = blockDim.x * blockIdx.x + threadIdx.x;
	if(id < buffer_size) {
		int data_idx = buffer_idxlist[offset+id];
		buffer[offset+id] = data[data_idx];
	}
}

__global__ void pack_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset) {

	int id = blockDim.x * blockIdx.x + threadIdx.x;
	if(id < buffer_size) {
		int data_idx = buffer_idxlist[offset+id];
		buffer[offset+id] = data[data_idx];
	}
}

__global__ void pack_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset) {

	int id = blockDim.x * blockIdx.x + threadIdx.x;
	if(id < buffer_size) {
		int data_idx = buffer_idxlist[offset+id];
		buffer[offset+id] = data[data_idx];
	}
}

__global__ void unpack_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset) {

	int id = blockDim.x * blockIdx.x + threadIdx.x;
	if(id < buffer_size) {
		int data_idx = buffer_idxlist[offset+id];
		data[data_idx] = buffer[offset+id];
	}
}

__global__ void unpack_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset) {

	int id = blockDim.x * blockIdx.x + threadIdx.x;
	if(id < buffer_size) {
		int data_idx = buffer_idxlist[offset+id];
		data[data_idx] = buffer[offset+id];
	}
}

__global__ void unpack_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset) {

	int id = blockDim.x * blockIdx.x + threadIdx.x;
	if(id < buffer_size) {
		int data_idx = buffer_idxlist[offset+id];
		data[data_idx] = buffer[offset+id];
	}
}

extern "C" t_kernels * new_vtable_cuda(MPI_Datatype type) {

	t_kernels * table_kernels = (t_kernels *)malloc(sizeof(t_kernels));

	/* Malloc / Free functions */
	table_kernels->allocator = allocator_cuda;
	table_kernels->deallocator = deallocator_cuda;

	if (type == MPI_INT) {

		/* Packing / Unpacking functions */
		table_kernels->pack = (kernel_func_pack)pack_cuda_int;
		table_kernels->unpack = (kernel_func_pack)unpack_cuda_int;
	} else if (type == MPI_REAL) {

		/* Packing / Unpacking functions */
		table_kernels->pack = (kernel_func_pack)pack_cuda_float;
		table_kernels->unpack = (kernel_func_pack)unpack_cuda_float;
	} else if (type == MPI_DOUBLE) {

		/* Packing / Unpacking functions */
		table_kernels->pack = (kernel_func_pack)pack_cuda_double;
		table_kernels->unpack = (kernel_func_pack)unpack_cuda_double;
	}
}

extern "C" void pack_cuda_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset) {

	int thr_per_blk = 256;
	int blk_in_grid = ceil( float(buffer_size) / thr_per_blk );

	pack_int<<< blk_in_grid, thr_per_blk >>>(buffer, data, buffer_idxlist, buffer_size, offset);

	cudaError_t err = cudaDeviceSynchronize();
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (pack_int): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void pack_cuda_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset) {

	int thr_per_blk = 256;
	int blk_in_grid = ceil( float(buffer_size) / thr_per_blk );

	pack_float<<< blk_in_grid, thr_per_blk >>>(buffer, data, buffer_idxlist, buffer_size, offset);

	cudaError_t err = cudaDeviceSynchronize();
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (pack_float): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void pack_cuda_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset) {

	int thr_per_blk = 256;
	int blk_in_grid = ceil( float(buffer_size) / thr_per_blk );

	pack_double<<< blk_in_grid, thr_per_blk >>>(buffer, data, buffer_idxlist, buffer_size, offset);

	cudaError_t err = cudaDeviceSynchronize();
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (pack_double): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void unpack_cuda_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset) {

	int thr_per_blk = 256;
	int blk_in_grid = ceil( float(buffer_size) / thr_per_blk );

	unpack_int<<< blk_in_grid, thr_per_blk >>>(buffer, data, buffer_idxlist, buffer_size, offset);

	cudaError_t err = cudaDeviceSynchronize();
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (unpack_int): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void unpack_cuda_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset) {

	int thr_per_blk = 256;
	int blk_in_grid = ceil( float(buffer_size) / thr_per_blk );

	unpack_float<<< blk_in_grid, thr_per_blk >>>(buffer, data, buffer_idxlist, buffer_size, offset);

	cudaError_t err = cudaDeviceSynchronize();
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (unpack_float): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void unpack_cuda_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset) {

	int thr_per_blk = 256;
	int blk_in_grid = ceil( float(buffer_size) / thr_per_blk );

	unpack_double<<< blk_in_grid, thr_per_blk >>>(buffer, data, buffer_idxlist, buffer_size, offset);

	cudaError_t err = cudaDeviceSynchronize();
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (unpack_double): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void* allocator_cuda(size_t buffer_size) {

	void *p;

	cudaError_t err = cudaMalloc(&p, buffer_size);
	if (err == cudaSuccess) {
		return p;
	} else {
		fprintf(stderr, "CUDA error (cudaMalloc): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void deallocator_cuda(void *buffer) {

	cudaError_t err = cudaFree ( buffer );
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (cudaFree): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

extern "C" void memcpy_h2d(int *buffer_cuda, int *buffer_cpu, int buffer_size) {

	cudaError_t err = cudaMemcpy ( buffer_cuda, buffer_cpu, (size_t)buffer_size*sizeof(int), cudaMemcpyHostToDevice );
	if ( err != cudaSuccess ) {
		fprintf(stderr, "CUDA error (cudaMemcpy): %s\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}