/**
 * @file bucket.h
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

#ifndef BUCKET_H
#define BUCKET_H

#include "mpi.h"
#include "idxlist.h"

/** @struct t_bucket
 * 
 *  @brief The structure contains information about each bucket.
 * 
 */
struct t_bucket {
	/** @brief Minimum number of global indices across all buckets */
	int min_size;
	/** @brief Number of global indices in the bucket */
	int size;
	/** @brief Maximum number of global indices across all buckets */
	int max_size;
	/** @brief Array of indices inside the bucket sorted in ascending order of the MPI processes owning them.
               Size is sizes */
	int *idxlist;
	/** @brief Array of MPI ranks owning the indices in the idxlist array
	           Size is size. */
	int *ranks;
	/** @brief Number of MPI processes that the bucket receives data from. */
	int count_recv;
	/** @brief Array of MPI processes sending data to the bucket.
	           Size is count_recv. */
	int *src_recv;
	/** @brief Array of number of indices that each MPI ranks in src_recv send to the bucket.
	           Size is count_recv. */
	int *msg_size_recv;
	/** @brief Array of number of indices that the MPI process send to each bucket.
	           Size is the number of MPI processes in the communicator. */
	int *size_ranks;
	/** @brief Array of MPI ranks to send / recv data to / from.
	   This is the result of the distributed directory algorithm.
	   Each element of the array is associated with an element of the idxlist in the same order
	   that the list was given to new_idxlist function.
	   Size is the same of the idxlist->count. */
	int *rank_exch;     ///< 
};
typedef struct t_bucket t_bucket;

/**
 * @brief Map a provided index list into a RD decomposition
 * 
 * @details Each rank in the MPI communicator is part of the RD decomposition and receive information
 *          about the ranks owning the indices its bucket (subdomain of the RD decomposition). 
 * 
 * @param[in] bucket        t_bucket object containing information about the RD decomposition
 * @param[in] idxlist       t_idxlist object containing the MPI rank index list which needs to be mapped
 *                          to the RD decomposition
 * @param[in] idxlist_local array containing the local indices of the index list
 * @param[in] nbuckets      number of buckets
 * @param[in] comm          MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @ingroup bucket
 */
void map_idxlist_to_RD_decomp(t_bucket  *bucket       ,
                              t_idxlist *idxlist      ,
                              int       *idxlist_local,
                              int        nbuckets     ,
                              MPI_Comm comm           );

/**
 * @brief Map source and destination information in the RD decomposition to the original decompositions
 * 
 * @details Each rank owns information about source and destination in the original decompositions of 
 *          its own indices. With that information, it can provide information to the original decompositions
 *          about the exchange.
 * 
 * @param[in] src_bucket    t_bucket object containing information about the RD decomposition for the source idxlist
 * @param[in] dst_bucket    t_bucket object containing information about the RD decomposition for the destination idxlist
 * @param[in] idxlist_local array containing the local indices of the index list in the original decomposition
 * @param[in] idxlist_size  size of the idxlist owned by the MPI rank in the original decomposition
 * @param[in] nbuckets      number of buckets
 * @param[in] comm          MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @ingroup bucket
 */
void map_RD_decomp_to_idxlist(t_bucket *src_bucket   ,
                              t_bucket *dst_bucket   ,
                              int      *idxlist_local,
                              int       idxlist_size ,
                              int       nbuckets     ,
                              MPI_Comm  comm         );

#endif