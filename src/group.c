#include "group.h"
#include "check.h"

void new_group(MPI_Comm *new_comm, MPI_Comm work_comm, int id) {
    check_mpi( MPI_Comm_split(work_comm, id, 0, new_comm) );
}