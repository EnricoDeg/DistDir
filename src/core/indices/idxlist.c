/*
 * @file idxlist.c
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

#include <stdlib.h>

#include "src/core/indices/idxlist.h"
#include "src/utils/timer.h"

int timer_new_idxlist_id       = -1;
int timer_new_idxlist_empty_id = -1;
int timer_delete_idxlist_id    = -1;

t_idxlist * new_idxlist(int *idx_array  ,
                        int  num_indices) {

	if (timer_new_idxlist_id == -1)
		timer_new_idxlist_id = new_timer(__func__);

	timer_start(timer_new_idxlist_id);

	t_idxlist *idxlist;
	idxlist = (t_idxlist *)malloc(sizeof(t_idxlist));
	idxlist->count = num_indices;
	if (idxlist->count > 0)
		idxlist->list = (int *)malloc(idxlist->count * sizeof(int));
	for (int i = 0; i < idxlist->count; i++)
		idxlist->list[i] = idx_array[i];

	timer_stop(timer_new_idxlist_id);

	return idxlist;
}

t_idxlist * new_idxlist_empty() {

	if (timer_new_idxlist_empty_id == -1)
		timer_new_idxlist_empty_id = new_timer(__func__);

	timer_start(timer_new_idxlist_empty_id);

	t_idxlist *idxlist;
	idxlist = (t_idxlist *)malloc(sizeof(t_idxlist));
	idxlist->count = 0;
	idxlist->list = NULL;

	timer_stop(timer_new_idxlist_empty_id);

	return idxlist;
}

void delete_idxlist(t_idxlist *idxlist) {

	if (timer_delete_idxlist_id == -1)
		timer_delete_idxlist_id = new_timer(__func__);

	timer_start(timer_delete_idxlist_id);

	if (idxlist->count > 0)
		free(idxlist->list);
	free(idxlist);

	timer_stop(timer_delete_idxlist_id);
}
