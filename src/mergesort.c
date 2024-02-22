// C program for Merge Sort
#include "mergesort.h"
#include <stdio.h>
#include <stdlib.h>
 
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int *arr, int *arr_idx, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    // Create temp arrays
    int L[n1], R[n2];
    int L_idx[n1], R_idx[n2];
 
    // Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    for (i = 0; i < n1; i++)
        L_idx[i] = arr_idx[l + i];
    for (j = 0; j < n2; j++)
        R_idx[j] = arr_idx[m + 1 + j];
 
    // Merge the temp arrays back into arr[l..r
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            arr_idx[k] = L_idx[i];
            i++;
        }
        else {
            arr[k] = R[j];
            arr_idx[k] = R_idx[j];
            j++;
        }
        k++;
    }
 
    // Copy the remaining elements of L[],
    // if there are any
    while (i < n1) {
        arr[k] = L[i];
        arr_idx[k] = L_idx[i];
        i++;
        k++;
    }
 
    // Copy the remaining elements of R[],
    // if there are any
    while (j < n2) {
        arr[k] = R[j];
        arr_idx[k] = R_idx[j];
        j++;
        k++;
    }
}
 
// l is for left index and r is right index of the
// sub-array of arr to be sorted
void mergeSort(int *arr, int *arr_idx, int l, int r)
{
    if (l < r) {
        int m = l + (r - l) / 2;
 
        // Sort first and second halves
        mergeSort(arr, arr_idx, l, m);
        mergeSort(arr, arr_idx, m + 1, r);
 
        merge(arr, arr_idx, l, m, r);
    }
}