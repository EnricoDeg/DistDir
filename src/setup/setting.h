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

typedef enum {
	CPU = 0,
	GPU_NVIDIA = 1,
	GPU_AMD = 2
} distdir_hardware ;

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

enum distdir_verbose {
	verbose_true = 0,
	verbose_false = 1
};

struct t_config {
	int initialized;
	enum distdir_exchanger exchanger;
	enum distdir_verbose verbose;
};
typedef struct t_config t_config;

void distdir_initialize();

void distdir_finalize();

void set_config_exchanger(int exchanger_type);

void set_config_verbose(int verbose_type);

int get_config_exchanger();

int get_config_verbose();

#endif