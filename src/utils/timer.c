/*
 * @file timer.c
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

#include <string.h>
#include <stdlib.h>
#include "src/utils/timer.h"

static int timer_count = 0;
static t_list_node *list_head = NULL;

static int new_timer_node(t_list_node **list_tail, const char * timer_name) {

	timer_count++;
	t_timer_data *timer_data = (t_timer_data *)malloc(sizeof(t_timer_data));
	timer_data->id = timer_count;
	timer_data->name = strdup(timer_name);
	timer_data->total_time = 0.0;
	timer_data->start_time = -1.0;
	
	t_list_node *new_node = (t_list_node *)malloc(sizeof(t_list_node));
	new_node->data = timer_data;
	new_node->next = NULL;

	// if the list_tail is empty, it means that the list is empty
	if (*list_tail == NULL) {
		// Create first element of the list
		(*list_tail) = new_node;
	} else {

		// Add new node to the list
		(*list_tail)->next = new_node;
	}

	printf("New timer node: %d, %s\n", timer_count, timer_name);

	return timer_count;
}

int new_timer(const char * timer_name) {

	int timer_id = -1;

	if (timer_count == 0) {

		// The list is empty so the timer is created
		timer_id = new_timer_node(&list_head, timer_name);
	} else {

		t_list_node *list_iterator = list_head;

		// check if timer already exist
		while (list_iterator->next != NULL) {

			if (strcmp(timer_name, list_iterator->data->name) == 0) {
				timer_id = list_iterator->data->id;
				return timer_id;
			}
			list_iterator = list_iterator->next;
		}

		// check for last element of the list
		if (strcmp(timer_name, list_iterator->data->name) == 0) {
			timer_id = list_iterator->data->id;
			return timer_id;
		}

		// The timer does not exist so it is created
		if (timer_id == -1)
			timer_id = new_timer_node(&list_iterator, timer_name);
	}
	return timer_id;
}