!
! @file example_basic7.f90
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

PROGRAM example_basic7
	USE mpi
	USE distdir_mod
	USE, INTRINSIC :: ISO_C_BINDING, ONLY: c_loc

	IMPLICIT NONE

	INTEGER, ALLOCATABLE, DIMENSION(:) :: list
	INTEGER, ALLOCATABLE, TARGET, DIMENSION(:) :: data
	INTEGER :: i, j, k
	TYPE(t_idxlist)   :: idxlist, idxlist_empty
	TYPE(t_map)       :: map2d, map
	TYPE(t_exchanger) :: exchanger
	INTEGER :: error, world_rank, world_size, world_role
	INTEGER :: npoints_local, ncols_local, nrows_local, total_size, nlevs_local

	INTEGER, PARAMETER :: I_SRC = 0
	INTEGER, PARAMETER :: I_DST = 1
	INTEGER, PARAMETER :: NCOLS = 4
	INTEGER, PARAMETER :: NROWS = 4
	INTEGER, PARAMETER :: NLEVS = 5
	INTEGER, PARAMETER :: comm  = MPI_COMM_WORLD
	INTEGER, PARAMETER :: hw    = DISTDIR_HW_CPU
	INTEGER, PARAMETER :: type  = MPI_INTEGER
	
	CALL distdir_initialize()

	CALL MPI_COMM_SIZE(comm, world_size, error)
	CALL MPI_COMM_RANK(comm, world_rank, error)

	npoints_local = NCOLS * NROWS / (world_size / 4)

	! index list with global indices
	IF (world_rank < 4) THEN

		world_role = I_SRC;
		ncols_local = NCOLS / (world_size / 4);

		IF (world_rank ==0) THEN

			nlevs_local = 2;
			total_size = npoints_local * 2;
			ALLOCATE(list(total_size))
			DO k = 0, 1
				DO i = 0, NROWS-1
					DO j = 0, ncols_local-1
						list(j+1+i*ncols_local+k*npoints_local) = j + i * NCOLS + world_rank * (NCOLS - ncols_local) + &
						                                           k * NCOLS*NROWS;
					END DO
				END DO
			END DO

		ELSE IF (world_rank==1) THEN

			nlevs_local = 2;
			total_size = npoints_local * 2;
			ALLOCATE(list(total_size))
			DO k = 0, 1
				DO i = 0, NROWS-1
					DO j = 0, ncols_local-1
						list(j+1+i*ncols_local+k*npoints_local) = j + i * NCOLS + world_rank * (NCOLS - ncols_local) + &
						                                          k * NCOLS*NROWS;
					END DO
				END DO
			END DO

		ELSE IF (world_rank==2) THEN

			nlevs_local = 3;
			total_size = npoints_local * 3;
			ALLOCATE(list(total_size))
			DO k = 2,NLEVS-1
				DO i = 0, NROWS-1
					DO j = 0, ncols_local-1
						list(j+1+i*ncols_local+(k-2)*npoints_local) = j + i * NCOLS + (world_rank-2) * (NCOLS - ncols_local) + &
						                                              k * NCOLS*NROWS;
					END DO
				END DO
			END DO

		ELSE

			nlevs_local = 3;
			total_size = npoints_local * 3;
			ALLOCATE(list(total_size))
			DO k = 2,NLEVS-1
				DO i = 0, NROWS-1
					DO j = 0, ncols_local-1
						list(j+1+i*ncols_local+(k-2)*npoints_local) = j + i * NCOLS + (world_rank-2) * (NCOLS - ncols_local) + &
						                                              k * NCOLS*NROWS;
					END DO
				END DO
			END DO

		END IF

	ELSE

		world_role = I_DST
		nrows_local = NROWS / (world_size / 4);
		IF (world_rank==4) THEN
			nlevs_local = 1;
			total_size = npoints_local;
			ALLOCATE(list(total_size))
			DO i = 0, nrows_local-1
				DO j = 0, NCOLS-1
					list(j+1+i*NCOLS) = j + i * NCOLS;
				END DO
			END DO

		ELSE IF (world_rank==5) THEN

			nlevs_local = 1;
			total_size = npoints_local;
			ALLOCATE(list(total_size))

			DO i = 0, nrows_local-1
				DO j = 0, NCOLS-1
					list(j+1+i*NCOLS) =  j + i * NCOLS + &
						                (world_rank - (world_size / 2) ) * (NROWS - nrows_local) * NCOLS;
				END DO
			END DO

		ELSE IF (world_rank==6) THEN

			nlevs_local = 4;
			total_size = npoints_local * 4;
			ALLOCATE(list(total_size))

			DO k = 1,NLEVS-1
				DO i = 0, nrows_local-1
					DO j = 0, NCOLS-1
						list(j+1+i*NCOLS+(k-1)*npoints_local) = j + i * NCOLS + k * NROWS * NCOLS;
					END DO
				END DO
			END DO

		ELSE

			nlevs_local = 4;
			total_size = npoints_local * 4;
			ALLOCATE(list(total_size))

			DO k = 1,NLEVS-1
				DO i = 0, nrows_local-1
					DO j = 0, NCOLS-1
						list(j+1+i*NCOLS+(k-1)*npoints_local) = j + i * NCOLS + &
						                                        (world_rank - 6) * (NROWS - nrows_local) * NCOLS + &
						                                        k * NROWS * NCOLS;
					END DO
				END DO
			END DO

		END IF

	END IF

	CALL new_idxlist(idxlist, list, total_size)
	CALL new_idxlist(idxlist_empty)

	IF (world_role == I_SRC) THEN
		CALL new_map(map, idxlist, idxlist_empty, NCOLS*NROWS, comm)
	ELSE
		CALL new_map(map, idxlist_empty, idxlist, NCOLS*NROWS, comm)
	END IF

	ALLOCATE(data(total_size))
	IF (world_role == I_SRC) THEN

		IF (world_rank==0) THEN

			DO k = 0, 1
				DO i = 0, npoints_local-1
					data(i+1+k*npoints_local) = i + k * npoints_local;
				END DO
			END DO

		ELSE IF (world_rank == 1) THEN

			DO k = 0, 1
				DO i = 0, npoints_local-1
					data(i+1+k*npoints_local) = i + k * npoints_local + npoints_local * 2 * world_rank;
				END DO
			END DO

		ELSE IF (world_rank == 2) THEN

			DO k = 2, NLEVS-1
				DO i = 0, npoints_local-1
					data(i+1+(k-2)*npoints_local) = i + (k-2) * npoints_local + &
						                            NROWS * NCOLS * 2;
				END DO
			END DO

		ELSE

			DO k = 2, NLEVS-1
				DO i = 0, npoints_local-1
					data(i+1+(k-2)*npoints_local) = i + NROWS * NCOLS * 2 + (k-2) * npoints_local + &
						                            npoints_local * 3 * (world_rank-2);
				END DO
			END DO

		END IF

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

END PROGRAM example_basic7