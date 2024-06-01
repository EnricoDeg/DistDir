find_package(MPI REQUIRED)

set(CMAKE_Fortran_MODULE_DIRECTORY ${PROJECT_SOURCE_DIR}/modules)

add_executable(example_basic1fc example_basic1.f90)
set_property(TARGET example_basic1fc PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries (example_basic1fc distdir ${MPI_Fortran_LIBRARIES})
target_include_directories(example_basic1fc PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic1fc PRIVATE ${MPI_Fortran_INCLUDE_DIRS})

add_executable(example_basic2fc example_basic2.f90)
set_property(TARGET example_basic2fc PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries (example_basic2fc distdir ${MPI_Fortran_LIBRARIES})
target_include_directories(example_basic2fc PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic2fc PRIVATE ${MPI_Fortran_INCLUDE_DIRS})

install (TARGETS
  example_basic1fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)

install (TARGETS
  example_basic2fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)
