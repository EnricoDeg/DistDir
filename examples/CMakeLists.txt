find_package(MPI REQUIRED)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

if(CMAKE_C_COMPILER_ID STREQUAL "NVHPC")
    if(ENABLE_CUDA)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -acc=verystrict -Minfo=accel,inline -gpu=cc80 -DCUDA")
    endif()
endif()

add_executable(example_basic1 example_basic1.c)
target_link_libraries (example_basic1 distdir)
target_link_libraries(example_basic1 ${MPI_C_LIBRARIES})
target_include_directories(example_basic1 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic1 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic1 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic2 example_basic2.c)
target_link_libraries (example_basic2 distdir)
target_link_libraries(example_basic2 ${MPI_C_LIBRARIES})
target_include_directories(example_basic2 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic2 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic2 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic3 example_basic3.c)
target_link_libraries (example_basic3 distdir)
target_link_libraries(example_basic3 ${MPI_C_LIBRARIES})
target_include_directories(example_basic3 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic3 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic3 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic4 example_basic4.c)
target_link_libraries (example_basic4 distdir)
target_link_libraries(example_basic4 ${MPI_C_LIBRARIES})
target_include_directories(example_basic4 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic4 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic4 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()


add_executable(example_basic5 example_basic5.c)
target_link_libraries (example_basic5 distdir)
target_link_libraries(example_basic5 ${MPI_C_LIBRARIES})
target_include_directories(example_basic5 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic5 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic5 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic6 example_basic6.c)
target_link_libraries (example_basic6 distdir)
target_link_libraries(example_basic6 ${MPI_C_LIBRARIES})
target_include_directories(example_basic6 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic6 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic6 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()


add_executable(example_basic7 example_basic7.c)
target_link_libraries (example_basic7 distdir)
target_link_libraries(example_basic7 ${MPI_C_LIBRARIES})
target_include_directories(example_basic7 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic7 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic7 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic8 example_basic8.c)
target_link_libraries (example_basic8 distdir)
target_link_libraries(example_basic8 ${MPI_C_LIBRARIES})
target_include_directories(example_basic8 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic8 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic8 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic9 example_basic9.c)
target_link_libraries (example_basic9 distdir)
target_link_libraries(example_basic9 ${MPI_C_LIBRARIES})
target_include_directories(example_basic9 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic9 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic9 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic10 example_basic10.c)
target_link_libraries (example_basic10 distdir)
target_link_libraries(example_basic10 ${MPI_C_LIBRARIES})
target_include_directories(example_basic10 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic10 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_basic10 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_multi_grid1 example_multi_grid1.c)
target_link_libraries (example_multi_grid1 distdir)
target_link_libraries(example_multi_grid1 ${MPI_C_LIBRARIES})
target_include_directories(example_multi_grid1 PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_multi_grid1 PRIVATE ${MPI_C_INCLUDE_DIRS})
if(ENABLE_CUDA)
    set_property(TARGET example_multi_grid1 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

install (TARGETS
  example_basic1 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic2 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic3 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic4 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic5 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic6 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic7 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic8 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic9 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_basic10 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

install (TARGETS
  example_multi_grid1 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

