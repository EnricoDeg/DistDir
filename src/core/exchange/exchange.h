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

#include "src/core/algorithm/map.h"
#include "src/setup/setting.h"
#include "src/core/exchange/backend_communication/backend_mpi.h"
#include "src/core/exchange/backend_hardware/backend_cpu.h"
#ifdef CUDA
#include "src/core/exchange/backend_hardware/backend_cuda.h"
#endif

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
	/** @brief pointer to buffer_idxlist **/
	int *buffer_idxlist;
};
typedef struct t_exchange t_exchange;

typedef void (*backend_func_wait) (t_mpi_exchange*);

/** @struct t_wait
 * 
 *  @brief The structure contains pointers to waiting functions
 * 
 */
struct t_wait {
	/** @brief pointer to backend_func_wait function to wait before the exchange */
	backend_func_wait pre_wait;
	/** @brief pointer to backend_func_wait function to wait after the exchange  */
	backend_func_wait post_wait;
};
typedef struct t_wait t_wait;

typedef void (*backend_func_go) (t_exchange *, t_exchange*,  t_map*, t_kernels*,
                                 t_mpi_exchange*, t_wait*, void*, void *, int *, int*);

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
	backend_func_go go;
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
 * @param[in] map  pointer to a t_map structure
 * @param[in] type type of the data in the form of MPI datatype
 * @param[in] hw   hardware location of the data on the MPI process
 * 
 * @ingroup exchange
 */
t_exchanger* new_exchanger(t_map        *map  ,
                           MPI_Datatype  type ,
                           distdir_hardware hw);

/**
 * @brief Arbitrary exchange given a map
 * 
 * @details Execute the MPI exchange given a previously generated exchanger.
 *          The source and destination buffers contain the data before and after the exchange
 * 
 * @param[in] exchanger     pointer to a t_exchanger structure
 * @param[in] src_data      pointer to the data to be sent
 * @param[in] dst_data      pointer to the data to be received
 * 
 * @ingroup exchange
 */
void exchanger_go(t_exchanger  *exchanger    ,
                  void         *src_data     ,
                  void         *dst_data     );

/**
 * @brief Arbitrary exchange given a map with transformation of memory layout
 * 
 * @details Execute the MPI exchange given a previously generated exchanger.
 *          The source and destination buffers contain the data before and after the exchange.
 *          A transformation array can be provided for source and destination.
 * 
 * @param[in] exchanger     pointer to a t_exchanger structure
 * @param[in] src_data      pointer to the data to be sent
 * @param[in] dst_data      pointer to the data to be received
 * @param[in] transform_src array of indices to define a memory layout transformation of the src_data
 * @param[in] transform_dst array of indices to define a memory layout transformation of the dst_data
 * 
 * @ingroup exchange
 */
void exchanger_go_with_transform(t_exchanger  *exchanger    ,
                                 void         *src_data     ,
                                 void         *dst_data     ,
                                 int          *transform_src,
                                 int          *transform_dst);

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