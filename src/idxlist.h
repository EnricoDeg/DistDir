#ifndef IDXLIST_H
#define IDXLIST_H

struct t_idxlist {
    int count;
    int *list;
};
typedef struct t_idxlist t_idxlist;

t_idxlist * new_idxlist(int *idx_array, int num_indices);
t_idxlist * new_idxlist_empty();
void delete_idxlist(t_idxlist *idxlist);

#endif