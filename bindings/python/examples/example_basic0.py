import pydistdir
import numpy as np
from mpi4py import MPI

dd = pydistdir.distdir()

dd.verbose(pydistdir.pydistdir_verbose.verbose_true)

size = MPI.COMM_WORLD.Get_size()
rank = MPI.COMM_WORLD.Get_rank()

if size != 2:
  raise Exception("Sorry, number of processes must be 2")

if rank == 0:
	src_idxlist = pydistdir.idxlist([1, 2, 3, 4])
	dst_idxlist = pydistdir.idxlist()
	a = np.array([1,4,3,2], dtype=np.double)
else:
	src_idxlist = pydistdir.idxlist()
	dst_idxlist = pydistdir.idxlist([1, 2, 3, 4])
	a = np.zeros(shape=(4), dtype=np.double)

print('rank '+str(rank)+': '+str(a))

map = pydistdir.map(src_idxlist=src_idxlist, dst_idxlist=dst_idxlist, comm=MPI.COMM_WORLD)

exchanger = pydistdir.exchanger(map)

exchanger.go(a, a)

print('rank '+str(rank)+': '+str(a))
