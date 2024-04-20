import distdir
import numpy as np
from mpi4py import MPI


size = MPI.COMM_WORLD.Get_size()
rank = MPI.COMM_WORLD.Get_rank()

array = [1, 2, 3, 4]

if rank == 0:
	src_idxlist = distdir.idxlist(array)
	dst_idxlist = distdir.idxlist()
	a = np.array([1,4,3,2], dtype=np.double)
else:
	src_idxlist = distdir.idxlist()
	dst_idxlist = distdir.idxlist(array)
	a = np.zeros(shape=(4), dtype=np.double)

if rank == 0:
	print('rank '+str(rank)+': '+str(a))
else:
	print('rank '+str(rank)+': '+str(a))

map = distdir.map(src_idxlist, dst_idxlist, MPI.COMM_WORLD)

exchanger = distdir.exchange()

exchanger.go(map, a, a)

if rank == 0:
	print('rank '+str(rank)+': '+str(a))
else:
	print('rank '+str(rank)+': '+str(a))
