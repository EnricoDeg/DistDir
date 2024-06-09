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

add_executable(example_basic3cpp example_basic3.cpp)
target_link_libraries (example_basic3cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic3cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic3cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

add_executable(example_basic4cpp example_basic4.cpp)
target_link_libraries (example_basic4cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic4cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic4cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

add_executable(example_basic5cpp example_basic5.cpp)
target_link_libraries (example_basic5cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic5cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic5cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

add_executable(example_basic6cpp example_basic6.cpp)
target_link_libraries (example_basic6cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic6cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic6cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

add_executable(example_basic7cpp example_basic7.cpp)
target_link_libraries (example_basic7cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic7cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic7cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

add_executable(example_basic8cpp example_basic8.cpp)
target_link_libraries (example_basic8cpp distdir ${MPI_CXX_LIBRARIES})
target_include_directories(example_basic8cpp PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic8cpp PRIVATE ${MPI_CXX_INCLUDE_DIRS})

install (TARGETS
  example_basic1cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic2cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic3cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic4cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic5cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic6cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic7cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

install (TARGETS
  example_basic8cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)

