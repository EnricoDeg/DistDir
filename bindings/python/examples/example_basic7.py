#
# @file example_basic7.py
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

import pydistdir
import numpy as np
from mpi4py import MPI

dd = pydistdir.distdir()

rank = MPI.COMM_WORLD.Get_rank()
size = MPI.COMM_WORLD.Get_size()

if size != 8:
  raise Exception("Sorry, number of processes must be 4")

# dummy way but easy to understand for the users
if rank < 4:
	if rank == 0:
		src_idxlist = pydistdir.idxlist([0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 21, 24, 25, 28, 29])
		data = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15], dtype = np.double)
	elif rank == 1:
		src_idxlist = pydistdir.idxlist([2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23, 26, 27, 30, 31])
		data = np.array([16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31], dtype = np.double)
	elif rank == 2:
		src_idxlist = pydistdir.idxlist([32, 33, 36, 37, 40, 41, 44, 45, 48, 49,
		                                 52, 53, 56, 57, 60, 61, 64, 65, 68, 69, 72, 73, 76, 77])
		data = np.array([32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
		                 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55], dtype = np.double)
	elif rank == 3:
		src_idxlist = pydistdir.idxlist([34, 35, 38, 39, 42, 43, 46, 47, 50, 51,
		                                 54, 55, 58, 59, 62, 63, 66, 67, 70, 71, 74, 75, 78, 79])
		data = np.array([56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
		                 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79], dtype = np.double)
	dst_idxlist = pydistdir.idxlist()
else:
	if rank == 4:
		dst_idxlist = pydistdir.idxlist([0, 1, 2, 3, 4, 5, 6, 7])
		data = np.zeros(shape=(8), dtype = np.double)
	elif rank == 5:
		dst_idxlist = pydistdir.idxlist([8, 9, 10, 11, 12, 13, 14, 15])
		data = np.zeros(shape=(8), dtype = np.double)
	elif rank == 6:
		dst_idxlist = pydistdir.idxlist([16, 17, 18, 19, 20, 21, 22, 23, 32, 33, 34, 35, 36, 37, 38, 39,
		                                 48, 49, 50, 51, 52, 53, 54, 55, 64, 65, 66, 67, 68, 69, 70, 71])
		data = np.zeros(shape=(32), dtype = np.double)
	elif rank == 7:
		dst_idxlist = pydistdir.idxlist([24, 25, 26, 27, 28, 29, 30, 31, 40, 41, 42, 43, 44, 45, 46, 47,
		                                 56, 57, 58, 59, 60, 61, 62, 63, 72, 73, 74, 75, 76, 77, 78, 79])
		data = np.zeros(shape=(32), dtype = np.double)
	src_idxlist = pydistdir.idxlist()

map = pydistdir.map(src_idxlist=src_idxlist, dst_idxlist=dst_idxlist, comm=MPI.COMM_WORLD)

hw = pydistdir.pydistdir_hardware.CPU
exchanger = pydistdir.exchanger(map, hw)

exchanger.go(data, data)

print('rank '+str(rank)+': '+str(data))