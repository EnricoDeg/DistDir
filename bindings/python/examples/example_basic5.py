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

exchanger = pydistdir.exchanger(map)

exchanger.go(src_data, rcv_data)

print('rank '+str(rank)+': '+str(src_data))
print('rank '+str(rank)+': '+str(rcv_data))