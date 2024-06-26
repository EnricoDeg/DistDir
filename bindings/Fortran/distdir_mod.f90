!
! @file distdir_mod.f90
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

MODULE distdir_mod

	USE, INTRINSIC :: ISO_C_BINDING, ONLY: c_ptr, c_int, c_null_ptr
	IMPLICIT NONE

	PRIVATE

	INTEGER, PARAMETER :: DISTDIR_HW_CPU        = 0
	INTEGER, PARAMETER :: DISTDIR_HW_GPU_NVIDIA = 1
	INTEGER, PARAMETER :: DISTDIR_HW_GPU_AMD    = 2

	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendIrecv1       = 0
	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendIrecv2       = 1
	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendRecv1        = 2
	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendRecv2        = 3
	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendIrecv1NoWait = 4
	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendIrecv2NoWait = 5
	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendRecv1NoWait  = 6
	INTEGER, PARAMETER :: DISTDIR_EXCHANGER_IsendRecv2NoWait  = 7

	INTEGER, PARAMETER :: DISTDIR_VERBOSE_TRUE  = 0
	INTEGER, PARAMETER :: DISTDIR_VERBOSE_FALSE = 1

	! note: this type must not be extended to contain any other
	! components, its memory pattern has to match void * exactly, which
	! it does because of C constraints
	TYPE, BIND(C), PUBLIC :: t_idxlist
#ifndef __G95__
		PRIVATE
#endif
		TYPE(c_ptr) :: cptr = c_null_ptr
	END TYPE t_idxlist

	! note: this type must not be extended to contain any other
	! components, its memory pattern has to match void * exactly, which
	! it does because of C constraints
	TYPE, BIND(C), PUBLIC :: t_map
#ifndef __G95__
		PRIVATE
#endif
		TYPE(c_ptr) :: cptr = c_null_ptr
	END TYPE t_map

	! note: this type must not be extended to contain any other
	! components, its memory pattern has to match void * exactly, which
	! it does because of C constraints
	TYPE, BIND(C), PUBLIC :: t_exchanger
#ifndef __G95__
		PRIVATE
