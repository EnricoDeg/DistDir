# Find MPI library
find_package(MPI REQUIRED COMPONENTS C)

if(ENABLE_CUDA)
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES 80)
    endif()
    SET(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -lineinfo -DCUDA ")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCUDA")
    SET(SOURCE_CUDA core/exchange/backend_hardware/backend_cuda.cu)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

set(SOURCE_EXE  ${SOURCE_CUDA}
                utils/check.c
	        setup/setting.c
	        sort/quicksort.c
                sort/mergesort.c
                setup/group.c
                core/indices/idxlist.c
		core/algorithm/backend/backend.c
                core/algorithm/bucket.c
                core/algorithm/map.c
		core/exchange/backend_hardware/backend_cpu.c
		core/exchange/backend_communication/backend_mpi.c
                core/exchange/exchange.c)

add_library(distdir SHARED ${SOURCE_EXE})
target_include_directories(distdir PRIVATE ${PROJECT_SOURCE_DIR})
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
