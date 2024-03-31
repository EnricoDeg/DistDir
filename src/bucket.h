/*
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
 *  @var size          Number of global indices in the bucket
 *  @var idxlist       Array of indices inside the bucket sorted in ascending order of the MPI processes owning them.
 *                     Size is size.
 *  @var ranks         Array of MPI ranks owning the indices in the idxlist array
 *                     Size is size.
 *  @var count_recv    Number of MPI processes that the bucket receives data from.
 *  @var src_recv      Array of MPI processes sending data to the bucket.
 *                     Size is count_recv.
 *  @var msg_size_recv Array of number of indices that each MPI ranks in src_recv send to the bucket.
 *                     Size is count_recv.
 *  @var size_ranks    Array of number of indices that the MPI process send to each bucket.
 *                     Size is the number of MPI processes in the communicator.
 *  @var rank_exch     Array of MPI ranks to send / recv data to / from.
 *                     This is the result of the distributed directory algorithm.
 *                     Each element of the array is associated with an element of the idxlist in the same order
 *                     that the list was given to new_idxlist function.
 *                     Size is the same of the idxlist->count.
 * 
 *  @ingroup data_structure
 */
struct t_bucket {
	int min_size;
	int size;
	int max_size;
	int *idxlist;
	int *ranks;
	int count_recv;
	int *src_recv;
	int *msg_size_recv;
	int *size_ranks;
	int *rank_exch;
};
typedef struct t_bucket t_bucket;

void map_idxlist_to_RD_decomp(t_bucket *bucket, t_idxlist *idxlist, int *idxlist_local, int nbuckets, MPI_Comm comm);
void map_RD_decomp_to_idxlist(t_bucket *src_bucket, t_bucket *dst_bucket, int *idxlist_local,
                              int idxlist_size, int nbuckets, MPI_Comm comm);

#endif