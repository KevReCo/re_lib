#ifndef RUNIT_H
#define RUNIT_H

/// Very simple unit testing framework for C/C++

static int runit_count        = 0; // Number of test
static int runit_fail         = 0; // Number of failed test
static const char* runit_name = 0; // Optional name to identify where we are durring the test

#define RUNIT_RESET_COUNT { runit_count = runit_fail = 0; } {runit_name = 0; }
#define __RUNIT_ASSERT(X, format, msg) if (!(X)) { \
    fprintf(stderr, "runit test fail (%s:%d) ",  __FILE__, __LINE__); \
    if (runit_name) fprintf(stderr, "[%s] ", runit_name); \
    fprintf(stderr, format, msg); \
    fprintf(stderr,"\n"); \
    ++runit_fail; \
    } \
    ++runit_count;

#define _RUNIT_ASSERT1(X)              __RUNIT_ASSERT(X, "%s", "")
#define _RUNIT_ASSERT2(X, msg)         __RUNIT_ASSERT(X, "%s", msg)
#define _RUNIT_ASSERT3(X, format, msg) __RUNIT_ASSERT(X, format, msg)

#define GET_RUNIT_ASSERT(arg1, arg2, arg3, arg4, ...) arg4
#define ASSERT_MACRO_CHOOSER(...) GET_RUNIT_ASSERT(__VA_ARGS__, \
    _RUNIT_ASSERT3, \
    _RUNIT_ASSERT2, \
    _RUNIT_ASSERT1)

#define RUNIT_ASSERT(...) ASSERT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define RUNIT_RUN(X) \
    RUNIT_RESET_COUNT \
    printf("===== runit %s Run =====\n", #X); \
    X(); \
    printf("===== runit %s Result =====\n", #X); \
    if (runit_fail && runit_count) { \
        fprintf(stderr, "\t%d/%d failed.\n", runit_fail, runit_count); \
    } if (!runit_fail && !runit_count) { \
        fprintf(stderr, "\tNo test performed.\n" ); \
    } else {\
        fprintf(stdout,"\tOK: %d/%d successul.\n", runit_count, runit_count); \
    }

// X must be const char *
#define RUNIT_SET_NAME(X) runit_name = X


/* Example of use


int one = 1;
int two = 3;

static void unit_test_one() {
    RUNIT_ASSERT(one == 1, "error, one != 1");
}

static void unit_test_two() {
    RUNIT_ASSERT(two == 2, "error, two != 2");
}

static void unit_test_all() {
    RUNIT_RUN(unit_test_one);
    RUNIT_RUN(unit_test_two);
}

static void unit_test_all_in_one() {
    unit_test_one();
    unit_test_two();
}

int main(int argc, char **argv) {

    unit_test_all();

    // Do the same thing with only one run
    RUNIT_RUN(unit_test_all_in_one);
}

*/

#endif // ruNIT_H
