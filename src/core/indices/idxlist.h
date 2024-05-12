/*
 * @file idxlist.h
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

#ifndef IDXLIST_H
#define IDXLIST_H

/** @struct t_idxlist
 * 
 *  @brief The structure contains information about an index list.
 * 
 */
struct t_idxlist {
	/** @brief Size of the index list */
	int count;
	/** @brief Array of indices in the index list */
	int *list;
};
typedef struct t_idxlist t_idxlist;

/**
 * @brief Create new index list
 * 
 * @details Create index list given an array of global indices and its size
 * 
 * @param[in] idx_array   Array of global indices
 * @param[in] num_indices Number of indices in the idx_array array
 * 
 * @return t_idxlist structure
 * 
 * @ingroup idxlist
 */
t_idxlist * new_idxlist(int *idx_array  ,
                        int  num_indices);

/**
 * @brief Create new empty index list
 * 
 * @details Create index list with size zero
 * 
 * @return t_idxlist structure
 * 
 * @ingroup idxlist
 */
t_idxlist * new_idxlist_empty();

/**
 * @brief Clean memory of a t_idxlist structure
 * 
 * @details Free all the memory of a t_idxlist structure
 * 
 * @param[in] idxlist pointer to t_idxlist structure
 * 
 * @ingroup idxlist
 */
void delete_idxlist(t_idxlist *idxlist);

#endif