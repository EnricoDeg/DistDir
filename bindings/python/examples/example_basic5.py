#
# @file example_basic5.py
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

if size != 2:
  raise Exception("Sorry, number of processes must be 4")

# dummy way but easy to understand for the users
if rank == 0:
	src_idxlist = pydistdir.idxlist([0, 1, 4, 5, 8, 9, 12, 13])
	dst_idxlist = pydistdir.idxlist([0, 1, 2, 3, 4, 5, 6, 7])
	src_data = np.array([0, 1, 2, 3, 4, 5, 6, 7], dtype = np.double)
	rcv_data = np.zeros(shape=(8), dtype = np.double)
else:
	src_idxlist = pydistdir.idxlist([2, 3, 6, 7, 10, 11, 14, 15])
	dst_idxlist = pydistdir.idxlist([8, 9, 10, 11, 12, 13, 14, 15])
	src_data = np.array([8, 9, 10, 11, 12, 13, 14, 15], dtype = np.double)
	rcv_data = np.zeros(shape=(8), dtype = np.double)

map = pydistdir.map(src_idxlist=src_idxlist, dst_idxlist=dst_idxlist, comm=MPI.COMM_WORLD)

hw = pydistdir.pydistdir_hardware.CPU
exchanger = pydistdir.exchanger(map, hw)

exchanger.go(src_data, rcv_data)

print('rank '+str(rank)+': '+str(src_data))
print('rank '+str(rank)+': '+str(rcv_data))