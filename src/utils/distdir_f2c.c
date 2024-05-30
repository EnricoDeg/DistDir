/*
 * @file distdir_f2c.c
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

#include <mpi.h>

#include "src/setup/setting.h"
#include "src/setup/group.h"
#include "src/core/indices/idxlist.h"
#include "src/core/algorithm/map.h"
#include "src/core/exchange/exchange.h"

struct t_idxlist_f {
	t_idxlist *cptr;
};

struct t_map_f {
	t_map *cptr;
};

struct t_exchanger_f {
	t_exchanger *cptr;
};

t_idxlist * t_idxlist_f2c(struct t_idxlist_f *p)
{
	return p->cptr;
}

t_map * t_map_f2c(struct t_map_f *p)
{
	return p->cptr;
}

t_exchanger * t_exchanger_f2c(struct t_exchanger_f *p)
{
	return p->cptr;
}

t_map * new_map_f(struct t_idxlist_f *src_idxlist_f,
                  struct t_idxlist_f *dst_idxlist_f,
                  int stride,
                  MPI_Fint comm_f)
{
	MPI_Comm comm_c = MPI_Comm_f2c(comm_f);
	return new_map(src_idxlist_f->cptr, dst_idxlist_f->cptr, stride, comm_c);
}

t_exchanger* new_exchanger_f(struct t_map_f *map    ,
                             MPI_Fint        type_f ,
                             int             hw     ) {

	MPI_Datatype type_c = MPI_Type_f2c(type_f);
	return new_exchanger(map->cptr, type_c, hw);
}