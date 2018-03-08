// vim: et:sts=4:ts=4:sw=4
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define LINE_WIDTH       80
#define PASS_MSG        "passed"
#define FAIL_MSG        "failed"

#define fail(s)	    do {                        \
        printf("%s:%d: ", __func__, __LINE__);  \
        fflush(stdout);                         \
        perror(s);                              \
        exit(EXIT_FAILURE);	                    \
    } while (0)

#define test_true(d, v)         do_test((d), (v), 0)
#define test_false(d, v)        do_test((d), (!(v)), 0)
#define test(d, v, e)           do_test((d), ((v) == (e)), 0)
#define not_test(d, v, e)       do_test((d), ((v) != (e)), 0)
#define fatal_test(d, v, e)	    do_test((d), ((v) == (e)), 1)
#define fatal_not_test(d, v, e)	do_test((d), ((v) != (e)), 1)

enum color {
    GREY    = 30,
    RED     = 31,
    GREEN   = 32,
    YELLOW  = 33,
    BLUE    = 34,
    MAGENTA = 35,
    CYAN    = 36,
    WHITE   = 37
};

#define cprintf(fmt, color, ...) printf("\033[%dm" fmt "\033[0m", color, __VA_ARGS__)

static __inline__ void
do_padding(const char *description, const char *outcome) {
    int num_chars = printf("%s", description);
    for (; num_chars < LINE_WIDTH - strlen(outcome); num_chars++)
        putchar('.');

    fflush(stdout);
}

static int
do_test(const char *description, int success, int fatal)
{
    const char *outcome = success ? PASS_MSG : FAIL_MSG;
    do_padding(description, outcome);

    cprintf("%s\n", success ? GREEN : RED, outcome);
    fflush(stdout);

    if (!success && fatal) exit(EXIT_FAILURE);
    return success ? 0 : 1;
}

static void
test_title(const char *title)
{
    size_t len = strlen(title);
    size_t pad = (LINE_WIDTH - len) / 2 - 1;
    size_t mod = (LINE_WIDTH - len) % 2;
    int i;
    assert(pad >= 1);
    putchar('\n');
    for (i = 0; i < pad; i++)
        putchar('=');
    printf(" %s ", title);
    for (i = 0; i < pad + mod; i++)
        putchar('=');
    putchar('\n');
}
