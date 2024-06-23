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

add_executable(example_basic3fc example_basic3.f90)
set_property(TARGET example_basic3fc PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries (example_basic3fc distdir ${MPI_Fortran_LIBRARIES})
target_include_directories(example_basic3fc PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic3fc PRIVATE ${MPI_Fortran_INCLUDE_DIRS})

add_executable(example_basic4fc example_basic4.f90)
set_property(TARGET example_basic4fc PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries (example_basic4fc distdir ${MPI_Fortran_LIBRARIES})
target_include_directories(example_basic4fc PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic4fc PRIVATE ${MPI_Fortran_INCLUDE_DIRS})

add_executable(example_basic5fc example_basic5.f90)
set_property(TARGET example_basic5fc PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries (example_basic5fc distdir ${MPI_Fortran_LIBRARIES})
target_include_directories(example_basic5fc PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic5fc PRIVATE ${MPI_Fortran_INCLUDE_DIRS})

add_executable(example_basic8fc example_basic8.f90)
set_property(TARGET example_basic8fc PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries (example_basic8fc distdir ${MPI_Fortran_LIBRARIES})
target_include_directories(example_basic8fc PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_basic8fc PRIVATE ${MPI_Fortran_INCLUDE_DIRS})

add_executable(example_multi_grid1fc example_multi_grid1.f90)
set_property(TARGET example_multi_grid1fc PROPERTY LINKER_LANGUAGE Fortran)
target_link_libraries (example_multi_grid1fc distdir ${MPI_Fortran_LIBRARIES})
target_include_directories(example_multi_grid1fc PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(example_multi_grid1fc PRIVATE ${MPI_Fortran_INCLUDE_DIRS})

install (TARGETS
  example_basic1fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)

install (TARGETS
  example_basic2fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)

install (TARGETS
  example_basic3fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)

install (TARGETS
  example_basic4fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)

install (TARGETS
  example_basic5fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)

install (TARGETS
  example_basic8fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)

install (TARGETS
  example_multi_grid1fc # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/Fortran/examples)
