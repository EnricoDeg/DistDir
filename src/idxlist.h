#ifndef IDXLIST_H
#define IDXLIST_H

struct t_idxlist {
    int count;
    int *list;
};

struct t_idxlist * new_idxlist(int *idx_array, int num_indices);
struct t_idxlist * new_idxlist_empty();
void delete_idxlist(struct t_idxlist *idxlist);

#endif