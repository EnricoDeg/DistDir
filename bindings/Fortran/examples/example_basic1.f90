!
! @file example_basic1.f90
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

PROGRAM example_basic1
	USE mpi
	USE distdir_mod

	IMPLICIT NONE

	INTEGER, ALLOCATABLE, DIMENSION(:) :: list
	INTEGER :: i, j
	TYPE(t_idxlist) :: idxlist
	INTEGER :: error, world_rank, world_size, world_role
	INTEGER :: npoints_local, ncols_local, nrows_local


	INTEGER, PARAMETER :: I_SRC = 0
	INTEGER, PARAMETER :: I_DST = 1
	INTEGER, PARAMETER :: NCOLS = 4
	INTEGER, PARAMETER :: NROWS = 4
	
	CALL distdir_initialize()

	CALL MPI_COMM_SIZE(MPI_COMM_WORLD, world_size, error)
	CALL MPI_COMM_RANK(MPI_COMM_WORLD, world_rank, error)

	npoints_local = NCOLS * NROWS / (world_size / 2)

	ALLOCATE(list(npoints_local))

	! index list with global indices
	IF (world_rank < 2) THEN
		world_role = I_SRC
		ncols_local = NCOLS / (world_size / 2)
		DO i = 0, NROWS - 1
			DO j = 0, ncols_local - 1
				list((j + 1) + i * ncols_local) = j + i * NCOLS + world_rank * (NCOLS - ncols_local)
			END DO
		END DO
	ELSE
		world_role = I_DST
		nrows_local = NROWS / (world_size / 2)
		DO i = 0, nrows_local - 1
			DO j = 0, NCOLS - 1
				list((j + 1) + i * NCOLS) = j + i * NCOLS + (world_rank - (world_size / 2)) * &
				                                    (NROWS - nrows_local) * NCOLS
			END DO
		END DO
	END IF

	CALL new_idxlist(idxlist, list, npoints_local)

	CALL delete_idxlist(idxlist)

	DEALLOCATE(list)
	
	CALL distdir_finalize()

END PROGRAM example_basic1