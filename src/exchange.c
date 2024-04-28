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
#include "setting.h"
#include "backend_cpu.h"
#include <stdio.h>

static void select_un_pack_kernels(t_kernels *table_kernels, MPI_Datatype type) {

	if (type == MPI_INT) {
		table_kernels->pack = (kernel_func_pack)pack_cpu_int;
		table_kernels->unpack = (kernel_func_pack)unpack_cpu_int;
	} else if (type == MPI_REAL) {
		table_kernels->pack = (kernel_func_pack)pack_cpu_float;
		table_kernels->unpack = (kernel_func_pack)unpack_cpu_float;
	} else if (type == MPI_DOUBLE) {
		table_kernels->pack = (kernel_func_pack)pack_cpu_double;
		table_kernels->unpack = (kernel_func_pack)unpack_cpu_double;
	}
}

static void exchanger_waitall(t_mpi_exchange* mpi_exchange) {

	if (mpi_exchange->nreq > 0)
		check_mpi( MPI_Waitall(mpi_exchange->nreq, mpi_exchange->req, mpi_exchange->stat) );
}

static void exchanger_IsendIrecv(t_exchange *exch_send, t_exchange *exch_recv,
                                 t_map *map, t_kernels *vtable, t_mpi_exchange* mpi_exchange,
                                 void *src_data, void *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	mpi_exchange->nreq = 0;

	for (int count = 0; count < map->exch_send->count; count++) {

		/* pack the buffer */
		vtable->pack(exch_send->exch[count]->buffer,
		                        src_data,
		                        map->exch_send->exch[count]->buffer_idxlist,
		                        exch_send->exch[count]->buffer_size);

		/* send the buffer */
		check_mpi( MPI_Isend(exch_send->exch[count]->buffer,
		                     exch_send->exch[count]->buffer_size,
		                     mpi_exchange->type,
		                     map->exch_send->exch[count]->exch_rank,
		                     world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                     map->comm, mpi_exchange->req + mpi_exchange->nreq) );
		mpi_exchange->nreq++;
	}

	// recv step
	for (int count = 0; count < map->exch_recv->count; count++) {

		check_mpi( MPI_Irecv(exch_recv->exch[count]->buffer,
		                     exch_recv->exch[count]->buffer_size,
		                     mpi_exchange->type,
		                     map->exch_recv->exch[count]->exch_rank,
		                     map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                     map->comm, mpi_exchange->req + mpi_exchange->nreq) );
		mpi_exchange->nreq++;
	}

	/* wait for all messages */
	exchanger_waitall(mpi_exchange);

	/* unpack all recv buffers */
	for (int count = 0; count < map->exch_recv->count; count++) {
		vtable->unpack(exch_recv->exch[count]->buffer,
		                      dst_data,
		                      map->exch_recv->exch[count]->buffer_idxlist,
		                      exch_recv->exch[count]->buffer_size);
	}
}

static void exchanger_IsendRecv1(t_exchange *exch_send, t_exchange *exch_recv,
                                 t_map *map, t_kernels *vtable, t_mpi_exchange* mpi_exchange,
                                 void *src_data, void *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	mpi_exchange->nreq = 0;

	for (int count = 0; count < map->exch_send->count; count++) {

		/* pack the buffer */
		vtable->pack(exch_send->exch[count]->buffer,
		                        src_data,
		                        map->exch_send->exch[count]->buffer_idxlist,
		                        exch_send->exch[count]->buffer_size);

		/* send the buffer */
		check_mpi( MPI_Isend(exch_send->exch[count]->buffer,
		                     exch_send->exch[count]->buffer_size,
		                     mpi_exchange->type,
		                     map->exch_send->exch[count]->exch_rank,
		                     world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                     map->comm, mpi_exchange->req + mpi_exchange->nreq) );
		mpi_exchange->nreq++;
	}

	/* wait for all messages */
	exchanger_waitall(mpi_exchange);

	// recv step
	for (int count = 0; count < map->exch_recv->count; count++) {

		check_mpi( MPI_Recv(exch_recv->exch[count]->buffer,
		                     exch_recv->exch[count]->buffer_size,
		                     mpi_exchange->type,
		                     map->exch_recv->exch[count]->exch_rank,
		                     map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                     map->comm, mpi_exchange->stat) );
	}

	/* unpack all recv buffers */
	for (int count = 0; count < map->exch_recv->count; count++) {
		vtable->unpack(exch_recv->exch[count]->buffer,
		                      dst_data,
		                      map->exch_recv->exch[count]->buffer_idxlist,
		                      exch_recv->exch[count]->buffer_size);
	}
}

