#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>

int cmp(const void *a, const void *b) {
    return *(pid_t *) a - *(pid_t *) b;
}

void fork_children(pid_t *cpids, int children) {
    int i;
    for (i = 0; i < children; i++) {
        pid_t tmp = fork();
        if (tmp < 0) errx(1, "Failed to fork child %d, errno %d\n", i, errno);
        if (tmp == 0) {
            // I am the child
            exit(0);
        };
        usleep(100); //0.1ms
        assert(tmp > 0);
        cpids[i] = tmp;
    }
}

void check_pids_same(pid_t *pids1, pid_t *pids2, unsigned int count) {
    qsort(pids1, count, sizeof(pid_t), cmp);
    qsort(pids2, count, sizeof(pid_t), cmp);
    int i;
    for (i = 0; i < count; i++) {
        if (pids1[i] != pids2[i]) errx(1, "Pid comparison failed\n");
    }
}

void join_children(pid_t *cpids, int children) {
    int i;
    for (i = 0; i < children; i++) {
        int status = 47;
        pid_t wpid = waitpid(cpids[i], &status, 0);
        if (wpid != cpids[i]) errx(1, "Bad wait?\n");
        if (status != 0) errx(1, "Bad child exit value\n");
    }
}
