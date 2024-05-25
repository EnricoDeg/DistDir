/*
 * @file backend_cpu.c
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
#include "src/core/exchange/backend_hardware/backend_cpu.h"
#include "src/utils/check.h"

t_kernels * new_vtable_cpu(MPI_Datatype type) {

	t_kernels * table_kernels = (t_kernels *)malloc(sizeof(t_kernels));

	/* Malloc / Free functions */
	table_kernels->allocator = allocator_cpu;
	table_kernels->deallocator = deallocator_cpu;

	if (type == MPI_INT) {

		/* Packing / Unpacking functions */
		table_kernels->pack = (kernel_func_pack)pack_cpu_int;
		table_kernels->unpack = (kernel_func_pack)unpack_cpu_int;
	} else if (type == MPI_REAL) {

		/* Packing / Unpacking functions */
		table_kernels->pack = (kernel_func_pack)pack_cpu_float;
		table_kernels->unpack = (kernel_func_pack)unpack_cpu_float;
	} else if (type == MPI_DOUBLE) {

		/* Packing / Unpacking functions */
		table_kernels->pack = (kernel_func_pack)pack_cpu_double;
		table_kernels->unpack = (kernel_func_pack)unpack_cpu_double;
	}
}

void delete_vtable(t_kernels *vtable) {

	free(vtable);
}

void pack_cpu_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset, int *transform) {

	if (transform == NULL) {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			buffer[offset+i] = data[data_idx];
		}
	} else {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			int data_idx_transform = transform[data_idx];
			buffer[offset+i] = data[data_idx_transform];
		}
	}
}

void unpack_cpu_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size, int offset, int *transform) {

	if (transform == NULL) {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			data[data_idx] = buffer[offset+i];
		}
	} else {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			int data_idx_transform = transform[data_idx];
			data[data_idx_transform] = buffer[offset+i];
		}
	}
}

void pack_cpu_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset, int *transform) {

	if (transform == NULL) {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			buffer[offset+i] = data[data_idx];
		}
	} else {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			int data_idx_transform = transform[data_idx];
			buffer[offset+i] = data[data_idx_transform];
		}
	}
}

void unpack_cpu_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size, int offset, int *transform) {

	if (transform == NULL) {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			data[data_idx] = buffer[offset+i];
		}
	} else {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			int data_idx_transform = transform[data_idx];
			data[data_idx_transform] = buffer[offset+i];
		}
	}
}

void pack_cpu_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset, int *transform) {

	if (transform == NULL) {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			buffer[offset+i] = data[data_idx];
		}
	} else {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			int data_idx_transform = transform[data_idx];
			buffer[offset+i] = data[data_idx_transform];
		}
	}
}

void unpack_cpu_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size, int offset, int *transform) {

	if (transform == NULL) {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			data[data_idx] = buffer[offset+i];
		}
	} else {

		for (int i = 0; i < buffer_size; i++) {
			int data_idx = buffer_idxlist[offset+i];
			int data_idx_transform = transform[data_idx];
			data[data_idx_transform] = buffer[offset+i];
		}
	}
}

void* allocator_cpu(size_t buffer_size) {

	void *ptr = malloc(buffer_size);

	if (!ptr && (buffer_size > 0)) {
	  fprintf(stderr, "malloc failed!\n");
	  exit(EXIT_FAILURE);
	}

	return ptr;
}

void deallocator_cpu(void *buffer) {

	free(buffer);
}