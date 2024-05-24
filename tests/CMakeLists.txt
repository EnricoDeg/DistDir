set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

####################
# Sequential tests #
####################
# idxlist
add_executable(idxlist_tests core/indices/idxlist_tests.c)
target_compile_features(idxlist_tests PRIVATE c_std_99)
target_link_libraries(idxlist_tests PRIVATE cmocka-static distdir)
target_include_directories(idxlist_tests PRIVATE ${PROJECT_SOURCE_DIR})

# Backend
add_executable(assign_idxlist_elements_to_buckets_tests core/algorithm/backend/assign_idxlist_elements_to_buckets_tests.c)
target_compile_features(assign_idxlist_elements_to_buckets_tests PRIVATE c_std_99)
target_link_libraries(assign_idxlist_elements_to_buckets_tests PRIVATE cmocka-static distdir)
target_include_directories(assign_idxlist_elements_to_buckets_tests PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(num_indices_to_send_to_each_bucket_tests core/algorithm/backend/num_indices_to_send_to_each_bucket_tests.c)
target_compile_features(num_indices_to_send_to_each_bucket_tests PRIVATE c_std_99)
target_link_libraries(num_indices_to_send_to_each_bucket_tests PRIVATE cmocka-static distdir)
target_include_directories(num_indices_to_send_to_each_bucket_tests PRIVATE ${PROJECT_SOURCE_DIR})

#############
# MPI tests #
#############
# Hello world MPI
add_executable(hello_world extra/hello_world.c)
target_link_libraries (hello_world distdir)
target_include_directories(hello_world PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(hello_world_tests extra/hello_world_tests.c)
target_compile_features(hello_world_tests PRIVATE c_std_99)
target_link_libraries(hello_world_tests PRIVATE cmocka-static)

# Backend
add_executable(num_procs_send_to_each_bucket_tests core/algorithm/backend/num_procs_send_to_each_bucket_tests.c)
target_link_libraries (num_procs_send_to_each_bucket_tests distdir)
target_include_directories(num_procs_send_to_each_bucket_tests PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(senders_to_bucket_tests core/algorithm/backend/senders_to_bucket_tests.c)
target_link_libraries(senders_to_bucket_tests distdir)
target_include_directories(senders_to_bucket_tests PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(num_indices_to_bucket_from_each_rank_tests core/algorithm/backend/num_indices_to_bucket_from_each_rank_tests.c)
target_link_libraries(num_indices_to_bucket_from_each_rank_tests PRIVATE distdir)
target_include_directories(num_indices_to_bucket_from_each_rank_tests PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(bucket_idxlist_procs_tests core/algorithm/backend/bucket_idxlist_procs_tests.c)
target_link_libraries(bucket_idxlist_procs_tests PRIVATE distdir)
target_include_directories(bucket_idxlist_procs_tests PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(bucket_idxlist_elements_tests core/algorithm/backend/bucket_idxlist_elements_tests.c)
target_link_libraries(bucket_idxlist_elements_tests PRIVATE distdir)
target_include_directories(bucket_idxlist_elements_tests PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(backend_MPI_tests core/algorithm/backend/backend_MPI_tests.c)
target_compile_features(backend_MPI_tests PRIVATE c_std_99)
target_link_libraries(backend_MPI_tests PRIVATE cmocka-static)

#########################
# Setup tests in CMocka #
#########################
enable_testing()

add_test(NAME idxlist_tests COMMAND idxlist_tests)
add_test(NAME assign_idxlist_elements_to_buckets_tests COMMAND assign_idxlist_elements_to_buckets_tests)
add_test(NAME num_indices_to_send_to_each_bucket_tests COMMAND num_indices_to_send_to_each_bucket_tests)

add_test(NAME hello_world_tests COMMAND hello_world_tests)
add_test(NAME backend_MPI_tests COMMAND backend_MPI_tests)

