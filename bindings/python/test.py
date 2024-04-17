import distdir
import numpy as np
from mpi4py import MPI


size = MPI.COMM_WORLD.Get_size()
rank = MPI.COMM_WORLD.Get_rank()

array = [1, 2, 3, 4]

if rank == 0:
	src_idxlist = distdir.idxlist(array)
	dst_idxlist = distdir.idxlist()
else:
	src_idxlist = distdir.idxlist()
	dst_idxlist = distdir.idxlist(array)

map = distdir.map(src_idxlist, dst_idxlist, MPI.COMM_WORLD)