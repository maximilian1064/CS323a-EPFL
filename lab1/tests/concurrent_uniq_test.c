// vim: et:sts=4:ts=4:sw=4
#include <stdio.h>
#include "syscalls.h"
#include "test_helper.h"
#include <omp.h>

#define MAX_PRINT_UUIDS    100
#define MIN(a,b)           ((a) < (b) ? (a) : (b))
static int
comparator(const void *a, const void *b) {
    return *(int *) a - *(int *) b;
}

static inline
long check_n_successive_calls(unsigned int n) {
    int i;
    long err;
    int *results = alloca(n * sizeof(int));
    int *sorted_results = alloca(n * sizeof(int));
    omp_set_num_threads(4);

    if (n < 2) return 0;

#pragma omp parallel for
    for (i = 0 ; i < n ; i++) {
        get_unique_id(results + i);
        sorted_results[i] = results[i];
    }

    qsort(sorted_results, n, sizeof(int), &comparator);
    for (i = 1 ; i < n ; i++) {
        if (sorted_results[i - 1] == sorted_results[i]) {
            fprintf(stderr, "At least 2 (not necessarily successive) calls returned the same value.\n");
            fprintf(stderr, "Returned values: [");
            for (i = 0 ; i < MIN(n, MAX_PRINT_UUIDS) ; i++)
                fprintf(stderr, "%d%s", results[i], i == (n - 1) ? "]" : ", ");
            if (n > MAX_PRINT_UUIDS) fprintf(stderr, "..., %d]", results[n - 1]);
            fprintf(stderr, "\n");
            return -1;
        }
    }

    return 0;
}

int main (int argc, char **argv) {

    test_title("GET_UNIQUE_ID syscall: Concurrent Access");
    test("100,000 successive calls return unique values", (int) check_n_successive_calls(100000), 0);

    return 0;
}
