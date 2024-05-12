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

/**
 * @brief Assign each element of a given index list to a bucket.
 * 
 * @details The assignment of the index list elements to a bucket is straightforward.
 *          Given N total global indices and M total buckets, bucket 0 owns elements 0 - N/M-1,
 *          bucket 1 owns elements N/M - 2*N/M-1, etc.
 *  
 * @param[out] bucket_idxlist    integer array with the bucket location for each element of the index list
 * @param[in]  idxlist           integer array with the values of the index list (global indices)
 * @param[in]  bucket_min_size   the smaller bucket size in the RD decomposition
 * @param[in]  idxlist_size      size of the idxlist array
 * @param[in]  nbuckets          number of buckets
 * 
 * @ingroup backend
 */
void assign_idxlist_elements_to_buckets(      int *bucket_idxlist,
                                        const int *idxlist       ,
                                              int bucket_min_size,
                                              int idxlist_size   ,
                                              int nbuckets       );

/**
 * @brief Assign each element of a given index list to a bucket.
 * 
 * @details The assignment of the index list elements to a bucket is based on a single stride and
 *          exatrapolated from that.
 *          Given N total global indices in a stride and M total buckets, bucket 0 owns elements 0 - N/M-1,
 *          bucket 1 owns elements N/M - 2*N/M-1, etc. Then the full bucket indices can be extrapolated
 *          given the total number of indices and the stride size.
 *  
 * @param[out] bucket_idxlist    integer array with the bucket location for each element of the index list
 * @param[in]  idxlist           integer array with the values of the index list (global indices)
 * @param[in]  bucket_min_size   the smaller bucket size in the RD decomposition
 * @param[in]  idxlist_size      size of the idxlist array
 * @param[in]  nbuckets          number of buckets
 * @param[in]  bucket_stride     bucket stride
 * 
 * @ingroup backend
 */
void assign_idxlist_elements_to_buckets2(      int *bucket_idxlist,
                                        const int *idxlist       ,
                                              int bucket_min_size,
                                              int idxlist_size   ,
                                              int nbuckets       ,
                                              int bucket_stride  );

/**
 * @brief Return the number of processes that send information to this bucket.
 * 
 * @details Each process fill an array of size nbuckets with 0 or 1 if they send data to that bucket or not.
 *          The number of processes sending info to a bucket is than obtained with a call to MPI_Reduce_scatter.
 * 
 * @param[in] bucket_idxlist    integer array with the bucket location for each element of the index list
 * @param[in] nbuckets          number of buckets
 * @param[in] idxlist_size      size of the idxlist array
 * @param[in] comm              MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @return the number of processes that send info to the bucket
 * 
 * @ingroup backend
 */
int num_procs_send_to_each_bucket(const int      *bucket_idxlist,
                                        int       nbuckets      ,
                                        int       idxlist_size  ,
                                        MPI_Comm  comm          );

/**
 * @brief Each process compute the number of indices to send to each bucket
 * 
 * @details The bucket_idxlist contains the bucket location for each element with the elemnts
 *          sending to the same bucket contiguous in memory. 
 *  
 * @param[out] n_idx_each_bucket integer array with the number of elements to send to each bucket
 * @param[in]  bucket_idxlist    integer array with the bucket location for each element of the index list
 * @param[in]  idxlist_size      size of the idxlist array
 * @param[in]  nbuckets          number of buckets
 * 
 * @ingroup backend
 */
void num_indices_to_send_to_each_bucket(      int *n_idx_each_bucket,
                                        const int *bucket_idxlist   ,
                                              int  idxlist_size     ,
                                              int  nbuckets         );

/**
 * @brief Each bucket receives an array of size n_procs_sending_to_bucket with the rank ID which
 *        send indices to that bucket.
 * 
 * @details On the receiver size, the MPI process does not know who is the source of the message, so 
 *          MPI_ANY_SOURCE is used. This means that the array containing the MPI rank ID can have any order.
 *          For this reason, the array is sorted after the exchange.
 *  
 * @param[out] senders_to_bucket         integer array with the list of ranks sending indices to bucket
 * @param[in]  n_idx_each_bucket         integer array with the number of elements to send to each bucket
 * @param[in]  n_procs_sending_to_bucket the number of processes that send info to the bucket
 * @param[in]  bucket_max_size           Max size between all buckets
 * @param[in]  idxlist_size              size of the idxlist array 
 * @param[in]  comm                      MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @ingroup backend
 */
