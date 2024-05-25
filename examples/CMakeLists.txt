set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

if(CMAKE_C_COMPILER_ID STREQUAL "NVHPC")
    if(ENABLE_CUDA)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -acc=verystrict -Minfo=accel,inline -gpu=cc80 -DCUDA")
    endif()
endif()

add_executable(example_basic1 example_basic1.c)
target_link_libraries (example_basic1 distdir)
target_include_directories(example_basic1 PRIVATE ${PROJECT_SOURCE_DIR})
if(ENABLE_CUDA)
    set_property(TARGET example_basic1 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic2 example_basic2.c)
target_link_libraries (example_basic2 distdir)
target_include_directories(example_basic2 PRIVATE ${PROJECT_SOURCE_DIR})
if(ENABLE_CUDA)
    set_property(TARGET example_basic2 PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

add_executable(example_basic3 example_basic3.c)
target_link_libraries (example_basic3 distdir)
target_include_directories(example_basic3 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_basic4 example_basic4.c)
target_link_libraries (example_basic4 distdir)
target_include_directories(example_basic4 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_basic5 example_basic5.c)
target_link_libraries (example_basic5 distdir)
target_include_directories(example_basic5 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_basic6 example_basic6.c)
target_link_libraries (example_basic6 distdir)
target_include_directories(example_basic6 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_basic7 example_basic7.c)
target_link_libraries (example_basic7 distdir)
target_include_directories(example_basic7 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_basic8 example_basic8.c)
target_link_libraries (example_basic8 distdir)
target_include_directories(example_basic8 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_multi_grid1 example_multi_grid1.c)
target_link_libraries (example_multi_grid1 distdir)
target_include_directories(example_multi_grid1 PRIVATE ${PROJECT_SOURCE_DIR})

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
  example_multi_grid1 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)
