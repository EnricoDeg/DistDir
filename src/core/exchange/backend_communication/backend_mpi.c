/*
 * @file backend_mpi.c
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
#include "src/core/exchange/backend_communication/backend_mpi.h"
#include "src/utils/check.h"

t_mpi_exchange * new_mpi_exchanger(MPI_Datatype  type, int size) {

	t_mpi_exchange *mpi_exchange = (t_mpi_exchange *)malloc(sizeof(t_mpi_exchange));
	if (type == MPI_INT || type == MPI_INTEGER) {
		mpi_exchange->isend = (kernel_func_isendirecv)mpi_wrapper_isend_int;
		mpi_exchange->irecv = (kernel_func_isendirecv)mpi_wrapper_irecv_int;
		mpi_exchange->recv  = (kernel_func_recv      )mpi_wrapper_recv_int ;
	} else if (type == MPI_REAL || type == MPI_FLOAT) {
		mpi_exchange->isend = (kernel_func_isendirecv)mpi_wrapper_isend_float;
		mpi_exchange->irecv = (kernel_func_isendirecv)mpi_wrapper_irecv_float;
		mpi_exchange->recv  = (kernel_func_recv      )mpi_wrapper_recv_float ;
	} else if (type == MPI_DOUBLE || type == MPI_DOUBLE_PRECISION) {
		mpi_exchange->isend = (kernel_func_isendirecv)mpi_wrapper_isend_double;
		mpi_exchange->irecv = (kernel_func_isendirecv)mpi_wrapper_irecv_double;
		mpi_exchange->recv  = (kernel_func_recv      )mpi_wrapper_recv_double ;
	}


	mpi_exchange->req = (MPI_Request *)malloc(size * sizeof(MPI_Request));
	mpi_exchange->stat = (MPI_Status *)malloc(size * sizeof(MPI_Status));

	MPI_Aint type_size;
	MPI_Aint type_lb;
	check_mpi( MPI_Type_get_extent(type, &type_lb, &type_size) );

	mpi_exchange->type_size = type_size;
	mpi_exchange->type = type;
	mpi_exchange->nreq_send = 0;
	mpi_exchange->nreq_recv = 0;
	mpi_exchange->wait = mpi_wrapper_waitall;

	return mpi_exchange;
}

void delete_mpi_exchanger(t_mpi_exchange *mpi_exchange) {

	free(mpi_exchange->req);
	free(mpi_exchange->stat);
	free(mpi_exchange);
}

void mpi_wrapper_isend_int(int *buffer, int count, MPI_Datatype datatype, int dest, int tag,
                           MPI_Comm comm, MPI_Request *request, int offset) {

	check_mpi( MPI_Isend(&buffer[offset], count, datatype, dest, tag, comm, request) );
}

void mpi_wrapper_isend_float(float *buffer, int count, MPI_Datatype datatype, int dest, int tag,
                             MPI_Comm comm, MPI_Request *request, int offset) {

	check_mpi( MPI_Isend(&buffer[offset], count, datatype, dest, tag, comm, request) );
}

void mpi_wrapper_isend_double(double *buffer, int count, MPI_Datatype datatype, int dest, int tag,
                              MPI_Comm comm, MPI_Request *request, int offset) {

	check_mpi( MPI_Isend(&buffer[offset], count, datatype, dest, tag, comm, request) );
}

void mpi_wrapper_irecv_int(int *buffer, int count, MPI_Datatype datatype, int source, int tag,
                           MPI_Comm comm, MPI_Request *request, int offset) {

	check_mpi( MPI_Irecv(&buffer[offset], count, datatype, source, tag, comm, request) );
}

void mpi_wrapper_irecv_float(float *buffer, int count, MPI_Datatype datatype, int source, int tag,
                             MPI_Comm comm, MPI_Request *request, int offset) {

	check_mpi( MPI_Irecv(&buffer[offset], count, datatype, source, tag, comm, request) );
}

void mpi_wrapper_irecv_double(double *buffer, int count, MPI_Datatype datatype, int source, int tag,
                              MPI_Comm comm, MPI_Request *request, int offset) {

	check_mpi( MPI_Irecv(&buffer[offset], count, datatype, source, tag, comm, request) );
}

void mpi_wrapper_recv_int(int *buffer, int count, MPI_Datatype datatype, int source, int tag,
                          MPI_Comm comm, MPI_Status *status, int offset) {

	check_mpi( MPI_Recv(&buffer[offset], count, datatype, source, tag, comm, status) );
}

void mpi_wrapper_recv_float(float *buffer, int count, MPI_Datatype datatype, int source, int tag,
                            MPI_Comm comm, MPI_Status *status, int offset) {

	check_mpi( MPI_Recv(&buffer[offset], count, datatype, source, tag, comm, status) );
}

void mpi_wrapper_recv_double(double *buffer, int count, MPI_Datatype datatype, int source, int tag,
                             MPI_Comm comm, MPI_Status *status, int offset) {

	check_mpi( MPI_Recv(&buffer[offset], count, datatype, source, tag, comm, status) );
}

void mpi_wrapper_waitall(int count, MPI_Request *requests, MPI_Status *statuses) {

	check_mpi( MPI_Waitall(count, requests, statuses) );
}
