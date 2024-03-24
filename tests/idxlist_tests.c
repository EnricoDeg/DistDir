#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <stdlib.h>

#include "src/distdir.h"

static void new_idxlist_test(void **state) {
    int num_indices = 10;
    int *idx_array = (int *)malloc(num_indices*sizeof(int));
    for (int i=0; i<10; i++)
        idx_array[i] = i;
    t_idxlist *idxlist = new_idxlist(idx_array, num_indices);
    assert_int_equal(num_indices, idxlist->count);
    for (int i=0; i<10; i++)
        assert_int_equal(idx_array[i], idxlist->list[i]);
    free(idx_array);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(new_idxlist_test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}