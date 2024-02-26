#include "bucket.h"

void assign_idxlist_elements_to_buckets(int *bucket_idxlist, int *idxlist, int idxlist_size, int nbuckets) {

    for (int i=0; i < idxlist_size; i++) {
        bucket_idxlist[i] = idxlist[i] / nbuckets;
        if (bucket_idxlist[i] >= nbuckets) bucket_idxlist[i] = nbuckets - 1;
    }

}