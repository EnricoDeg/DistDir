add_executable(example_basic main.c)
target_link_libraries (example_basic distdir)
target_include_directories(example_basic PRIVATE ${PROJECT_SOURCE_DIR})

install (TARGETS
  example_basic # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/examples)

