add_executable(hello_world hello_world.c)
target_link_libraries (hello_world distdir)
target_include_directories(hello_world PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(hello_world_tests hello_world_tests.c)
target_compile_features(hello_world_tests PRIVATE c_std_99)
target_link_libraries(hello_world_tests PRIVATE cmocka-static)

add_executable(idxlist_tests idxlist_tests.c)
target_compile_features(idxlist_tests PRIVATE c_std_99)
target_link_libraries(idxlist_tests PRIVATE cmocka-static distdir)
target_include_directories(idxlist_tests PRIVATE ${PROJECT_SOURCE_DIR})

enable_testing()
add_test(NAME hello_world_tests COMMAND hello_world_tests)
add_test(NAME idxlist_tests COMMAND idxlist_tests)
