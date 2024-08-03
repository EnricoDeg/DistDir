# Find MPI library
find_package(MPI REQUIRED)

if(ENABLE_FORTRAN)
    set(CMAKE_Fortran_MODULE_DIRECTORY ${PROJECT_SOURCE_DIR}/modules)
    set(SOURCE_FORTRAN ../bindings/Fortran/distdir_mod.f90)
    SET(CMAKE_Fortran_FLAGS "-cpp ${CMAKE_Fortran_FLAGS}")
endif()

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
                utils/timer.c
            setup/setting.c
            sort/quicksort.c
                sort/mergesort.c
                sort/timsort.c
                setup/group.c
                core/indices/idxlist.c
        core/algorithm/backend/backend.c
                core/algorithm/bucket.c
                core/algorithm/map.c
        core/exchange/backend_hardware/backend_cpu.c
        core/exchange/backend_communication/backend_mpi.c
                core/exchange/exchange.c
                utils/distdir_f2c.c
                ${SOURCE_FORTRAN})

add_library(distdir SHARED ${SOURCE_EXE})
target_include_directories(distdir PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(distdir PRIVATE ${MPI_C_INCLUDE_DIRS})
target_link_libraries(distdir ${MPI_C_LIBRARIES})
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

# Install the library Fortran module
if(ENABLE_FORTRAN)
    install(DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY} DESTINATION ${PROJECT_SOURCE_DIR})
endif()
