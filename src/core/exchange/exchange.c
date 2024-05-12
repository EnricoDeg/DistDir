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
#include <assert.h>

#include "src/core/exchange/exchange.h"
#include "src/utils/check.h"
#include "src/setup/setting.h"
#include "src/core/exchange/backend_hardware/backend_cpu.h"
#ifdef CUDA
#include "src/core/exchange/backend_hardware/backend_cuda.h"
#endif
#include "src/core/exchange/backend_communication/backend_mpi.h"
#include <stdio.h>

static void select_un_pack_kernels(t_kernels *table_kernels, MPI_Datatype type, distdir_hardware hw) {

	if (type == MPI_INT) {

		/* Packing / Unpacking functions */
		switch(hw) {
		case CPU:
			table_kernels->pack = (kernel_func_pack)pack_cpu_int;
			table_kernels->unpack = (kernel_func_pack)unpack_cpu_int;
			break;
#ifdef CUDA
		case GPU_NVIDIA:
			table_kernels->pack = (kernel_func_pack)pack_cuda_int;
			table_kernels->unpack = (kernel_func_pack)unpack_cuda_int;
			break;
#endif
		}
	} else if (type == MPI_REAL) {

		/* Packing / Unpacking functions */
		switch(hw) {
			case CPU:
				table_kernels->pack = (kernel_func_pack)pack_cpu_float;
				table_kernels->unpack = (kernel_func_pack)unpack_cpu_float;
				break;
#ifdef CUDA
			case GPU_NVIDIA:
				table_kernels->pack = (kernel_func_pack)pack_cuda_float;
				table_kernels->unpack = (kernel_func_pack)unpack_cuda_float;
				break;
#endif
		}
	} else if (type == MPI_DOUBLE) {

		/* Packing / Unpacking functions */
		switch(hw) {
			case CPU:
				table_kernels->pack = (kernel_func_pack)pack_cpu_double;
				table_kernels->unpack = (kernel_func_pack)unpack_cpu_double;
				break;
#ifdef CUDA
			case GPU_NVIDIA:
				table_kernels->pack = (kernel_func_pack)pack_cuda_double;
				table_kernels->unpack = (kernel_func_pack)unpack_cuda_double;
				break;
#endif
		}
	}
}

static void exchanger_waitall(t_mpi_exchange* mpi_exchange) {

	if ((mpi_exchange->nreq_send + mpi_exchange->nreq_recv) > 0)
		mpi_exchange->wait(mpi_exchange->nreq_send + mpi_exchange->nreq_recv,
		                       mpi_exchange->req, mpi_exchange->stat);
}

static void exchanger_waitall_dummy(t_mpi_exchange* mpi_exchange) {

	(void *)mpi_exchange;
}

static void exchanger_waitall_send(t_mpi_exchange* mpi_exchange) {

	if (mpi_exchange->nreq_send > 0)
		mpi_exchange->wait(mpi_exchange->nreq_send,
		                       mpi_exchange->req, mpi_exchange->stat);
}

static void exchanger_waitall_recv(t_mpi_exchange *mpi_exchange) {

	if (mpi_exchange->nreq_recv > 0)
		mpi_exchange->wait(mpi_exchange->nreq_recv,
		                       mpi_exchange->req, mpi_exchange->stat);
}

