add_executable(example_basic1 example_basic1.c)
target_link_libraries (example_basic1 distdir)
target_include_directories(example_basic1 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_basic2 example_basic2.c)
target_link_libraries (example_basic2 distdir)
target_include_directories(example_basic2 PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(example_basic3 example_basic3.c)
target_link_libraries (example_basic3 distdir)
target_include_directories(example_basic3 PRIVATE ${PROJECT_SOURCE_DIR})

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
  example_multi_grid1 # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)
