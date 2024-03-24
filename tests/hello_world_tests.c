#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <stdlib.h>



static void test(void **state)
{
    int err = system("mpirun -n 2 ./hello_world");
    assert_int_equal(err, 0);
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
