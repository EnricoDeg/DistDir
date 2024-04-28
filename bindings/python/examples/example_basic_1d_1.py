import pydistdir
import numpy as np
from mpi4py import MPI

dd = pydistdir.distdir()

dd.verbose(pydistdir.pydistdir_verbose.verbose_true)

size = MPI.COMM_WORLD.Get_size()
rank = MPI.COMM_WORLD.Get_rank()

array = [1, 2, 3, 4]

if rank == 0:
	src_idxlist = pydistdir.idxlist(array)
	dst_idxlist = pydistdir.idxlist()
	a = np.array([1,4,3,2], dtype=np.double)
else:
	src_idxlist = pydistdir.idxlist()
	dst_idxlist = pydistdir.idxlist(array)
	a = np.zeros(shape=(4), dtype=np.double)

if rank == 0:
	print('rank '+str(rank)+': '+str(a))
else:
	print('rank '+str(rank)+': '+str(a))

map = pydistdir.map(src_idxlist, dst_idxlist, -1, MPI.COMM_WORLD)

exchanger = pydistdir.exchanger(map)

exchanger.go(a, a)

if rank == 0:
	print('rank '+str(rank)+': '+str(a))
else:
	print('rank '+str(rank)+': '+str(a))