static void exchanger_IsendIrecv1(t_exchange *exch_send, t_exchange *exch_recv,
                                 t_map *map, t_kernels *vtable, t_mpi_exchange* mpi_exchange,
                                 t_wait *vtable_wait,
                                 void *src_data, void *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	vtable_wait->pre_wait(mpi_exchange);

	mpi_exchange->nreq_send = 0;
	mpi_exchange->nreq_recv = 0;
	int nreq = 0;

	for (int count = 0; count < map->exch_send->count; count++) {

		/* pack the buffer */
		int offset = map->exch_send->buffer_offset[count];

		int upper_bound = count == map->exch_send->count-1 ?
		                           map->exch_send->buffer_size :
		                           map->exch_send->buffer_offset[count + 1];

		int size = upper_bound - map->exch_send->buffer_offset[count];

		vtable->pack(exch_send->buffer,
		             src_data,
		             exch_send->buffer_idxlist,
		             size,
		             offset);

		/* send the buffer */
		mpi_exchange->isend(exch_send->buffer,
		                    size,
		                    mpi_exchange->type,
		                    map->exch_send->exch[count]->exch_rank,
		                    world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                    map->comm, mpi_exchange->req + nreq,
		                    offset);
		mpi_exchange->nreq_send++;
		nreq++;
	}

	// recv step
	for (int count = 0; count < map->exch_recv->count; count++) {

		int offset = map->exch_recv->buffer_offset[count];

		int upper_bound = count == map->exch_recv->count-1 ?
		                           map->exch_recv->buffer_size :
		                           map->exch_recv->buffer_offset[count + 1];

		int size = upper_bound - map->exch_recv->buffer_offset[count];

		mpi_exchange->irecv(exch_recv->buffer,
		                    size,
		                    mpi_exchange->type,
		                    map->exch_recv->exch[count]->exch_rank,
		                    map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                    map->comm, mpi_exchange->req + nreq,
		                    offset);
		mpi_exchange->nreq_recv++;
		nreq++;
	}

	/* wait for all messages */
	vtable_wait->post_wait(mpi_exchange);

	/* unpack all recv buffers */
	vtable->unpack(exch_recv->buffer,
	               dst_data,
	               exch_recv->buffer_idxlist,
	               map->exch_recv->buffer_size,
	               0);
}

static void exchanger_IsendIrecv2(t_exchange *exch_send, t_exchange *exch_recv,
                                 t_map *map, t_kernels *vtable, t_mpi_exchange* mpi_exchange,
                                 t_wait *vtable_wait,
                                 void *src_data, void *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	vtable_wait->pre_wait(mpi_exchange);

	mpi_exchange->nreq_send = 0;
	mpi_exchange->nreq_recv = 0;
	int nreq = 0;

	vtable->pack(exch_send->buffer,
		         src_data,
		         exch_send->buffer_idxlist,
		         map->exch_send->buffer_size,
		         0);

	for (int count = 0; count < map->exch_send->count; count++) {

		/* pack the buffer */
		int offset = map->exch_send->buffer_offset[count];

		int upper_bound = count == map->exch_send->count-1 ?
		                           map->exch_send->buffer_size :
		                           map->exch_send->buffer_offset[count + 1];

		int size = upper_bound - map->exch_send->buffer_offset[count];

		/* send the buffer */
		mpi_exchange->isend(exch_send->buffer,
		                    size,
		                    mpi_exchange->type,
		                    map->exch_send->exch[count]->exch_rank,
		                    world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                    map->comm, mpi_exchange->req + nreq,
		                    offset);
		mpi_exchange->nreq_send++;
		nreq++;
	}

	// recv step
	for (int count = 0; count < map->exch_recv->count; count++) {

		int offset = map->exch_recv->buffer_offset[count];

		int upper_bound = count == map->exch_recv->count-1 ?
		                           map->exch_recv->buffer_size :
		                           map->exch_recv->buffer_offset[count + 1];

		int size = upper_bound - map->exch_recv->buffer_offset[count];

		mpi_exchange->irecv(exch_recv->buffer,
		                    size,
		                    mpi_exchange->type,
		                    map->exch_recv->exch[count]->exch_rank,
		                    map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                    map->comm, mpi_exchange->req + nreq,
		                    offset);
		mpi_exchange->nreq_recv++;
		nreq++;
	}

	/* wait for all messages */
	vtable_wait->post_wait(mpi_exchange);

	/* unpack all recv buffers */
	vtable->unpack(exch_recv->buffer,
	               dst_data,
	               exch_recv->buffer_idxlist,
	               map->exch_recv->buffer_size,
	               0);
}

