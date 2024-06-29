!
! @file example_multi_grid1.f90
!
! @copyright Copyright (C) 2024 Enrico Degregori <enrico.degregori@gmail.com>
!
! @author Enrico Degregori <enrico.degregori@gmail.com>
!
! Redistribution and use in source and binary forms, with or without
! modification, are permitted provided that the following conditions are met:
!
! 1. Redistributions of source code must retain the above copyright notice, this
!    list of conditions and the following disclaimer.
!
! 2. Redistributions in binary form must reproduce the above copyright notice,
!    this list of conditions and the following disclaimer in the documentation
!    and/or other materials provided with the distribution.
!
! 3. Neither the name of the copyright holder nor the names of its
!    contributors may be used to endorse or promote products derived from
!    this software without specific prior written permission.
! 
! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
! DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
! FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
! DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
! SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
! CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
! OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
! OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
!

PROGRAM example_multi_grid1
	USE mpi
	USE distdir_mod
	USE, INTRINSIC :: ISO_C_BINDING, ONLY: c_loc

	IMPLICIT NONE

	INTEGER            :: block_comm, id
	INTEGER            :: block_size, block_rank
	INTEGER            :: i, j
	TYPE(t_idxlist)    :: idxlist, idxlist_empty, idxlist_global
	TYPE(t_map)        :: map
	TYPE(t_exchanger)  :: exchanger
	INTEGER            :: error, world_rank, world_size, world_role
	INTEGER            :: npoints_local, ncols_local, nrows_local
	INTEGER, ALLOCATABLE, DIMENSION(:) :: list, list_global
	INTEGER, ALLOCATABLE, TARGET, DIMENSION(:) :: data, data_global

	INTEGER, PARAMETER :: I_SRC = 0
	INTEGER, PARAMETER :: I_DST = 1
	INTEGER, PARAMETER :: NCOLS = 4
	INTEGER, PARAMETER :: NROWS = 4
	INTEGER, PARAMETER :: comm  = MPI_COMM_WORLD
	INTEGER, PARAMETER :: hw    = DISTDIR_HW_CPU
	INTEGER, PARAMETER :: type  = MPI_INTEGER

	CALL distdir_initialize()

	CALL MPI_COMM_SIZE(comm, world_size, error)
	CALL MPI_COMM_RANK(comm, world_rank, error)

	IF (world_rank < 4) THEN
		id = 0
	ELSE
		id = 1
	END IF

	CALL new_group(block_comm, comm, id)
	CALL MPI_COMM_SIZE(block_comm, block_size, error)
	CALL MPI_COMM_RANK(block_comm, block_rank, error)

	npoints_local = 4;
	ALLOCATE(list(npoints_local))
	IF (block_rank == 0) ALLOCATE(list_global(NROWS*NCOLS))

	IF (block_rank == 0) THEN
		list(1) = 0;
		list(2) = 1;
		list(3) = 4;
		list(4) = 5;
	ELSE IF (block_rank == 1) THEN
		list(1) = 2;
		list(2) = 3;
		list(3) = 6;
		list(4) = 7;
	ELSE IF (block_rank == 2) THEN
		list(1) = 8;
		list(2) = 9;
		list(3) = 12;
		list(4) = 13;
	ELSE IF (block_rank == 3) THEN
		list(1) = 10;
		list(2) = 11;
		list(3) = 14;
		list(4) = 15;
	END IF

	IF (block_rank == 0) THEN
		DO i = 0, NROWS*NCOLS-1
			list_global(i+1) = i
		END DO
	END IF

	CALL new_idxlist(idxlist, list, npoints_local)
	IF (block_rank == 0) CALL new_idxlist(idxlist_global, list_global, NROWS*NCOLS)
	CALL new_idxlist(idxlist_empty)

	IF (block_rank > 0) THEN
		CALL new_map(map, idxlist, idxlist_empty, block_comm)
	ELSE
		CALL new_map(map, idxlist, idxlist_global, block_comm)
	END IF

	ALLOCATE(data(npoints_local))

	DO i = 0, npoints_local-1
		data(i+1) = i + npoints_local * world_rank;
	END DO

	IF (block_rank == 0) ALLOCATE(data_global(NROWS*NCOLS))

	CALL new_exchanger(exchanger, map, type, hw)

	CALL exchanger_go(exchanger, C_LOC(data(1)), C_LOC(data_global(1)))

	write(*,*) world_rank, ': ', data(:)
	IF (block_rank == 0) write(*,*) world_rank, ': ', data_global(:)

	CALL delete_exchanger(exchanger)
	CALL delete_map(map)
	CALL delete_idxlist(idxlist_empty)
	CALL delete_idxlist(idxlist)
	IF (block_rank == 0) CALL delete_idxlist(idxlist_global)

	DEALLOCATE(data)
	IF (block_rank == 0) DEALLOCATE(data_global)

	DEALLOCATE(list)
	IF (block_rank == 0) DEALLOCATE(list_global)

	CALL distdir_finalize()

END PROGRAM example_multi_grid1