/*
 * @file timer.h
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

#ifndef TIMER_H
#define TIMER_H

#include "mpi.h"

#define STRING_MAX 1024

/** @struct t_timer_data
 * 
 *  @brief The structure contains information about a single timer
 * 
 */
struct t_timer_data {

	/** @brief timer ID */
	int id;
	/** @brief timer name */
	char* name;
	/** @brief timer total time */
	double total_time;
	/** @brief timer start time */
	double start_time;
};
typedef struct t_timer_data t_timer_data;

/** @struct t_list_node
 * 
 *  @brief The structure contains a linked list single node for the timer module
 * 
 */
struct t_list_node
{

	/** @brief pointer to t_timer_data holding single timer information */
	t_timer_data *data;
	/** @brief pointer to next node of the linked list */
	struct t_list_node *next;
};
typedef struct t_list_node t_list_node;

/**
 * @brief Create a new timer
 * 
 * @details A timer is created and added to the internal linked list,
 *          then the timer ID is returned.
 *          If the timer already exists, its ID is returned.
 * 
 * @param[in] timer_name timer name string
 * 
 * @return timer ID
 * 
 * @ingroup timer
 */
int new_timer(const char * timer_name) ;

/**
 * @brief Start timer based on ID.
 * 
 * @details The timer is started using internally MPI_Wtime()
 * 
 * @param[in] timer_name timer name string
 * 
 * @ingroup timer
 */
void timer_start(int timer_id) ;

/**
 * @brief Stop timer based on ID.
 * 
 * @details The timer is stopped using internally MPI_Wtime() and 
 *          adding the time to the total timer time.
 *          If timer_start() was not called before, it fails.
 * 
 * @param[in] timer_name timer name string
 * 
 * @ingroup timer
 */
void timer_stop(int timer_id) ;

/**
 * @brief Create reports of all timers.
 * 
 * @details The reports are stored in a specific directory and 
 *          a report is created by each MPI rank.
 *          A summary report is also created with min, max and mean 
 *          timers' times.
 * 
 * @ingroup timer
 */
void timers_report() ;

/**
 * @brief Reset all the timers.
 * 
 * @details The total time of all the timers is reset to zero.
 * 
 * @ingroup timer
 */
void timers_reset() ;

/**
 * @brief Free timers memory.
 * 
 * @details Free each timer memory and the memory related to the linked 
 *          list.
 * 
 * @ingroup timer
 */
void delete_timers() ;

#endif
