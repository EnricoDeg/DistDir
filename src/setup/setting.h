/*
 * @file setting.h
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

#ifndef SETTING_H
#define SETTING_H

/** @enum distdir_hardware
 * 
 *  @brief Enum for supported hardware
 * 
 */
typedef enum {
	CPU = 0,
	GPU_NVIDIA = 1,
	GPU_AMD = 2
} distdir_hardware ;

/** @enum distdir_exchanger
 * 
 *  @brief Enum for supported exchanger
 * 
 */
enum distdir_exchanger {
	IsendIrecv1 = 0,
	IsendIrecv2 = 1,
	IsendRecv1 = 2,
	IsendRecv2 = 3,
	IsendIrecv1NoWait = 4,
	IsendIrecv2NoWait = 5,
	IsendRecv1NoWait = 6,
	IsendRecv2NoWait = 7
};

/** @enum distdir_verbose
 * 
 *  @brief Enum for verbosity
 * 
 */
enum distdir_verbose {
	verbose_true = 0,
	verbose_false = 1
};

/** @enum distdir_sort
 * 
 *  @brief Enum for supported sorting algorithms
 * 
 */
enum distdir_sort {
	mergesort = 0,
	quicksort = 1,
	timsort   = 2
};

/** @struct t_config
 * 
 *  @brief The structure contains information about the library configuration
 * 
 */
struct t_config {
	/** @brief define initialization state of the library */
	int initialized;
	/** @brief exchanger type */
	enum distdir_exchanger exchanger;
	/** @brief verbose type */
	enum distdir_verbose verbose;
	/** @brief sort type */
	enum distdir_sort sort;
};
typedef struct t_config t_config;

/**
 * @brief Initialize the library
 * 
 * @details Initialize MPI if not already done and the library configuration
 * 
 * @ingroup setting
 */
void distdir_initialize();

/**
 * @brief Finalize the library
 * 
 * @details Finalize MPI and the library configuration
 * 
 * @ingroup setting
 */
void distdir_finalize();

/**
 * @brief Set library exchanger type
 * 
 * @details It can also be set up with environment variable \c DISTDIR_EXCHANGER.
 *          The function should be called before a call to \c new_exchanger.
 * 
 * @param[in] exchanger_type exchanger type using values of distdir_exchanger enum
 * 
 * @ingroup setting
 */
void set_config_exchanger(int exchanger_type);

/**
 * @brief Set library verbosity
 * 
 * @details It can also be set up with environment variable \c DISTDIR_VERBOSE.
 * 
 * @param[in] verbose_type verbose type using values of distdir_verbose enum
 * 
 * @ingroup setting
 */
void set_config_verbose(int verbose_type);

/**
 * @brief Set library sort algorithm
 * 
 * @details It can also be set up with environment variable \c DISTDIR_SORT.
 *          The function should be called before a call to \c new_map.
 * 
 * @param[in] sort_type sort algorithm type using values of distdir_sort enum
 * 
 * @ingroup setting
 */
void set_config_sort(int sort_type);

/**
 * @brief get current exchanger configuration
 * 
 * @details Return a value of the distdir_exchanger enum.
 * 
 * @return value of the distdir_exchanger enum
 * 
 * @ingroup setting
 */
int get_config_exchanger();

/**
 * @brief get current verbose configuration
 * 
 * @details Return a value of the distdir_verbose enum.
 * 
 * @return value of the distdir_verbose enum
 * 
 * @ingroup setting
 */
int get_config_verbose();

/**
 * @brief get current sert algorithm configuration
 * 
 * @details Return a value of the distdir_sort enum.
 * 
 * @return value of the distdir_sort enum
 * 
 * @ingroup setting
 */
int get_config_sort();

#endif