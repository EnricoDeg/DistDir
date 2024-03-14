#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include "mpi.h"

static void test(void **state)
{
    assert_int_equal(2, 2);
}

int main()
{
    MPI_Init(NULL,NULL);
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test),
    };

    MPI_Finalize();
    return cmocka_run_group_tests(tests, NULL, NULL);
}
