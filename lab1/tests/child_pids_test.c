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

    test_title("GET_CHILD_PIDS syscall");

    // to store our results
    size_t nr_children;
    pid_t pid_list[3];

    fatal_not_test("syscall is implemented", (int) get_child_pids(pid_list, 3, &nr_children), ENOSYS);

    // create two children
    pid_t cpid_list[2];
    fork_children(cpid_list, 2);

    // CASE : Arbitrary address for num_children
    test("Invalid num_children address (47424742)", (int) get_child_pids(pid_list, 3, (size_t *)47424742), EFAULT);

    // CASE : Arbitrary address for pid_list 
    test("Invalid pid_list address (47424742)", (int) get_child_pids((pid_t *)47424742, 3, &nr_children), EFAULT);

    // CASE : NULL pid_list, non initialized
    test("Invalid pid_list (NULL)", (int) get_child_pids(NULL, 3, &nr_children), EFAULT);

    // CASE : NULL pid_list, limit = 0, num_children = 2
    test("testing for nr_children = 2, limit = 0, pid_list = NULL (Corner Case)", (int) get_child_pids(NULL, 0, &nr_children), 0);
    printf("nr_children: %zd\n", nr_children);

    // CASE : Normal execution, num_children < limit
    test("Testing for nr_children = 2, limit = 3", (int) get_child_pids(pid_list, 3, &nr_children), 0);
    print_list(pid_list, (nr_children <= 3) ? nr_children : 3);
    printf("nr_children: %zd\n", nr_children);

    // CASE : Normal execution, num_children = limit
    test("Testing for nr_children = 2, limit = 2", (int) get_child_pids(pid_list, 2, &nr_children), 0);
    print_list(pid_list, (nr_children <= 2) ? nr_children : 2);
    printf("nr_children: %zd\n", nr_children);

    // CASE : Insufficient Buffer, num_children > limit
    test("Testing for nr_children = 2, limit = 1", (int) get_child_pids(pid_list, 1, &nr_children), ENOBUFS);
    print_list(pid_list, (nr_children <= 1) ? nr_children : 1);
    printf("nr_children: %zd\n", nr_children);

    // CASE : Normal execution, limit = 0
    test("Testing for nr_children = 2, limit = 0", (int) get_child_pids(pid_list, 0, &nr_children), 0);
    printf("nr_children: %zd\n", nr_children);

    // join children
    join_children(cpid_list, 2); 

    return 0;
}
