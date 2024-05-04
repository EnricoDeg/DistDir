## DistDir
The library allows the exchange of fields between two arbitrary domain decompositions over the same grid.

The user needs to provide the global indices belonging to each MPI process in its subdomain. Internally, the library uses a distributed directory algorithm to figure out the exchange patterns.

Features:
 - 2D fields exchange
 - 3D fields exchange over a 2D decomposition
 - 3D fields exchange
 - support for multigrid meshes

Applications:
 - exchange between concurrent components operating on the same grid but using a different number of processes
 - transposition for spectral methods

#### Quick start

Dependencies:
 - MPI
 - CMake
 - Doxygen
 - Python

##### Installation
```
export CC=mpicc
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```

##### Testing
```
cd build/tests
ctest
```

##### Documentation
```
cd build
make distdir_docs
firefox docs/html/index.html
```
The full documentation can be found here: https://enricodeg.github.io/DistDir

##### Python bindings
Distdir library can be very easily used in a python script. Remember to set the `PYTHONPATH` environment variable:
```
export PYTHONPATH=${DISTDIR_ROOT}/lib/pydistdir
```
Examples of python scripts using `distdir` library can be found in `bindings/python/examples`

##### Examples
A complete list of use cases can be found in the `examples` folder
