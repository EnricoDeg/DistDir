!
! @file example_basic5.f90
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

PROGRAM example_basic5
	USE mpi
	USE distdir_mod
	USE, INTRINSIC :: ISO_C_BINDING, ONLY: c_loc

	IMPLICIT NONE

	INTEGER, ALLOCATABLE, DIMENSION(:) :: src_list, dst_list
	INTEGER, ALLOCATABLE, TARGET, DIMENSION(:) :: src_data, dst_data
	INTEGER :: i, j
	TYPE(t_idxlist)   :: src_idxlist, dst_idxlist
	TYPE(t_map)       :: map
	TYPE(t_exchanger) :: exchanger
	INTEGER :: error, world_rank, world_size, world_role
	INTEGER :: npoints_local, ncols_local, nrows_local

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

	npoints_local = NCOLS * NROWS / (world_size)

	! index list with global indices
	ncols_local = NCOLS / (world_size);
	ALLOCATE(src_list(npoints_local))
	DO i = 1, NROWS
		DO j = 1, ncols_local
			src_list(j + (i - 1) * ncols_local) = (j - 1) + (i - 1) * NCOLS + world_rank * (NCOLS - ncols_local);
		END DO
	END DO

	nrows_local = NROWS / (world_size);
	ALLOCATE(dst_list(npoints_local))
	DO i = 1, nrows_local
		DO j = 1, NCOLS
			dst_list(j + (i - 1) * NCOLS) = (j - 1) + (i - 1) * NCOLS + world_rank * (NROWS - nrows_local) * NCOLS;
		END DO
	END DO

	CALL new_idxlist(src_idxlist, src_list, npoints_local)
	CALL new_idxlist(dst_idxlist, dst_list, npoints_local)

	CALL new_map(map, src_idxlist, dst_idxlist, comm)

	ALLOCATE(src_data(npoints_local))
	ALLOCATE(dst_data(npoints_local))

	! MPI ranks fill src_data array
	DO i = 1, npoints_local
		src_data(i) = (i - 1) + npoints_local * world_rank;
	END DO

	write(*,*) world_rank, ': ', src_data(:)

	CALL new_exchanger(exchanger, map, type, hw)

	CALL exchanger_go(exchanger, C_LOC(src_data(1)), C_LOC(dst_data(1)))

	write(*,*) world_rank, ': ', dst_data(:)

	CALL delete_exchanger(exchanger)
	CALL delete_map(map)
	CALL delete_idxlist(src_idxlist)
	CALL delete_idxlist(dst_idxlist)

	DEALLOCATE(src_list)
	DEALLOCATE(dst_list)
	DEALLOCATE(src_data)
	DEALLOCATE(dst_data)
	CALL distdir_finalize()

END PROGRAM example_basic5