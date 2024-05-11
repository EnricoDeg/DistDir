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

#include "backend_mpi.h"
#include "check.h"

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