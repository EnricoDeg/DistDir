find_package(MPI REQUIRED)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

####################
# Sequential tests #
####################
# idxlist
add_executable(idxlist_tests core/indices/idxlist_tests.c)
target_compile_features(idxlist_tests PRIVATE c_std_99)
target_link_libraries(idxlist_tests PRIVATE cmocka-static distdir ${MPI_C_LIBRARIES})
target_include_directories(idxlist_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(idxlist_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Backend
add_executable(assign_idxlist_elements_to_buckets_tests core/algorithm/backend/assign_idxlist_elements_to_buckets_tests.c)
target_compile_features(assign_idxlist_elements_to_buckets_tests PRIVATE c_std_99)
target_link_libraries(assign_idxlist_elements_to_buckets_tests PRIVATE cmocka-static distdir ${MPI_C_LIBRARIES})
target_include_directories(assign_idxlist_elements_to_buckets_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(assign_idxlist_elements_to_buckets_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(num_indices_to_send_to_each_bucket_tests core/algorithm/backend/num_indices_to_send_to_each_bucket_tests.c)
target_compile_features(num_indices_to_send_to_each_bucket_tests PRIVATE c_std_99)
target_link_libraries(num_indices_to_send_to_each_bucket_tests PRIVATE cmocka-static distdir ${MPI_C_LIBRARIES})
target_include_directories(num_indices_to_send_to_each_bucket_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(num_indices_to_send_to_each_bucket_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Sorting
add_executable(sorting_tests sort/sorting_tests.c)
target_compile_features(sorting_tests PRIVATE c_std_99)
target_link_libraries(sorting_tests PRIVATE cmocka-static distdir ${MPI_C_LIBRARIES})
target_include_directories(sorting_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(sorting_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Backend CPU
add_executable(packing_cpu_tests core/exchange/backend_hardware/packing_cpu_tests.c)
target_compile_features(packing_cpu_tests PRIVATE c_std_99)
target_link_libraries(packing_cpu_tests PRIVATE cmocka-static distdir ${MPI_C_LIBRARIES})
target_include_directories(packing_cpu_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(packing_cpu_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(unpacking_cpu_tests core/exchange/backend_hardware/unpacking_cpu_tests.c)
target_compile_features(unpacking_cpu_tests PRIVATE c_std_99)
target_link_libraries(unpacking_cpu_tests PRIVATE cmocka-static distdir ${MPI_C_LIBRARIES})
target_include_directories(unpacking_cpu_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(unpacking_cpu_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(allocate_cpu_tests core/exchange/backend_hardware/allocate_cpu_tests.c)
target_compile_features(allocate_cpu_tests PRIVATE c_std_99)
target_link_libraries(allocate_cpu_tests PRIVATE cmocka-static distdir ${MPI_C_LIBRARIES})
target_include_directories(allocate_cpu_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(allocate_cpu_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

#############
# MPI tests #
#############
# Hello world MPI
add_executable(hello_world extra/hello_world.c)
target_link_libraries (hello_world distdir ${MPI_C_LIBRARIES})
target_include_directories(hello_world PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(hello_world PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(hello_world_tests extra/hello_world_tests.c)
target_compile_features(hello_world_tests PRIVATE c_std_99)
target_link_libraries(hello_world_tests PRIVATE cmocka-static)

# Backend
add_executable(num_procs_send_to_each_bucket_tests core/algorithm/backend/num_procs_send_to_each_bucket_tests.c)
target_link_libraries (num_procs_send_to_each_bucket_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(num_procs_send_to_each_bucket_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(num_procs_send_to_each_bucket_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(senders_to_bucket_tests core/algorithm/backend/senders_to_bucket_tests.c)
target_link_libraries(senders_to_bucket_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(senders_to_bucket_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(senders_to_bucket_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(num_indices_to_bucket_from_each_rank_tests core/algorithm/backend/num_indices_to_bucket_from_each_rank_tests.c)
target_link_libraries(num_indices_to_bucket_from_each_rank_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(num_indices_to_bucket_from_each_rank_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(num_indices_to_bucket_from_each_rank_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(bucket_idxlist_procs_tests core/algorithm/backend/bucket_idxlist_procs_tests.c)
target_link_libraries(bucket_idxlist_procs_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(bucket_idxlist_procs_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(bucket_idxlist_procs_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(bucket_idxlist_elements_tests core/algorithm/backend/bucket_idxlist_elements_tests.c)
target_link_libraries(bucket_idxlist_elements_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(bucket_idxlist_elements_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(bucket_idxlist_elements_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Bucket
add_executable(map_idxlist_to_RD_decomp_tests core/algorithm/map_idxlist_to_RD_decomp_tests.c)
target_link_libraries(map_idxlist_to_RD_decomp_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(map_idxlist_to_RD_decomp_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(map_idxlist_to_RD_decomp_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

add_executable(map_RD_decomp_to_idxlist_tests core/algorithm/map_RD_decomp_to_idxlist_tests.c)
target_link_libraries(map_RD_decomp_to_idxlist_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(map_RD_decomp_to_idxlist_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(map_RD_decomp_to_idxlist_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Map
add_executable(map_tests core/algorithm/map_tests.c)
target_link_libraries(map_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(map_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(map_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Setting
add_executable(setting_tests setup/setting_tests.c)
target_link_libraries(setting_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(setting_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(setting_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Group
add_executable(group_tests setup/group_tests.c)
target_link_libraries(group_tests PRIVATE distdir ${MPI_C_LIBRARIES})
target_include_directories(group_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(group_tests PRIVATE ${MPI_C_INCLUDE_DIRS})

# Backend
add_executable(backend_MPI_tests core/algorithm/backend/backend_MPI_tests.c)
target_compile_features(backend_MPI_tests PRIVATE c_std_99)
target_link_libraries(backend_MPI_tests PRIVATE cmocka-static)

# Bucket
add_executable(bucket_MPI_tests core/algorithm/bucket_MPI_tests.c)
target_compile_features(bucket_MPI_tests PRIVATE c_std_99)
target_link_libraries(bucket_MPI_tests PRIVATE cmocka-static)

# Map
add_executable(map_MPI_tests core/algorithm/map_MPI_tests.c)
target_compile_features(map_MPI_tests PRIVATE c_std_99)
target_link_libraries(map_MPI_tests PRIVATE cmocka-static)

# Setting
add_executable(setting_MPI_tests setup/setting_MPI_tests.c)
target_compile_features(setting_MPI_tests PRIVATE c_std_99)
target_link_libraries(setting_MPI_tests PRIVATE cmocka-static)

# Group
add_executable(group_MPI_tests setup/group_MPI_tests.c)
target_compile_features(group_MPI_tests PRIVATE c_std_99)
target_link_libraries(group_MPI_tests PRIVATE cmocka-static)

#########################
# Setup tests in CMocka #
#########################
enable_testing()

add_test(NAME idxlist_tests COMMAND idxlist_tests)
add_test(NAME assign_idxlist_elements_to_buckets_tests COMMAND assign_idxlist_elements_to_buckets_tests)
add_test(NAME num_indices_to_send_to_each_bucket_tests COMMAND num_indices_to_send_to_each_bucket_tests)
add_test(NAME sorting_tests COMMAND sorting_tests)
add_test(NAME packing_cpu_tests COMMAND packing_cpu_tests)
add_test(NAME unpacking_cpu_tests COMMAND unpacking_cpu_tests)
add_test(NAME allocate_cpu_tests COMMAND allocate_cpu_tests)

add_test(NAME hello_world_tests COMMAND hello_world_tests)
add_test(NAME backend_MPI_tests COMMAND backend_MPI_tests)
add_test(NAME bucket_MPI_tests COMMAND bucket_MPI_tests)
add_test(NAME map_MPI_tests COMMAND map_MPI_tests)
add_test(NAME setting_MPI_tests COMMAND setting_MPI_tests)
add_test(NAME group_MPI_tests COMMAND group_MPI_tests)

