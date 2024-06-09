find_package(MPI REQUIRED)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

add_executable(example_basic1cpp example_basic1.cpp)
target_link_libraries (example_basic1cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic1cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic1cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

add_executable(example_basic2cpp example_basic2.cpp)
target_link_libraries (example_basic2cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic2cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic2cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

install (TARGETS
  example_basic1cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic2cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

