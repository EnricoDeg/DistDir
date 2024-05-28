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

	! note: this type must not be extended to contain any other
	! components, its memory pattern has to match void * exactly, which
	! it does because of C constraints
	TYPE, BIND(C), PUBLIC :: t_idxlist
#ifndef __G95__
		PRIVATE
#endif
		TYPE(c_ptr) :: cptr = c_null_ptr
	END TYPE t_idxlist

	INTERFACE

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

		SUBROUTINE delete_idxlist_c(ptr) BIND(C, name='delete_idxlist')
			IMPORT :: c_ptr
			IMPLICIT NONE
			TYPE(c_ptr), VALUE, INTENT(in) :: ptr
		END SUBROUTINE delete_idxlist_c

	END INTERFACE

	CONTAINS

	FUNCTION t_idxlist_c2f(idxlist) RESULT(p)
		TYPE(c_ptr), INTENT(in) :: idxlist
		TYPE(t_idxlist) :: p
		p%cptr = idxlist
	END FUNCTION t_idxlist_c2f

	SUBROUTINE new_idxlist(idxlist, list, num_indices)
		type(t_idxlist), INTENT(OUT) :: idxlist
		INTEGER, INTENT(IN) :: list(:)
		INTEGER, INTENT(IN) :: num_indices

		idxlist = t_idxlist_c2f(new_idxlist_c(list, num_indices))
	END SUBROUTINE new_idxlist

	SUBROUTINE delete_idxlist(idxlist)
		type(t_idxlist), INTENT(INOUT) :: idxlist

		CALL delete_idxlist_c(idxlist%cptr)
		idxlist%cptr = c_null_ptr
	END SUBROUTINE delete_idxlist

END MODULE distdir_mod