static void exchanger_IsendRecv1(t_exchange *exch_send, t_exchange *exch_recv,
                                 t_map *map, t_kernels *vtable, t_mpi_exchange* mpi_exchange,
                                 t_wait *vtable_wait,
                                 void *src_data, void *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	vtable_wait->pre_wait(mpi_exchange);

	mpi_exchange->nreq_send = 0;

	for (int count = 0; count < map->exch_send->count; count++) {

		/* pack the buffer */
		int offset = map->exch_send->buffer_offset[count];

		int upper_bound = count == map->exch_send->count-1 ?
		                           map->exch_send->buffer_size :
		                           map->exch_send->buffer_offset[count + 1];

		int size = upper_bound - map->exch_send->buffer_offset[count];

		vtable->pack(exch_send->buffer,
		             src_data,
		             exch_send->buffer_idxlist,
		             size, offset);

		/* send the buffer */
		mpi_exchange->isend(exch_send->buffer,
		                    size,
		                    mpi_exchange->type,
		                    map->exch_send->exch[count]->exch_rank,
		                    world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                    map->comm, mpi_exchange->req + mpi_exchange->nreq_send,
		                    offset);
		mpi_exchange->nreq_send++;
	}

	/* wait for all messages */
	vtable_wait->post_wait(mpi_exchange);

	// recv step
	for (int count = 0; count < map->exch_recv->count; count++) {

		int offset = map->exch_recv->buffer_offset[count];
		
		int upper_bound = count == map->exch_recv->count-1 ?
		                           map->exch_recv->buffer_size :
		                           map->exch_recv->buffer_offset[count + 1];

		int size = upper_bound - map->exch_recv->buffer_offset[count];

		mpi_exchange->recv(exch_recv->buffer,
		                   size,
		                   mpi_exchange->type,
		                   map->exch_recv->exch[count]->exch_rank,
		                   map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                   map->comm, mpi_exchange->stat,
		                   offset);
	}

	/* unpack all recv buffers */
	vtable->unpack(exch_recv->buffer,
	               dst_data,
	               exch_recv->buffer_idxlist,
	               map->exch_recv->buffer_size,
	               0);
}

static void exchanger_IsendRecv2(t_exchange *exch_send, t_exchange *exch_recv,
                                 t_map *map, t_kernels *vtable, t_mpi_exchange* mpi_exchange,
                                 t_wait *vtable_wait,
                                 void *src_data, void *dst_data) {

	int world_size;
	check_mpi( MPI_Comm_size(map->comm, &world_size) );
	int world_rank;
	check_mpi( MPI_Comm_rank(map->comm, &world_rank) );

	vtable_wait->pre_wait(mpi_exchange);

	mpi_exchange->nreq_send = 0;

	for (int count = 0; count < map->exch_send->count; count++) {

		/* pack the buffer */
		int offset = map->exch_send->buffer_offset[count];

		int upper_bound = count == map->exch_send->count-1 ?
		                           map->exch_send->buffer_size :
		                           map->exch_send->buffer_offset[count + 1];

		int size = upper_bound - map->exch_send->buffer_offset[count];

		vtable->pack(exch_send->buffer,
		             src_data,
		             exch_send->buffer_idxlist,
		             size, offset);

		/* send the buffer */
		mpi_exchange->isend(exch_send->buffer,
		                    size,
		                    mpi_exchange->type,
		                    map->exch_send->exch[count]->exch_rank,
		                    world_rank + world_size * (map->exch_send->exch[count]->exch_rank + 1),
		                    map->comm, mpi_exchange->req + mpi_exchange->nreq_send,
		                    offset);
		mpi_exchange->nreq_send++;
	}

	/* wait for all messages */
	vtable_wait->post_wait(mpi_exchange);

	// recv and unpack step
	for (int count = 0; count < map->exch_recv->count; count++) {

		int offset = map->exch_recv->buffer_offset[count];

		int upper_bound = count == map->exch_recv->count-1 ?
		                           map->exch_recv->buffer_size :
		                           map->exch_recv->buffer_offset[count + 1];

		int size = upper_bound - map->exch_recv->buffer_offset[count];

		mpi_exchange->recv(exch_recv->buffer,
		                   size,
		                   mpi_exchange->type,
		                   map->exch_recv->exch[count]->exch_rank,
		                   map->exch_recv->exch[count]->exch_rank + world_size * (world_rank + 1),
		                   map->comm, mpi_exchange->stat,
		                   offset);

		vtable->unpack(exch_recv->buffer,
		               dst_data,
		               exch_recv->buffer_idxlist,
		               size, offset);
	}
}

