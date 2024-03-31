/*
 * @file backend.h
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

#include "mpi.h"

void assign_idxlist_elements_to_buckets(int *bucket_idxlist, const int *idxlist,
                                        int bucket_min_size,
                                        int idxlist_size, int nbuckets);

int get_n_procs_send_to_bucket(const int *bucket_idxlist, int nbuckets, int idxlist_size, MPI_Comm comm);

void num_indices_to_send_to_each_bucket(int *size_ranks, int *bucket_idxlist, int idxlist_size, int nranks);

void get_receivers_bucket(int *bucket_src_recv, const int *bucket_size_ranks, 
                          int  bucket_count_recv, int bucket_max_size, int idxlist_count, MPI_Comm comm);

void get_n_indices_from_each_process(int *bucket_msg_size_recv, 
                                     const int *bucket_size_ranks, const int *bucket_src_recv,
                                     int bucket_count_recv, int bucket_max_size,
                                     int idxlist_count, MPI_Comm comm);

void bucket_idxlist_procs(int *bucket_ranks,
                          const int *bucket_size_ranks, const int *bucket_msg_size_recv,
                          const int *bucket_src_recv,
                          int bucket_count_recv, int bucket_max_size,
                          int idxlist_count, MPI_Comm comm);

void bucket_idxlist_elements(int *bucket_idxlist,
                             const int *src_idxlist_sort,
                             const int *bucket_size_ranks, const int *bucket_msg_size_recv,
                             const int *bucket_src_recv,
                             int bucket_count_recv, int bucket_max_size,
                             int idxlist_count, MPI_Comm comm);

