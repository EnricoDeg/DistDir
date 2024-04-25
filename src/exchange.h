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

typedef void (*kernel_func) (void*, void*, int*, int);

/** @struct xt_un_pack_kernels
 * 
 *  @brief The structure contains pointer to pack and unpack functions
 * 
 */
struct xt_un_pack_kernels {
	/** @brief pointer to pack function */
	kernel_func pack;
	/** @brief pointer to unpack function */
	kernel_func unpack;
};
typedef struct xt_un_pack_kernels xt_un_pack_kernels;

/** @struct t_map_exch_per_rank
 * 
 *  @brief The structure contains buffers for each exchange
 * 
 */
struct t_exchange_per_rank {
	/** @brief size of the exchange message */
	int buffer_size;
	/** @brief buffer to store the message */
	void *buffer;
};
typedef struct t_exchange_per_rank t_exchange_per_rank;

/** @struct t_map_exch
 * 
 *  @brief The structure contains all buffers in one direction
 * 
 */
struct t_exchange {
	/** @brief number of exchanges */
	int count;
	/** @brief array of pointers to t_exchange_per_rank structure */
	t_exchange_per_rank **exch;
};
typedef struct t_exchange t_exchange;

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
	xt_un_pack_kernels* vtable;
	/** @brief pointer to map object */
	t_map *map;
};
typedef struct t_exchanger t_exchanger;

/**
 * @brief Arbitrary exchange given a map
 * 
 * @details Execute the MPI exchange given a previously generated map.
 *          The source and destination buffers contain the data before and after the exchange
 * 
 * @param[in] map      pointer to a t_map structure
 * @param[in] type     type of the data in the form of MPI datatype
 * @param[in] src_data pointer to the data to be sent
 * @param[in] dst_data pointer to the data to be received
 * 
 * @ingroup exchange
 */
void exchange_go(t_map        *map     ,
                 MPI_Datatype  type    ,
                 void         *src_data,
                 void         *dst_data);

#endif