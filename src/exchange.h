/*
 * @file exchange.h
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

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "map.h"

/** @struct t_map_exch
 * 
 *  @brief The structure contains all buffers in one direction
 * 
 */
struct t_exchange {
	/** @brief number of exchanges */
	int count;
	/** @brief total size of the exchange messages */
	int buffer_size;
	/** @brief buffer to store the messages */
	void *buffer;
};
typedef struct t_exchange t_exchange;

typedef void (*kernel_backend_func_wait) (int, MPI_Request *, MPI_Status *);

struct t_mpi_exchange {
	/** @brief MPI datatype used for the exchange */
	MPI_Datatype type;
	/** @brief Size of the MPI datatype used for the exchange */
	MPI_Aint type_size;
	/** @brief array of message requests */
	MPI_Request *req;
	/** @brief array of message status */
	MPI_Status *stat;
	/** @brief number of send message requests */
	int nreq_send;
	/** @brief number of recv message requests */
	int nreq_recv;
	/** @brief communication library wait function */
	kernel_backend_func_wait wait;
};
typedef struct t_mpi_exchange t_mpi_exchange;

typedef void (*kernel_func_pack) ( void*, void*, int*, int, int );

typedef void (*kernel_func_isend) ( void *, int , MPI_Datatype, int, int,
                                    MPI_Comm, MPI_Request *, int ) ;

typedef void (*kernel_func_irecv) ( void *, int, MPI_Datatype, int, int,
                                    MPI_Comm, MPI_Request *, int ) ;

typedef void (*kernel_func_recv)  ( void *, int, MPI_Datatype, int, int,
                                    MPI_Comm, MPI_Status * , int) ;

typedef void* (*kernel_func_alloc) (size_t);

/** @struct xt_un_pack_kernels
 * 
 *  @brief The structure contains pointer to pack and unpack functions
 * 
 */
struct t_kernels {
	/** @brief pointer to pack function */
	kernel_func_pack pack;
	/** @brief pointer to unpack function */
	kernel_func_pack unpack;
	/** @brief pointer to isend function */
	kernel_func_isend isend;
	/** @brief pointer to irecv function */
	kernel_func_irecv irecv;
	/** @brief pointer to recv function */
	kernel_func_recv recv;
	/** @brief pointer to allocate function */
	kernel_func_alloc allocator;
};
typedef struct t_kernels t_kernels;

typedef void (*kernel_func_wait) (t_mpi_exchange*);

struct t_wait {
	kernel_func_wait pre_wait;
	kernel_func_wait post_wait;
};
typedef struct t_wait t_wait;

typedef void (*kernel_func_go) (t_exchange *, t_exchange*,  t_map*, t_kernels*, t_mpi_exchange*, t_wait*, void*, void *);

struct t_messages {
	/** @brief pointer to backend send/recv function */
	kernel_func_go send_recv;
};
typedef struct t_messages t_messages;

/** @struct t_map_exch
 * 
 *  @brief The structure contains all buffers associated with a map
 * 
 */
struct t_exchanger {
	/** @brief pointer to t_exchange structure to store send information */
	t_exchange *exch_send;
	/** @brief pointer to t_exchange structure to store receive information */
	t_exchange *exch_recv;
	/** @brief pointer to pack and unpack functions */
	t_kernels* vtable;
	/** @brief pointer to pack and unpack functions */
	t_messages* vtable_messages;
	/** @brief pointer to wait functions */
	t_wait* vtable_wait;
	/** @brief pointer to map object */
	t_map *map;
	/** @brief pointer to mpi_exchange struct */
	t_mpi_exchange *mpi_exchange;
};
typedef struct t_exchanger t_exchanger;

/**
 * @brief Create a new t_exchanger structure
 * 
 * @details Create an exchanger given a map object and a datatype
 * 
 * @param[in] map      pointer to a t_map structure
 * @param[in] type     type of the data in the form of MPI datatype
 * 
 * @ingroup exchange
 */
t_exchanger* new_exchanger(t_map        *map ,
                           MPI_Datatype  type);

/**
 * @brief Arbitrary exchange given a map
 * 
 * @details Execute the MPI exchange given a previously generated exchanger.
 *          The source and destination buffers contain the data before and after the exchange
 * 
 * @param[in] exchanger pointer to a t_exchanger structure
 * @param[in] src_data  pointer to the data to be sent
 * @param[in] dst_data  pointer to the data to be received
 * 
 * @ingroup exchange
 */
void exchanger_go(t_exchanger  *exchanger,
                  void         *src_data ,
                  void         *dst_data );

/**
 * @brief Clean memory of a t_exchanger structure
 * 
 * @details Free all the memory of a t_exchanger structure
 * 
 * @param[in] exchanger pointer to t_exchanger structure
 * 
 * @ingroup exchange
 */
void delete_exchanger(t_exchanger * exchanger);

#endif