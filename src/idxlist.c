#include "idxlist.h"
#include <stdlib.h>

t_idxlist * new_idxlist(int *idx_array, int num_indices) {
    t_idxlist *idxlist;
    idxlist = (t_idxlist *)malloc(sizeof(t_idxlist));
    idxlist->count = num_indices;
    if (idxlist->count > 0)
        idxlist->list = (int *)malloc(idxlist->count * sizeof(int));
    for (int i = 0; i < idxlist->count; i++)
        idxlist->list[i] = idx_array[i];
    return idxlist;
}

t_idxlist * new_idxlist_empty() {
    t_idxlist *idxlist;
    idxlist = (t_idxlist *)malloc(sizeof(t_idxlist));
    idxlist->count = 0;
    idxlist->list = NULL;
    return idxlist;
}

void delete_idxlist(t_idxlist *idxlist) {
    if (idxlist->count > 0)
        free(idxlist->list);
    free(idxlist);
}