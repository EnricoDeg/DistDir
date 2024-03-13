#ifndef GROUP_H
#define GROUP_H

#include "mpi.h"

void new_group(MPI_Comm *new_comm, MPI_Comm work_comm, int id);

#endif