void senders_to_bucket(      int      *senders_to_bucket        ,
                       const int      *n_idx_each_bucket        , 
                             int       n_procs_sending_to_bucket,
                             int       bucket_max_size          ,
                             int       idxlist_size             ,
                             MPI_Comm  comm                     );

/**
 * @brief Each bucket receives an array of size n_procs_sending_to_bucket with the number of indices
 *        that each rank send to that bucket.
 * 
 * @details The bucket already knows which ranks send data to it, so it is straightforward to receive info
 *          about the number of indices that it will receive from those ranks.
 *  
 * @param[out] bucket_msg_size_senders   integer array with the number of indices that each rank sends to bucket
 * @param[in]  n_idx_each_bucket         integer array with the number of elements to send to each bucket
 * @param[in]  senders_to_bucket         integer array with the list of ranks sending indices to bucket
 * @param[in]  n_procs_sending_to_bucket the number of processes that send info to the bucket
 * @param[in]  bucket_max_size           Max size between all buckets
 * @param[in]  idxlist_size              size of the idxlist array
 * @param[in]  comm                      MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @ingroup backend
 */
void num_indices_to_bucket_from_each_rank(      int      *bucket_msg_size_senders  ,
                                          const int      *n_idx_each_bucket        ,
                                          const int      *senders_to_bucket        ,
                                                int       n_procs_sending_to_bucket,
                                                int       bucket_max_size          ,
                                                int       idxlist_size             ,
                                                MPI_Comm  comm                     );

/**
 * @brief Each bucket receives an array of size n_procs_sending_to_bucket with the MPI processes
 *        owning the index list element in the original decomposition
 * 
 * @details This allows to map the indices of the bucket, so RD decomposition, with the indices in the original 
 *          decomposition.
 *  
 * @param[out] bucket_ranks              integer array with the processes owning the index list element
 * @param[in]  n_idx_each_bucket         integer array with the number of elements to send to each bucket
 * @param[in]  bucket_msg_size_senders   integer array with the number of indices that each rank sends to bucket
 * @param[in]  senders_to_bucket         integer array with the list of ranks sending indices to bucket
 * @param[in]  n_procs_sending_to_bucket the number of processes that send info to the bucket
 * @param[in]  bucket_max_size           Max size between all buckets
 * @param[in]  idxlist_size              size of the idxlist array
 * @param[in]  comm                      MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @ingroup backend
 */
void bucket_idxlist_procs(      int      *bucket_ranks             ,
                          const int      *n_idx_each_bucket        ,
                          const int      *bucket_msg_size_senders  ,
                          const int      *senders_to_bucket        ,
                                int       n_procs_sending_to_bucket,
                                int       bucket_max_size          ,
                                int       idxlist_size            ,
                                MPI_Comm  comm                     );

/**
 * @brief Each bucket receives an array of size n_procs_sending_to_bucket with the indices 
 *        of the original decomposition belonging to the bucket.
 * 
 * @details The indices received are the one owned by the bucket, so the RD decomposition but grouped with 
 *          the ranks owning them in the original decomposition.
 *  
 * @param[out] bucket_indices            integer array with the processes owning the index list element
 * @param[in]  original_idxlist_sorted   integer array with the index list in the original domain decomposition
 *                                       sorted with ascending order of the bucket ID owning the indices
 * @param[in]  n_idx_each_bucket         integer array with the number of elements to send to each bucket
 * @param[in]  bucket_msg_size_senders   integer array with the number of indices that each rank sends to bucket
 * @param[in]  senders_to_bucket         integer array with the list of ranks sending indices to bucket
 * @param[in]  n_procs_sending_to_bucket the number of processes that send info to the bucket
 * @param[in]  bucket_max_size           Max size between all buckets
 * @param[in]  idxlist_size              size of the idxlist array
 * @param[in]  comm                      MPI communicator containing all the MPI procs involved in the RD decomposition
 * 
 * @ingroup backend
 */
void bucket_idxlist_elements(      int      *bucket_indices           ,
                             const int      *original_idxlist_sorted  ,
                             const int      *n_idx_each_bucket        ,
                             const int      *bucket_msg_size_senders  ,
                             const int      *senders_to_bucket        ,
                                   int       n_procs_sending_to_bucket,
                                   int       bucket_max_size          ,
                                   int       idxlist_size             ,
                                   MPI_Comm  comm                     );

