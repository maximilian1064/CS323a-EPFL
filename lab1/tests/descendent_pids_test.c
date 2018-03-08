// vim: et:sts=4:ts=4:sw=4
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/wait.h>
#include "syscalls.h"
#include "test_helper.h"
#include "child_pids_helper.h"

void print_list(pid_t *list, size_t limit) {
    int i;
    fprintf(stderr, "List content: [");
    for (i = 0 ; i < limit ; i++) {
        fprintf(stderr, "%d%s", list[i], i == (limit - 1) ? "]\n" : ", ");
    }
}

int main (int argc, char **argv) {

    test_title("GET_CHILD_PIDS syscall: Get PID of all descendents");

    // to store our results
    size_t nr_descendents;
    pid_t pid_list[3];

    /* Create one children, which also has one children */

    pid_t child = fork();
    if (child == 0) {
        pid_t child_1 = fork();
        if (child_1 == 0) {
            // Sleep before we finish all the tests
            usleep(10000000);
            exit(0);
        }
        wait(NULL);
        exit(0);

    } else {
        // Ensure all descendents get created
        usleep(100000);

        /* Begin tests */

        // CASE : NULL pid_list, limit = 0, num_children = 2
        test("testing for nr_descendents = 2, limit = 0, pid_list = NULL (Corner Case)", (int) get_child_pids(NULL, 0, &nr_descendents), 0);
        printf("nr_descendents: %zd\n", nr_descendents);

        // CASE : Normal execution, num_children < limit
        test("Testing for nr_descendents = 2, limit = 3", (int) get_child_pids(pid_list, 3, &nr_descendents), 0);
        print_list(pid_list, (nr_descendents <= 3) ? nr_descendents : 3);
        printf("nr_descendents: %zd\n", nr_descendents);

        // CASE : Normal execution, num_children = limit
        test("Testing for nr_descendents = 2, limit = 2", (int) get_child_pids(pid_list, 2, &nr_descendents), 0);
        print_list(pid_list, (nr_descendents <= 2) ? nr_descendents : 2);
        printf("nr_descendents: %zd\n", nr_descendents);

        // CASE : Insufficient Buffer, num_children > limit
        test("Testing for nr_descendents = 2, limit = 1", (int) get_child_pids(pid_list, 1, &nr_descendents), ENOBUFS);
        print_list(pid_list, (nr_descendents <= 1) ? nr_descendents : 1);
        printf("nr_descendents: %zd\n", nr_descendents);

        // CASE : Normal execution, limit = 0
        test("Testing for nr_descendents = 2, limit = 0", (int) get_child_pids(pid_list, 0, &nr_descendents), 0);
        printf("nr_descendents: %zd\n", nr_descendents);

        // join children
        wait(NULL);

        return 0;
    }
}
