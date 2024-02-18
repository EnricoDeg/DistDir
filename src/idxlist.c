#include "idxlist.h"
#include <stdlib.h>

struct t_idxlist * new_idxlist(int *idx_array, int num_indices) {
    struct t_idxlist *idxlist;
    idxlist = (struct t_idxlist *)malloc(sizeof(struct t_idxlist));
    idxlist->count = num_indices;
    if (idxlist->count > 0)
        idxlist->list = (int *)malloc(idxlist->count * sizeof(int));
    for (int i = 0; i < idxlist->count; i++)
        idxlist->list[i] = idx_array[i];
    return idxlist;
}

struct t_idxlist * new_idxlist_empty() {
    struct t_idxlist *idxlist;
    idxlist = (struct t_idxlist *)malloc(sizeof(struct t_idxlist));
    idxlist->count = 0;
    idxlist->list = NULL;
    return idxlist;
}

void delete_idxlist(struct t_idxlist *idxlist) {
    if (idxlist->count > 0)
        free(idxlist->list);
    free(idxlist);
}