#endif
		TYPE(c_ptr) :: cptr = c_null_ptr
	END TYPE t_exchanger

	INTERFACE

		SUBROUTINE distdir_initialize_c() BIND(C, name='distdir_initialize')
			IMPLICIT NONE
		END SUBROUTINE distdir_initialize_c

		SUBROUTINE distdir_finalize_c() BIND(C, name='distdir_finalize')
			IMPLICIT NONE
		END SUBROUTINE distdir_finalize_c

		SUBROUTINE new_group_c(new_comm, work_comm, id) &
		                       BIND(C, name='new_group_f')
			IMPORT :: c_int
			IMPLICIT NONE
			INTEGER(c_int), INTENT(OUT) :: new_comm
			INTEGER(c_int), VALUE, INTENT(IN) :: work_comm
			INTEGER(c_int), VALUE, INTENT(IN) :: id
		END SUBROUTINE new_group_c

		SUBROUTINE set_config_exchanger_c(exchanger_type) &
		                                  BIND(C, name='set_config_exchanger')
			IMPORT :: c_int
			IMPLICIT NONE
			INTEGER(c_int), VALUE, INTENT(IN) :: exchanger_type
		END SUBROUTINE set_config_exchanger_c

		SUBROUTINE set_config_verbose_c(verbose_type) &
		                                BIND(C, name='set_config_verbose')
			IMPORT :: c_int
			IMPLICIT NONE
			INTEGER(c_int), VALUE, INTENT(IN) :: verbose_type
		END SUBROUTINE set_config_verbose_c

		! this function must not be implemented in Fortran because
		! PGI 11.x chokes on that
		FUNCTION t_idxlist_f2c(idxlist) BIND(c, name='t_idxlist_f2c') RESULT(p)
			IMPORT :: c_ptr, t_idxlist
			IMPLICIT NONE
			TYPE(t_idxlist), INTENT(IN) :: idxlist
			TYPE(c_ptr)                 :: p
		END FUNCTION t_idxlist_f2c

		FUNCTION new_idxlist_c(list, num_indices) &
		                        BIND(C, name='new_idxlist') RESULT(res_ptr)
			IMPORT :: c_ptr, c_int
			IMPLICIT NONE
			INTEGER(c_int),        INTENT(IN) :: list(*)
			INTEGER(c_int), VALUE, INTENT(IN) :: num_indices
			TYPE(c_ptr)                       :: res_ptr
		END FUNCTION new_idxlist_c

		FUNCTION new_idxlist_empty_c() BIND(C, name='new_idxlist_empty') RESULT(res_ptr)
			IMPORT :: c_ptr
			IMPLICIT NONE
			TYPE(c_ptr) :: res_ptr
		END FUNCTION new_idxlist_empty_c

		SUBROUTINE delete_idxlist_c(ptr) BIND(C, name='delete_idxlist')
			IMPORT :: c_ptr
			IMPLICIT NONE
			TYPE(c_ptr), VALUE, INTENT(in) :: ptr
		END SUBROUTINE delete_idxlist_c

		! this function must not be implemented in Fortran because
		! PGI 11.x chokes on that
		FUNCTION t_map_f2c(map) BIND(c, name='t_map_f2c') RESULT(p)
			IMPORT :: c_ptr, t_map
			IMPLICIT NONE
			TYPE(t_map), INTENT(IN) :: map
			TYPE(c_ptr)             :: p
		END FUNCTION t_map_f2c

		FUNCTION new_map_c(src_idxlist, dst_idxlist, stride, comm) &
		                        BIND(C, name='new_map_f') RESULT(map_ptr)
			IMPORT :: c_ptr, c_int, t_idxlist
			IMPLICIT NONE
			TYPE(t_idxlist), INTENT(in) :: src_idxlist, dst_idxlist
			INTEGER(c_int), VALUE, INTENT(IN) :: stride
			INTEGER(c_int), VALUE, INTENT(in) :: comm
			TYPE(c_ptr) :: map_ptr
		END FUNCTION new_map_c

		FUNCTION extend_map_3d_c(map2d, nlevels) &
		                         BIND(C, name='extend_map_3d_f') RESULT(map_ptr)
			IMPORT :: c_ptr, t_map, c_int
			IMPLICIT NONE
			TYPE(t_map),  INTENT(IN) :: map2d
			INTEGER(c_int), VALUE, INTENT(IN) :: nlevels
			TYPE(c_ptr) :: map_ptr
		END FUNCTION extend_map_3d_c

		SUBROUTINE delete_map_c(ptr) BIND(C, name='delete_map')
			IMPORT :: c_ptr
			IMPLICIT NONE
			TYPE(c_ptr), VALUE, INTENT(in) :: ptr
		END SUBROUTINE delete_map_c

		! this function must not be implemented in Fortran because
		! PGI 11.x chokes on that
		FUNCTION t_exchanger_f2c(exchanger) BIND(c, name='t_exchanger_f2c') RESULT(p)
			IMPORT :: c_ptr, t_exchanger
			IMPLICIT NONE
			TYPE(t_exchanger), INTENT(IN) :: exchanger
			TYPE(c_ptr)                 :: p
		END FUNCTION t_exchanger_f2c

		FUNCTION new_exchanger_c(map, type, hw) &
		                        BIND(C, name='new_exchanger_f') RESULT(exchanger_ptr)
			IMPORT :: c_ptr, c_int, t_map
			IMPLICIT NONE
			TYPE(t_map), INTENT(in) :: map
			INTEGER(c_int), VALUE, INTENT(IN) :: type
			INTEGER(c_int), VALUE, INTENT(IN) :: hw
			TYPE(c_ptr) :: exchanger_ptr
		END FUNCTION new_exchanger_c

		SUBROUTINE delete_exchanger_c(ptr) BIND(C, name='delete_exchanger')
			IMPORT :: c_ptr
			IMPLICIT NONE
			TYPE(c_ptr), VALUE, INTENT(in) :: ptr
		END SUBROUTINE delete_exchanger_c

		SUBROUTINE exchanger_go_c(exchanger, src_data, dst_data) BIND(C, name='exchanger_go_f')
			IMPORT :: c_ptr, t_exchanger
			IMPLICIT NONE
			TYPE(t_exchanger), INTENT(in) :: exchanger
			TYPE(c_ptr), VALUE, INTENT(IN) :: src_data
			TYPE(c_ptr), VALUE, INTENT(IN) :: dst_data
		END SUBROUTINE exchanger_go_c

		SUBROUTINE exchanger_go_with_transform_c(exchanger, src_data, dst_data, transform_src, transform_dst) &
		                                    BIND(C, name='exchanger_go_with_transform_f')
			IMPORT :: c_ptr, t_exchanger, c_int
			IMPLICIT NONE
			TYPE(t_exchanger),  INTENT(IN) :: exchanger
			TYPE(c_ptr), VALUE, INTENT(IN) :: src_data
			TYPE(c_ptr), VALUE, INTENT(IN) :: dst_data
			INTEGER(c_int),     INTENT(IN) :: transform_src(*)
			INTEGER(c_int),     INTENT(IN) :: transform_dst(*)
		END SUBROUTINE exchanger_go_with_transform_c

	END INTERFACE

	INTERFACE new_map
		MODULE PROCEDURE :: new_map_full
		MODULE PROCEDURE :: new_map_default
		MODULE PROCEDURE :: new_map_extend
	END INTERFACE

	INTERFACE new_idxlist
		MODULE PROCEDURE :: new_idxlist_full
		MODULE PROCEDURE :: new_idxlist_empty
	END INTERFACE

	INTERFACE new_exchanger
		MODULE PROCEDURE :: new_exchanger_full
		MODULE PROCEDURE :: new_exchanger_cpu
	END INTERFACE

	INTERFACE exchanger_go
		MODULE PROCEDURE :: exchanger_go_no_transform
		MODULE PROCEDURE :: exchanger_go_with_transform
	END INTERFACE

	PUBLIC :: DISTDIR_HW_CPU, DISTDIR_HW_GPU_AMD, DISTDIR_HW_GPU_NVIDIA
	PUBLIC :: DISTDIR_VERBOSE_TRUE, DISTDIR_VERBOSE_FALSE
	PUBLIC :: DISTDIR_EXCHANGER_IsendIrecv1, DISTDIR_EXCHANGER_IsendIrecv1NoWait
	PUBLIC :: DISTDIR_EXCHANGER_IsendIrecv2, DISTDIR_EXCHANGER_IsendIrecv2NoWait
	PUBLIC :: DISTDIR_EXCHANGER_IsendRecv1, DISTDIR_EXCHANGER_IsendRecv1NoWait
	PUBLIC :: DISTDIR_EXCHANGER_IsendRecv2, DISTDIR_EXCHANGER_IsendRecv2NoWait
	PUBLIC :: distdir_initialize, distdir_finalize
	PUBLIC :: set_config_exchanger, set_config_verbose
	PUBLIC :: new_group
	PUBLIC :: new_idxlist, delete_idxlist
	PUBLIC :: new_map, delete_map
	PUBLIC :: new_exchanger, delete_exchanger, exchanger_go

	CONTAINS

	SUBROUTINE distdir_initialize()

		CALL distdir_initialize_c()
	END SUBROUTINE

	SUBROUTINE distdir_finalize()

		CALL distdir_finalize_c()
	END SUBROUTINE distdir_finalize

	SUBROUTINE new_group(new_comm, work_comm, id)
		INTEGER, INTENT(OUT) :: new_comm
		INTEGER, INTENT(IN)  :: work_comm
		INTEGER, INTENT(IN)  :: id

		CALL new_group_c(new_comm, work_comm, id)
	END SUBROUTINE new_group

	SUBROUTINE set_config_exchanger(exchanger_type)
		INTEGER, INTENT(IN) :: exchanger_type

		CALL set_config_exchanger_c(exchanger_type)
	END SUBROUTINE set_config_exchanger

	SUBROUTINE set_config_verbose(verbose_type)
		INTEGER, INTENT(IN) :: verbose_type

		CALL set_config_verbose_c(verbose_type)
	END SUBROUTINE set_config_verbose

	FUNCTION t_idxlist_c2f(idxlist) RESULT(p)
		TYPE(c_ptr), INTENT(in) :: idxlist
		TYPE(t_idxlist) :: p
		p%cptr = idxlist
	END FUNCTION t_idxlist_c2f

	SUBROUTINE new_idxlist_full(idxlist, list, num_indices)
		type(t_idxlist), INTENT(OUT) :: idxlist
		INTEGER, INTENT(IN) :: list(:)
		INTEGER, INTENT(IN) :: num_indices

		idxlist = t_idxlist_c2f(new_idxlist_c(list, num_indices))
	END SUBROUTINE new_idxlist_full

	SUBROUTINE new_idxlist_empty(idxlist)
		type(t_idxlist), INTENT(OUT) :: idxlist

		idxlist = t_idxlist_c2f(new_idxlist_empty_c())
	END SUBROUTINE new_idxlist_empty

	SUBROUTINE delete_idxlist(idxlist)
		type(t_idxlist), INTENT(INOUT) :: idxlist

		CALL delete_idxlist_c(idxlist%cptr)
		idxlist%cptr = c_null_ptr
	END SUBROUTINE delete_idxlist

	FUNCTION t_map_c2f(map) RESULT(p)
		TYPE(c_ptr), INTENT(in) :: map
		TYPE(t_map) :: p
		p%cptr = map
	END FUNCTION t_map_c2f

	SUBROUTINE new_map_full(map, src_idxlist, dst_idxlist, stride, comm)
		type(t_map),     INTENT(OUT) :: map
		type(t_idxlist), INTENT(IN)  :: src_idxlist
		type(t_idxlist), INTENT(IN)  :: dst_idxlist
		INTEGER,         INTENT(IN)  :: stride
		INTEGER,         INTENT(IN)  :: comm

		map = t_map_c2f(new_map_c(src_idxlist, dst_idxlist, stride, comm))
	END SUBROUTINE new_map_full

	SUBROUTINE new_map_default(map, src_idxlist, dst_idxlist, comm)
		type(t_map),     INTENT(OUT) :: map
		type(t_idxlist), INTENT(IN)  :: src_idxlist
		type(t_idxlist), INTENT(IN)  :: dst_idxlist
		INTEGER,         INTENT(IN)  :: comm

		INTEGER :: stride = -1
		map = t_map_c2f(new_map_c(src_idxlist, dst_idxlist, stride, comm))
	END SUBROUTINE new_map_default

	SUBROUTINE new_map_extend(map3d, map2d, nlevels)
		TYPE(t_map), INTENT(OUT) :: map3d
		TYPE(t_map), INTENT(IN)  :: map2d
		INTEGER,     INTENT(IN)  :: nlevels

		map3d = t_map_c2f(extend_map_3d_c(map2d, nlevels))
	END SUBROUTINE new_map_extend

	SUBROUTINE delete_map(map)
		type(t_map), INTENT(INOUT) :: map

		CALL delete_map_c(map%cptr)
		map%cptr = c_null_ptr
	END SUBROUTINE delete_map

	FUNCTION t_exchanger_c2f(exchanger) RESULT(p)
		TYPE(c_ptr), INTENT(in) :: exchanger
		TYPE(t_exchanger) :: p
		p%cptr = exchanger
	END FUNCTION t_exchanger_c2f

	SUBROUTINE new_exchanger_full(exchanger, map, type, hw)
		type(t_exchanger), INTENT(OUT) :: exchanger
		type(t_map),       INTENT(IN)  :: map
		INTEGER,           INTENT(IN)  :: type
		INTEGER,           INTENT(IN)  :: hw

		exchanger = t_exchanger_c2f(new_exchanger_c(map, type, hw))
	END SUBROUTINE new_exchanger_full

	SUBROUTINE new_exchanger_cpu(exchanger, map, type)
		type(t_exchanger), INTENT(OUT) :: exchanger
		type(t_map),       INTENT(IN)  :: map
		INTEGER,           INTENT(IN)  :: type

		exchanger = t_exchanger_c2f(new_exchanger_c(map, type, DISTDIR_HW_CPU))
	END SUBROUTINE new_exchanger_cpu

	SUBROUTINE delete_exchanger(exchanger)
		type(t_exchanger), INTENT(INOUT) :: exchanger

		CALL delete_exchanger_c(exchanger%cptr)
		exchanger%cptr = c_null_ptr
	END SUBROUTINE delete_exchanger

	SUBROUTINE exchanger_go_no_transform(exchanger, src_data, dst_data)
		TYPE(t_exchanger) :: exchanger
		TYPE(c_ptr)       :: src_data
		TYPE(c_ptr)       :: dst_data

		CALL exchanger_go_c(exchanger, src_data, dst_data)
	END SUBROUTINE exchanger_go_no_transform

	SUBROUTINE exchanger_go_with_transform(exchanger, src_data, dst_data, transform_src, transform_dst)
		TYPE(t_exchanger)   :: exchanger
		TYPE(c_ptr)         :: src_data
		TYPE(c_ptr)         :: dst_data
		INTEGER, INTENT(IN) :: transform_src(:)
		INTEGER, INTENT(IN) :: transform_dst(:)

		CALL exchanger_go_with_transform_c(exchanger, src_data, dst_data, transform_src, transform_dst)
	END SUBROUTINE exchanger_go_with_transform

END MODULE distdir_mod
