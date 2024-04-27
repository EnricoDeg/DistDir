/*
 * @file setting.c
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
#include <stdio.h>
#include "setting.h"
#include "check.h"

static t_config *config;

static int get_env_variable(const char* variable_name) {

	const char *variable_string = getenv(variable_name);

	if (variable_string == NULL)
		return -1;

	if (variable_string[0] != '\0') {
		return atoi(variable_string);
	}
	else
		return -1;
}

static void set_default_config(t_config *config) {

	config->hardware = CPU;
	config->exchanger = IsendIrecv;
	config->verbose = verbose_false;
}

static void print_config() {

	printf("DISTDIR_HARDWARE  = %d\n", config->hardware );
	printf("DISTDIR_EXCHANGER = %d\n", config->exchanger);
	printf("DISTDIR_VERBOSE   = %d\n", config->verbose  );
}

void set_config_hardware(int hardware_type) {

	config->hardware = hardware_type;
}

void set_config_exchanger(int exchanger_type) {

	config->exchanger = exchanger_type;
}

void set_config_verbose(int verbose_type) {

	config->verbose = verbose_type;
}

int get_config_hardware() {

	return config->hardware;
}

int get_config_exchanger() {

	return config->exchanger;
}

int get_verbose_type() {

	return config->verbose;
}

void distdir_initialize() {

	int mpi_initialized;
	check_mpi( MPI_Initialized( &mpi_initialized ) );
	if (!mpi_initialized) {
		MPI_Init(NULL, NULL);
		check_mpi( MPI_Initialized( &mpi_initialized ) );
	}

	config = (t_config *)malloc(sizeof(t_config));
	config->initialized = mpi_initialized;
	set_default_config(config);

	// set hardware type from env variable
	{
		int variable = get_env_variable("DISTDIR_HARDWARE");
		if (variable != -1) config->hardware = variable;
	}

	// set exchanger type from env variable
	{
		int variable = get_env_variable("DISTDIR_EXCHANGER");
		if (variable != -1) config->exchanger = variable;
	}

	// set verbose from env variable
	{
		int variable = get_env_variable("DISTDIR_VERBOSE");
		if (variable != -1) config->verbose = variable;
	}

	if (config->verbose == verbose_true) print_config();
}

void distdir_finalize() {

	int mpi_finalized;
	int mpi_initialized = config->initialized;
	check_mpi( MPI_Finalized( &mpi_finalized ) );

	free(config);

	if (mpi_initialized && !mpi_finalized)
		check_mpi(MPI_Finalize());
}