add_executable(CMockaExample main.c)
target_compile_features(CMockaExample PRIVATE c_std_99)
target_link_libraries(CMockaExample PRIVATE cmocka-static)

enable_testing()
add_test(NAME CMockaExample COMMAND mpirun -n 4 CMockaExample)
