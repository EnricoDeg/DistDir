/*
 * @file backend_mpi.h
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

#ifndef BACKEND_MPI_H
#define BACKEND_MPI_H

#include "mpi.h"

/**
 * @brief Lightweight wrapper around MPI_Isend for integer buffers.
 *  
 * @param[in]  buffer   buffer to be sent
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  dest     rank to send the message to
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] request  MPI request for non blocking messages
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_isend_int(int *buffer, int count, MPI_Datatype datatype, int dest, int tag,
                           MPI_Comm comm, MPI_Request *request, int offset);

/**
 * @brief Lightweight wrapper around MPI_Isend for float buffers.
 *  
 * @param[in]  buffer   buffer to be sent
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  dest     rank to send the message to
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] request  MPI request for non blocking messages
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_isend_float(float *buffer, int count, MPI_Datatype datatype, int dest, int tag,
                             MPI_Comm comm, MPI_Request *request, int offset);

/**
 * @brief Lightweight wrapper around MPI_Isend for double buffers.
 *  
 * @param[in]  buffer   buffer to be sent
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  dest     rank to send the message to
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] request  MPI request for non blocking messages
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_isend_double(double *buffer, int count, MPI_Datatype datatype, int dest, int tag,
                              MPI_Comm comm, MPI_Request *request, int offset);

/**
 * @brief Lightweight wrapper around MPI_Irecv for integer buffers.
 *  
 * @param[in]  buffer   buffer to be received
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  source     rank to receive the message from
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] request  MPI request for non blocking messages
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_irecv_int(int *buffer, int count, MPI_Datatype datatype, int source, int tag,
                           MPI_Comm comm, MPI_Request *request, int offset);

/**
 * @brief Lightweight wrapper around MPI_Irecv for float buffers.
 *  
 * @param[in]  buffer   buffer to be received
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  source     rank to receive the message from
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] request  MPI request for non blocking messages
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_irecv_float(float *buffer, int count, MPI_Datatype datatype, int source, int tag,
                             MPI_Comm comm, MPI_Request *request, int offset);

/**
 * @brief Lightweight wrapper around MPI_Irecv for double buffers.
 *  
 * @param[in]  buffer   buffer to be received
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  source     rank to receive the message from
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] request  MPI request for non blocking messages
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_irecv_double(double *buffer, int count, MPI_Datatype datatype, int source, int tag,
                              MPI_Comm comm, MPI_Request *request, int offset);

/**
 * @brief Lightweight wrapper around MPI_Recv for integer buffers.
 *  
 * @param[in]  buffer   buffer to be received
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  source     rank to receive the message from
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] status   message status
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_recv_int(int *buffer, int count, MPI_Datatype datatype, int source, int tag,
                          MPI_Comm comm, MPI_Status *status, int offset);

/**
 * @brief Lightweight wrapper around MPI_Recv for float buffers.
 *  
 * @param[in]  buffer   buffer to be received
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  source     rank to receive the message from
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] status   message status
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_recv_float(float *buffer, int count, MPI_Datatype datatype, int source, int tag,
                            MPI_Comm comm, MPI_Status *status, int offset);

/**
 * @brief Lightweight wrapper around MPI_Recv for double buffers.
 *  
 * @param[in]  buffer   buffer to be received
 * @param[in]  count    size of the message
 * @param[in]  datatype data type of buffer
 * @param[in]  source     rank to receive the message from
 * @param[in]  tag      message tag
 * @param[in]  comm     MPI communicator
 * @param[out] status   message status
 * @param[in]  offset   buffer offset
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_recv_double(double *buffer, int count, MPI_Datatype datatype, int source, int tag,
                             MPI_Comm comm, MPI_Status *status, int offset);

/**
 * @brief Lightweight wrapper around MPI_Waitall.
 *  
 * @param[in]  count    number of non blocking messages
 * @param[in]  requests array of MPI request
 * @param[out] statuses array of message statuses
 * 
 * @ingroup backend_mpi
 */
void mpi_wrapper_waitall(int count, MPI_Request *requests, MPI_Status *statuses);

#endif