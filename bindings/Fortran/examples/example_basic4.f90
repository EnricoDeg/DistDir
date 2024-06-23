!
! @file example_basic4.f90
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

PROGRAM example_basic4
	USE mpi
	USE distdir_mod
	USE, INTRINSIC :: ISO_C_BINDING, ONLY: c_loc

	IMPLICIT NONE

	INTEGER, ALLOCATABLE, DIMENSION(:) :: list
	INTEGER, ALLOCATABLE, TARGET, DIMENSION(:) :: data
	INTEGER :: i
	TYPE(t_idxlist)   :: idxlist, idxlist_empty
	TYPE(t_map)       :: map
	TYPE(t_exchanger) :: exchanger
	INTEGER :: error, world_rank, world_size, world_role
	INTEGER :: npoints_local

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

	! index list with global indices
	IF (world_rank < 4) THEN
		world_role = I_SRC
		npoints_local = 4
		ALLOCATE(list(npoints_local))
		IF (world_rank == 0) THEN
			list(1) = 0;
			list(2) = 1;
			list(3) = 4;
			list(4) = 5;
		ELSE IF (world_rank == 1) THEN
			list(1) = 2;
			list(2) = 3;
			list(3) = 6;
			list(4) = 7;
		ELSE IF (world_rank == 2) THEN
			list(1) = 8;
			list(2) = 9;
			list(3) = 12;
			list(4) = 13;
		ELSE IF (world_rank == 3) THEN
			list(1) = 10;
			list(2) = 11;
			list(3) = 14;
			list(4) = 15;
		END IF
	ELSE
		world_role = I_DST;
		npoints_local = 8;
		ALLOCATE(list(npoints_local))
		IF (world_rank == 4) THEN
			list(1) = 0;
			list(2) = 1;
			list(3) = 4;
			list(4) = 5;
			list(5) = 8;
			list(6) = 9;
			list(7) = 12;
			list(8) = 13;
		ELSE IF (world_rank == 5) THEN
			list(1) = 2;
			list(2) = 3;
			list(3) = 6;
			list(4) = 7;
			list(5) = 10;
			list(6) = 11;
			list(7) = 14;
			list(8) = 15;
		END IF
	END IF

	CALL new_idxlist(idxlist, list, npoints_local)
	CALL new_idxlist(idxlist_empty)

	IF (world_role == I_SRC) THEN
		CALL new_map(map, idxlist, idxlist_empty, comm)
	ELSE
		CALL new_map(map, idxlist_empty, idxlist, comm)
	END IF

	ALLOCATE(data(npoints_local))

	! src MPI ranks fill data array
	IF (world_role == I_SRC) THEN
		DO i = 1,npoints_local
			data(i) = (i - 1) + npoints_local * world_rank;
		END DO
	END IF

	CALL new_exchanger(exchanger, map, type, hw)

	CALL exchanger_go(exchanger, C_LOC(data(1)), C_LOC(data(1)))

	write(*,*) world_rank, ': ', data(:)

	CALL delete_exchanger(exchanger)
	CALL delete_map(map)
	CALL delete_idxlist(idxlist_empty)
	CALL delete_idxlist(idxlist)

	DEALLOCATE(data)
	DEALLOCATE(list)

	CALL distdir_finalize()

END PROGRAM example_basic4