t_exchanger* new_exchanger(t_map        *map  ,
                           MPI_Datatype  type ,
                           distdir_hardware hw) {

#ifdef ERROR_CHECK
	assert(type == MPI_INT || type == MPI_REAL || type == MPI_DOUBLE);
	assert(hw == CPU       || hw == GPU_NVIDIA || hw == GPU_AMD     );
#endif

	// group all info into data structure
	t_exchanger *exchanger;

	exchanger = (t_exchanger *)malloc(sizeof(t_exchanger));
	exchanger->exch_send = (t_exchange *)malloc(sizeof(t_exchange));
	exchanger->exch_recv = (t_exchange *)malloc(sizeof(t_exchange));

	exchanger->exch_send->count = map->exch_send->count;
	exchanger->exch_send->buffer_size = map->exch_send->buffer_size;

	exchanger->exch_recv->count = map->exch_recv->count;
	exchanger->exch_recv->buffer_size = map->exch_recv->buffer_size;

	t_kernels* vtable_kernels = (t_kernels*)malloc(sizeof(t_kernels));
	select_un_pack_kernels(vtable_kernels, type, hw);
	exchanger->vtable = vtable_kernels;

	exchanger->vtable_wait = (t_wait*)malloc(sizeof(t_wait));

	switch (hw) {
		case CPU:
			exchanger->vtable->allocator = allocator_cpu;
			exchanger->vtable->deallocator = deallocator_cpu;
			exchanger->exch_send->buffer_idxlist = map->exch_send->buffer_idxlist;
			exchanger->exch_recv->buffer_idxlist = map->exch_recv->buffer_idxlist;
			break;
#ifdef CUDA
		case GPU_NVIDIA:
			exchanger->vtable->allocator = allocator_cuda;
			exchanger->vtable->deallocator = deallocator_cuda;
			exchanger->exch_send->buffer_idxlist = map->exch_send->buffer_idxlist_gpu;
			exchanger->exch_recv->buffer_idxlist = map->exch_recv->buffer_idxlist_gpu;
			break;
#endif
	}

	exchanger->map = map;
	int mpi_size;
	int exchanger_type = get_config_exchanger();
	switch (exchanger_type) {
		case IsendIrecv1:
			mpi_size = exchanger->map->exch_send->count + exchanger->map->exch_recv->count;
			exchanger->go = exchanger_IsendIrecv1;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_dummy;
			exchanger->vtable_wait->post_wait = exchanger_waitall;
			break;
		case IsendIrecv2:
			mpi_size = exchanger->map->exch_send->count + exchanger->map->exch_recv->count;
			exchanger->go = exchanger_IsendIrecv2;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_dummy;
			exchanger->vtable_wait->post_wait = exchanger_waitall;
			break;
		case IsendRecv1:
			mpi_size = exchanger->map->exch_send->count;
			exchanger->go = exchanger_IsendRecv1;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_dummy;
			exchanger->vtable_wait->post_wait = exchanger_waitall;
			break;
		case IsendRecv2:
			mpi_size = exchanger->map->exch_send->count;
			exchanger->go = exchanger_IsendRecv2;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_dummy;
			exchanger->vtable_wait->post_wait = exchanger_waitall;
			break;
		case IsendIrecv1NoWait:
			/* This exchange type can be used only with one direction exchange */
#ifdef ERROR_CHECK
			assert(exchanger->map->exch_send->count == 0 || exchanger->map->exch_recv->count == 0);
#endif
			mpi_size = exchanger->map->exch_send->count + exchanger->map->exch_recv->count;
			exchanger->go = exchanger_IsendIrecv1;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_send;
			exchanger->vtable_wait->post_wait = exchanger_waitall_recv;
			break;
		case IsendIrecv2NoWait:
			/* This exchange type can be used only with one direction exchange */
#ifdef ERROR_CHECK
			assert(exchanger->map->exch_send->count == 0 || exchanger->map->exch_recv->count == 0);
#endif
			mpi_size = exchanger->map->exch_send->count + exchanger->map->exch_recv->count;
			exchanger->go = exchanger_IsendIrecv2;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_send;
			exchanger->vtable_wait->post_wait = exchanger_waitall_recv;
			break;
		case IsendRecv1NoWait:
			/* This exchange type can be used only with one direction exchange */
#ifdef ERROR_CHECK
			assert(exchanger->map->exch_send->count == 0 || exchanger->map->exch_recv->count == 0);
#endif
			mpi_size = exchanger->map->exch_send->count;
			exchanger->go = exchanger_IsendRecv1;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_send;
			exchanger->vtable_wait->post_wait = exchanger_waitall_recv;
			break;
		case IsendRecv2NoWait:
			/* This exchange type can be used only with one direction exchange */
#ifdef ERROR_CHECK
			assert(exchanger->map->exch_send->count == 0 || exchanger->map->exch_recv->count == 0);
#endif
			mpi_size = exchanger->map->exch_send->count;
			exchanger->go = exchanger_IsendRecv2;
			exchanger->vtable_wait->pre_wait = exchanger_waitall_send;
			exchanger->vtable_wait->post_wait = exchanger_waitall_recv;
			break;
	}
	exchanger->mpi_exchange = new_mpi_exchanger(type, mpi_size);

	/* allocate the buffer */
	if (exchanger->exch_send->buffer_size > 0)
		exchanger->exch_send->buffer = exchanger->vtable->allocator(exchanger->exch_send->buffer_size *
		                                                            exchanger->mpi_exchange->type_size);

	if (exchanger->exch_recv->buffer_size > 0)
		exchanger->exch_recv->buffer = exchanger->vtable->allocator(exchanger->exch_recv->buffer_size *
		                                                            exchanger->mpi_exchange->type_size);

	return exchanger;
}

void exchanger_go(t_exchanger  *exchanger ,
                  void         *src_data  ,
                  void         *dst_data  ) {

	exchanger->go(exchanger->exch_send,
	              exchanger->exch_recv,
	              exchanger->map,
	              exchanger->vtable,
	              exchanger->mpi_exchange,
	              exchanger->vtable_wait,
	              src_data, dst_data);
}

void delete_exchanger(t_exchanger *exchanger) {

	/* Wait for possible send messages (because of no wait in final step) */
	exchanger->vtable_wait->pre_wait(exchanger->mpi_exchange);

	// free memory
	if (exchanger->exch_send->buffer_size > 0)
		 exchanger->vtable->deallocator(exchanger->exch_send->buffer);

	if (exchanger->exch_recv->buffer_size > 0)
		 exchanger->vtable->deallocator(exchanger->exch_recv->buffer);

	if (exchanger->exch_send->count > 0) {
		free(exchanger->exch_send);
	}

	if (exchanger->exch_recv->count > 0) {
		free(exchanger->exch_recv);
	}

	free(exchanger->vtable);
	free(exchanger->vtable_wait);

	delete_mpi_exchanger(exchanger->mpi_exchange);

	free(exchanger);
}