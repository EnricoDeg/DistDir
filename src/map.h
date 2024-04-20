/*
 * @file map.h
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

#ifndef MAP_H
#define MAP_H

#include "mpi.h"

#include "idxlist.h"

/** @struct t_map_exch_per_rank
 * 
 *  @brief The structure contains information about each exchange
 * 
 */
struct t_map_exch_per_rank {
	/** @brief rank to send/recv data to/from */
	int exch_rank;
	/** @brief size of the exchange message */
	int buffer_size;
	/** @brief idxlist to create the exchange buffer */
	int *buffer_idxlist;
	/** @brief buffer to store the message */
	void *buffer;
};
typedef struct t_map_exch_per_rank t_map_exch_per_rank;

/** @struct t_map_exch
 * 
 *  @brief The structure contains information about all the exchanges in one direction
 * 
 */
struct t_map_exch {
	/** @brief number of exchanges */
	int count;
	/** @brief array of pointers to t_map_exch_per_rank structure */
	t_map_exch_per_rank **exch;
};
typedef struct t_map_exch t_map_exch;

/** @struct t_map_exch
 * 
 *  @brief The structure contains information about all the exchanges associated with
 *         a source idxlist and a destination index list.
 * 
 */
struct t_map {
	/** @brief MPI communicator containing all the processes involved in the exchange */
	MPI_Comm comm;
	/** @brief pointer to t_map_exch structure to store send information */
	t_map_exch *exch_send;
	/** @brief pointer to t_map_exch structure to store receive information */
	t_map_exch *exch_recv;
};
typedef struct t_map t_map;

/**
 * @brief Create a new t_map structure
 * 
 * @details Create a map given a source index list and a destination index list
 * 
 * @param[in] src_idxlist pointer to source index list
 * @param[in] dst_idxlist pointer to destination index list
 * @param[in] comm        MPI communicator containing all the MPI procs involved in the exchange
 * 
 * @ingroup map
 */
t_map * new_map(t_idxlist *src_idxlist ,
                t_idxlist *dst_idxlist ,
                MPI_Comm   comm        );

/**
 * @brief Create a new t_map structure for 3D decomposition
 * 
 * @details Create a map given an existing map based on 2D decomposition
 * 
 * @param[in] map2d   pointer to t_map structure based on 2D decomposition
 * @param[in] nlevels number of vertical levels of the domain
 * 
 * @ingroup map
 */
t_map * extend_map_3d(t_map *map2d  ,
                      int    nlevels);

/**
 * @brief Clean memory of a t_map structure
 * 
 * @details Free all the memory of a t_map structure
 * 
 * @param[in] map pointer to t_map structure
 * 
 * @ingroup map
 */
void delete_map(t_map *map);

#endif