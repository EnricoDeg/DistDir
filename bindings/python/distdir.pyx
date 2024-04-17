#
# @file bucket.h
#
# @copyright Copyright (C) 2024 Enrico Degregori <enrico.degregori@gmail.com>
#
# @author Enrico Degregori <enrico.degregori@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
 
import cython

import numpy as _np
cimport mpi4py.libmpi as libmpi
cimport mpi4py.MPI as MPI

cdef extern from "distdir.h":
	ctypedef struct t_idxlist:
		int  count
		int *list

	t_idxlist * new_idxlist(int *idx_array, int num_indices);
	t_idxlist * new_idxlist_empty();
	void delete_idxlist(t_idxlist *idxlist);

	ctypedef struct t_map_exch_per_rank:
		int exch_rank
		int buffer_size
		int *buffer_idxlist
		void *buffer
	
	ctypedef struct t_map_exch:
		int count
		t_map_exch_per_rank **exch
	
	ctypedef struct t_map:
		libmpi.MPI_Comm comm
		t_map_exch *exch_send;
		t_map_exch *exch_recv;
	
	t_map * new_map(t_idxlist *src_idxlist, t_idxlist *dst_idxlist, libmpi.MPI_Comm comm);
	t_map * extend_map_3d(t_map *map2d, int nlevels);
	void delete_map(t_map *map);

cdef class idxlist:
	cdef t_idxlist *_idxlist

	def __init__(self, array=None):
		cdef int[::1] array_view
		if array is None:
			self._idxlist = new_idxlist_empty()
		else:
			array_view = _np.ascontiguousarray(array, dtype=_np.int32)
			self._idxlist = new_idxlist(&array_view[0], len(array_view))

	def __del__(self):
		self.cleanup()

	def cleanup(self):
		delete_idxlist((<idxlist?>self)._idxlist)

	def size(self):
		return self._idxlist.count

	def list(self):
		# Get a memoryview.
		cdef int[:] array_view = <int[:self.size()]> self._idxlist.list
		# Coercion the memoryview to numpy array. Not working.
		ret = _np.asarray(array_view)
		return ret

cdef class map:
	cdef t_map *_map

	def __init__(self, src_idxlist, dst_idxlist, MPI.Comm comm):
		self._map = new_map( (<idxlist?>src_idxlist)._idxlist , (<idxlist?>dst_idxlist)._idxlist, comm.ob_mpi)