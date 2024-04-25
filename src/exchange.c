/*
 * @file exchange.c
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

#include "exchange.h"
#include "check.h"
#include <stdio.h>

static void pack_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size) {
	for (int i = 0; i < buffer_size; i++) {
		int data_idx = buffer_idxlist[i];
		buffer[i] = data[data_idx];
	}
}

static void unpack_int(int *buffer, int *data, int *buffer_idxlist, int buffer_size) {
	for (int i = 0; i < buffer_size; i++) {
		int data_idx = buffer_idxlist[i];
		data[data_idx] = buffer[i];
	}
}

static void pack_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size) {
	for (int i = 0; i < buffer_size; i++) {
		int data_idx = buffer_idxlist[i];
		buffer[i] = data[data_idx];
	}
}

static void unpack_float(float *buffer, float *data, int *buffer_idxlist, int buffer_size) {
	for (int i = 0; i < buffer_size; i++) {
		int data_idx = buffer_idxlist[i];
		data[data_idx] = buffer[i];
	}
}

static void pack_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size) {
	for (int i = 0; i < buffer_size; i++) {
		int data_idx = buffer_idxlist[i];
		buffer[i] = data[data_idx];
	}
}

static void unpack_double(double *buffer, double *data, int *buffer_idxlist, int buffer_size) {
	for (int i = 0; i < buffer_size; i++) {
		int data_idx = buffer_idxlist[i];
		data[data_idx] = buffer[i];
	}
}

static void select_un_pack_kernels(xt_un_pack_kernels *table_kernels, MPI_Datatype type) {

	if (type == MPI_INT) {
		table_kernels->pack = (kernel_func)pack_int;
		table_kernels->unpack = (kernel_func)unpack_int;
	} else if (type == MPI_REAL) {
		table_kernels->pack = (kernel_func)pack_float;
		table_kernels->unpack = (kernel_func)unpack_float;
	} else if (type == MPI_DOUBLE) {
		table_kernels->pack = (kernel_func)pack_double;
		table_kernels->unpack = (kernel_func)unpack_double;
	}
}

void exchange_go(t_map        *map     ,
                 MPI_Datatype  type    ,
                 void         *src_data,
                 void         *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	// group all info into data structure
	t_exchanger *exchanger;

	exchanger = (t_exchanger *)malloc(sizeof(t_exchanger));
	exchanger->exch_send = (t_exchange *)malloc(sizeof(t_exchange));
	exchanger->exch_recv = (t_exchange *)malloc(sizeof(t_exchange));

	exchanger->exch_send->count = map->exch_send->count;
	exchanger->exch_send->exch = (t_exchange_per_rank**)malloc(exchanger->exch_send->count * sizeof(t_exchange_per_rank*));

	if (exchanger->exch_send->count>0) {
		for (int count = 0; count < exchanger->exch_send->count; count++) {
			if (map->exch_send->exch[count]->buffer_size > 0)
				exchanger->exch_send->exch[count] = (t_exchange_per_rank *)malloc(sizeof(t_exchange_per_rank));
			exchanger->exch_send->exch[count]->buffer_size = map->exch_send->exch[count]->buffer_size;
		}
	}

	exchanger->exch_recv->count = map->exch_recv->count;
	exchanger->exch_recv->exch = (t_exchange_per_rank**)malloc(exchanger->exch_recv->count * sizeof(t_exchange_per_rank*));

	if (exchanger->exch_recv->count>0) {
		for (int count = 0; count < exchanger->exch_recv->count; count++) {
			if (map->exch_recv->exch[count]->buffer_size > 0)
				exchanger->exch_recv->exch[count] = (t_exchange_per_rank *)malloc(sizeof(t_exchange_per_rank));
			exchanger->exch_recv->exch[count]->buffer_size = map->exch_recv->exch[count]->buffer_size;
		}
	}

	xt_un_pack_kernels* vtable_kernels = (xt_un_pack_kernels*)malloc(sizeof(xt_un_pack_kernels));
	select_un_pack_kernels(vtable_kernels, type);
	exchanger->vtable = vtable_kernels;

	// exchange data array
	MPI_Request req[map->exch_send->count + map->exch_recv->count];
	MPI_Status stat[map->exch_send->count + map->exch_recv->count];
	int nreq = 0;

	MPI_Aint type_size;
	MPI_Aint type_lb;
	check_mpi( MPI_Type_get_extent(type, &type_lb, &type_size) );

	// send step
	for (int count = 0; count < map->exch_send->count; count++) {
		/* allocate the buffer */
		exchanger->exch_send->exch[count]->buffer = malloc(exchanger->exch_send->exch[count]->buffer_size * type_size);

		/* pack the buffer */
		exchanger->vtable->pack(exchanger->exch_send->exch[count]->buffer,
		                    src_data,
		                    map->exch_send->exch[count]->buffer_idxlist,
		                    exchanger->exch_send->exch[count]->buffer_size);

		/* send the buffer */
		check_mpi( MPI_Isend(exchanger->exch_send->exch[count]->buffer,
		                     exchanger->exch_send->exch[count]->buffer_size,
		                     type,
		                     map->exch_send->exch[count]->exch_rank,
		                     world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                     map->comm, &req[nreq]) );
		nreq++;
	}

	// recv step
	for (int count = 0; count < map->exch_recv->count; count++) {
		/* allocate the buffer */
		exchanger->exch_recv->exch[count]->buffer = malloc(exchanger->exch_recv->exch[count]->buffer_size * type_size);

		/* receive the buffer */
		check_mpi( MPI_Irecv(exchanger->exch_recv->exch[count]->buffer,
		                     exchanger->exch_recv->exch[count]->buffer_size,
		                     type,
		                     map->exch_recv->exch[count]->exch_rank,
		                     map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                     map->comm, &req[nreq]) );
		nreq++;
	}

	/* wait for all messages */
	check_mpi( MPI_Waitall(nreq, req, stat) );

	// unpack all recv buffers
	for (int count = 0; count < map->exch_recv->count; count++) {
		exchanger->vtable->unpack(exchanger->exch_recv->exch[count]->buffer,
		                      dst_data,
		                      map->exch_recv->exch[count]->buffer_idxlist,
		                      exchanger->exch_recv->exch[count]->buffer_size);
	}

	// free memory
	for (int count = 0; count < exchanger->exch_send->count; count++)
		free(exchanger->exch_send->exch[count]->buffer);

	for (int count = 0; count < exchanger->exch_recv->count; count++)
		free(exchanger->exch_recv->exch[count]->buffer);

	for (int count = 0; count < exchanger->exch_send->count; count++)
		free(exchanger->exch_send->exch[count]);
	free(exchanger->exch_send->exch);
	free(exchanger->exch_send);

	for (int count = 0; count < exchanger->exch_recv->count; count++)
		free(exchanger->exch_recv->exch[count]);
	free(exchanger->exch_recv->exch);
	free(exchanger->exch_recv);
	free(exchanger->vtable);

	free(exchanger);
}