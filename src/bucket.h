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

struct t_bucket {
	int size;
	int *idxlist;
	int *ranks;
	int count_recv;
	int *src_recv;
	int *msg_size_recv;
	int *size_ranks;
	int *rank_exch;
};
typedef struct t_bucket t_bucket;

int get_n_receiver_bucket(int *bucket_idxlist, int nbuckets, int idxlist_size, MPI_Comm comm);
void get_n_indices_for_each_bucket(int *size_ranks, int *bucket_idxlist, int idxlist_size, int nranks);
void map_idxlist_to_RD_decomp(t_bucket *bucket, t_idxlist *idxlist, int *idxlist_local, int nbuckets, MPI_Comm comm);
void map_RD_decomp_to_idxlist(t_bucket *src_bucket, t_bucket *dst_bucket, int *idxlist_local,
                              int idxlist_size, int nbuckets, MPI_Comm comm);

#endif