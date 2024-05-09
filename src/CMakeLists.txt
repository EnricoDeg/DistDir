# Find MPI library
find_package(MPI REQUIRED COMPONENTS C)

if(ENABLE_CUDA)
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES 80)
    endif()
    SET(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -lineinfo -DCUDA ")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCUDA")
    SET(SOURCE_CUDA backend_cuda.cu)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

set(SOURCE_EXE  ${SOURCE_CUDA}
                check.c
	        setting.c
	        quicksort.c
                mergesort.c
                group.c
                idxlist.c
		backend.c
                bucket.c
                map.c
		backend_cpu.c
		backend_mpi.c
                exchange.c)

add_library(distdir SHARED ${SOURCE_EXE})
if(ENABLE_CUDA)
    target_link_libraries(distdir ${CUDA_LIBRARIES})
    set_property(TARGET distdir PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

install (
  TARGETS distdir
  LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib
  ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)

set(HEADERS_C distdir.h)
install(FILES ${HEADERS_C} DESTINATION ${PROJECT_SOURCE_DIR}/include)