static void exchanger_IsendRecv2(t_exchange *exch_send, t_exchange *exch_recv,
                                 t_map *map, t_kernels *vtable, t_mpi_exchange* mpi_exchange,
                                 void *src_data, void *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	mpi_exchange->nreq = 0;

	for (int count = 0; count < map->exch_send->count; count++) {

		/* pack the buffer */
		vtable->pack(exch_send->exch[count]->buffer,
		                        src_data,
		                        map->exch_send->exch[count]->buffer_idxlist,
		                        exch_send->exch[count]->buffer_size);

		/* send the buffer */
		check_mpi( MPI_Isend(exch_send->exch[count]->buffer,
		                     exch_send->exch[count]->buffer_size,
		                     mpi_exchange->type,
		                     map->exch_send->exch[count]->exch_rank,
		                     world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                     map->comm, mpi_exchange->req + mpi_exchange->nreq) );
		mpi_exchange->nreq++;
	}

	/* wait for all messages */
	exchanger_waitall(mpi_exchange);

	// recv and unpack step
	for (int count = 0; count < map->exch_recv->count; count++) {

		check_mpi( MPI_Recv(exch_recv->exch[count]->buffer,
		                     exch_recv->exch[count]->buffer_size,
		                     mpi_exchange->type,
		                     map->exch_recv->exch[count]->exch_rank,
		                     map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                     map->comm, mpi_exchange->stat) );

		vtable->unpack(exch_recv->exch[count]->buffer,
		                      dst_data,
		                      map->exch_recv->exch[count]->buffer_idxlist,
		                      exch_recv->exch[count]->buffer_size);

	}

}

t_exchanger* new_exchanger(t_map        *map ,
                           MPI_Datatype  type) {
	
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

	exchanger->mpi_exchange = (t_mpi_exchange *)malloc(sizeof(t_mpi_exchange));

	t_kernels* vtable_kernels = (t_kernels*)malloc(sizeof(t_kernels));
	select_un_pack_kernels(vtable_kernels, type);
	exchanger->vtable = vtable_kernels;

	exchanger->vtable_messages = (t_messages*)malloc(sizeof(t_messages));

	int hardware_type = get_config_hardware();
	switch (hardware_type) {
		case CPU:
			exchanger->vtable->allocator = allocator_cpu;
	}

	exchanger->map = map;

	MPI_Aint type_size;
	MPI_Aint type_lb;
	check_mpi( MPI_Type_get_extent(type, &type_lb, &type_size) );

	exchanger->mpi_exchange->type_size = type_size;

	exchanger->mpi_exchange->type = type;

	for (int count = 0; count < exchanger->map->exch_send->count; count++) {
		/* allocate the buffer */
		exchanger->exch_send->exch[count]->buffer = exchanger->vtable->allocator(exchanger->exch_send->exch[count]->buffer_size *
		                                                                         exchanger->mpi_exchange->type_size);
	}

	for (int count = 0; count < exchanger->map->exch_recv->count; count++) {
		/* allocate the buffer */
		exchanger->exch_recv->exch[count]->buffer = exchanger->vtable->allocator(exchanger->exch_recv->exch[count]->buffer_size *
		                                                                         exchanger->mpi_exchange->type_size);
	}

	int exchanger_type = get_config_exchanger();
	switch (exchanger_type) {
		case IsendIrecv:
			exchanger->mpi_exchange->req = (MPI_Request *)malloc((exchanger->map->exch_send->count +
			                                        exchanger->map->exch_recv->count) * sizeof(MPI_Request));
			exchanger->mpi_exchange->stat = (MPI_Status *)malloc((exchanger->map->exch_send->count + 
			                                        exchanger->map->exch_recv->count) * sizeof(MPI_Status));
			exchanger->vtable_messages->send_recv = exchanger_IsendIrecv;
			break;
		case IsendRecv1:
			exchanger->mpi_exchange->req = (MPI_Request *)malloc((exchanger->map->exch_send->count)*sizeof(MPI_Request));
			exchanger->mpi_exchange->stat = (MPI_Status *)malloc((exchanger->map->exch_send->count)*sizeof(MPI_Status));
			exchanger->vtable_messages->send_recv = exchanger_IsendRecv1;
			break;
		case IsendRecv2:
			exchanger->mpi_exchange->req = (MPI_Request *)malloc((exchanger->map->exch_send->count)*sizeof(MPI_Request));
			exchanger->mpi_exchange->stat = (MPI_Status *)malloc((exchanger->map->exch_send->count)*sizeof(MPI_Status));
			exchanger->vtable_messages->send_recv = exchanger_IsendRecv2;
			break;
	}

	return exchanger;
}

void exchanger_go(t_exchanger  *exchanger ,
                  void         *src_data  ,
                  void         *dst_data  ) {

	exchanger->vtable_messages->send_recv(exchanger->exch_send, exchanger->exch_recv,
	                                      exchanger->map, exchanger->vtable, exchanger->mpi_exchange,
	                                      src_data, dst_data);
}

void delete_exchanger(t_exchanger *exchanger) {

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
	free(exchanger->vtable_messages);

	free(exchanger->mpi_exchange->req);
	free(exchanger->mpi_exchange->stat);
	free(exchanger->mpi_exchange);

	free(exchanger);
}