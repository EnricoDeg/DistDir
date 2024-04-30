#
# @file pydistdir.pyx
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
from mpi4py import MPI
from enum import IntEnum

cdef extern from "Python.h":
	int Py_AtExit(void (*)())

cdef extern from "distdir.h":

	ctypedef enum distdir_hardware:
		CPU = 0,
		GPU_NVIDIA = 1,
		GPU_AMD = 2

	ctypedef enum distdir_exchanger:
		IsendIrecv = 0,
		IsendRecv1 = 1,
		IsendRecv2 = 2,
		IsendIrecvNoWait = 3,
		IsendRecv1NoWait = 4,
		IsendRecv2NoWait = 5

	ctypedef enum distdir_verbose:
		verbose_true = 0,
		verbose_false = 1

	void distdir_initialize()
	void distdir_finalize()
	void set_config_hardware(int hardware_type)
	void set_config_exchanger(int exchanger_type)
	void set_config_verbose(int verbose_type)

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
		t_map_exch *exch_send
		t_map_exch *exch_recv
	
	t_map * new_map(t_idxlist *src_idxlist, t_idxlist *dst_idxlist, int stride, libmpi.MPI_Comm comm);
	t_map * extend_map_3d(t_map *map2d, int nlevels);
	void delete_map(t_map *map);

	ctypedef struct t_exchange_per_rank:
		int buffer_size
		void *buffer

	ctypedef struct t_exchange:
		int count
		t_exchange_per_rank **exch

	ctypedef struct t_mpi_exchange:
		libmpi.MPI_Datatype type
		libmpi.MPI_Aint type_size
		libmpi.MPI_Request *req
		libmpi.MPI_Status *stat
		int nreq_send
		int nreq_recv

	ctypedef void (*kernel_func_pack) (void*, void*, int*, int)

	ctypedef void* (*kernel_func_alloc) (size_t)

	ctypedef struct t_kernels:
		kernel_func_pack pack
		kernel_func_pack unpack
		kernel_func_alloc allocator

	ctypedef void (*kernel_func_wait) (t_mpi_exchange*)

	ctypedef struct t_wait:
		kernel_func_wait pre_wait
		kernel_func_wait post_wait

	ctypedef void (*kernel_func_go) (t_exchange *, t_exchange*,  t_map*, t_kernels*, t_mpi_exchange*, t_wait*, void*, void *)

	ctypedef struct t_messages:
		kernel_func_go send_recv

	struct t_exchanger:
		t_exchange *exch_send
		t_exchange *exch_recv
		t_kernels* vtable
		t_messages* vtable_messages
		t_wait* vtable_wait
		t_map *map
		t_mpi_exchange *mpi_exchange

	t_exchanger* new_exchanger(t_map *map, libmpi.MPI_Datatype type)
	void exchanger_go(t_exchanger* exchanger, void *src_data, void* dst_data)
	void delete_exchanger(t_exchanger * exchanger)

class pydistdir_verbose(IntEnum):
	verbose_true = 0
	verbose_false = 1

cdef class distdir:
	def __init__(self):
		distdir_initialize()

	def __del__(self):
		pass

	def hardware(self, hardware_type):
		set_config_hardware(hardware_type)

	def exchanger(self, exchanger_type):
		set_config_exchanger(exchanger_type)

	def verbose(self, verbose_type):
		set_config_verbose(verbose_type)

if Py_AtExit(distdir_finalize) < 0:
	print(
		b"WARNING: %s\n",
		b"could not register distdir_finalize with Py_AtExit()",
	)


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

	def __init__(self, src_idxlist=None, dst_idxlist=None, stride=None, MPI.Comm comm=None,
	                   map2d=None, nlevels=None):
		cdef int stride_val;
		if src_idxlist is not None and dst_idxlist is not None and comm is not None and stride is not None:
			stride_val = _np.array(stride, dtype=_np.int32)
			self._map = new_map( (<idxlist?>src_idxlist)._idxlist , (<idxlist?>dst_idxlist)._idxlist, stride_val, comm.ob_mpi)
		elif map is not None and nlevels is not None:
			self._map = extend_map_3d((<map?>map2d)._map, nlevels)

	def __del__(self):
		self.cleanup()

	def cleanup(self):
		delete_map((<map?>self)._map)


cdef class exchanger:
	cdef t_exchanger *_exchanger

	def __init__(self, p_map):
		cdef MPI.Datatype type = MPI.DOUBLE
		self._exchanger = new_exchanger((<map?>p_map)._map, type.ob_mpi)

	def __del__(self):
		self.cleanup()

	def cleanup(self):
		delete_exchanger((<exchanger?>self)._exchanger)

	def go(self, src_data, dst_data):
		cdef double[::1] src_data_view = _np.ascontiguousarray(src_data, dtype=_np.double)
		cdef double[::1] dst_data_view = _np.ascontiguousarray(dst_data, dtype=_np.double)
		exchanger_go((<exchanger?>self)._exchanger, <void*> &src_data_view[0], <void*> &dst_data_view[0])