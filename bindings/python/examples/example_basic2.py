import pydistdir
import numpy as np
from mpi4py import MPI

dd = pydistdir.distdir()

rank = MPI.COMM_WORLD.Get_rank()
size = MPI.COMM_WORLD.Get_size()

if size != 4:
  raise Exception("Sorry, number of processes must be 4")

# dummy way but easy to understand for the users
if rank < 2:
	if rank == 0:
		src_idxlist = pydistdir.idxlist([0, 2, 4, 6, 8, 10, 12, 14])
		data = np.array([0, 1, 2, 3, 4, 5, 6, 7], dtype = np.double)
	else:
		src_idxlist = pydistdir.idxlist([1, 3, 5, 7, 9, 11, 13, 15])
		data = np.array([8, 9, 10, 11, 12, 13, 14, 15], dtype = np.double)
	dst_idxlist = pydistdir.idxlist()
else:
	if rank == 2:
		dst_idxlist = pydistdir.idxlist([0, 1, 2, 3, 4, 5, 6, 7])
		data = np.zeros(shape=(8), dtype = np.double)
	else:
		dst_idxlist = pydistdir.idxlist([8, 9, 10, 11, 12, 13, 14, 15])
		data = np.zeros(shape=(8), dtype = np.double)
	src_idxlist = pydistdir.idxlist()

map = pydistdir.map(src_idxlist=src_idxlist, dst_idxlist=dst_idxlist, comm=MPI.COMM_WORLD)

hw = pydistdir.pydistdir_hardware.CPU
exchanger = pydistdir.exchanger(map, hw)

exchanger.go(data, data)

print('rank '+str(rank)+': '+str(data))