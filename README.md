## DistDir
The library allows the exchange of fields between two arbitrary domain decompositions over the same grid.

The user needs to provide the global indices belonging to each MPI process in its 2D subdomain. Internally, the library uses a distributed directory algorithm to figure out the exchange patterns. This can then be extended to a 3D field providing the number of levels.

Features:
 - 2D fields exchange
 - 3D fields exchange over a 2D decomposition
 - support for multigrid meshes

Applications: exchange between concurrent components operating on the same grid but using a different number of processes.
