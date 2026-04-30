#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TEST_ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "ASSERT FAILED: %s:%d: %s\n", __FILE__, __LINE__, #expr); \
            return 1; \
        } \
    } while (0)

#define TEST_ASSERT_EQ_INT(expected, actual) \
    do { \
        intmax_t _e = (intmax_t)(expected); \
        intmax_t _a = (intmax_t)(actual); \
        if (_e != _a) { \
            fprintf(stderr, "ASSERT FAILED: %s:%d: expected %jd actual %jd: %s\n", \
                    __FILE__, __LINE__, _e, _a, #actual); \
            return 1; \
        } \
    } while (0)

#define TEST_ASSERT_EQ_UINT(expected, actual) \
    do { \
        uintmax_t _e = (uintmax_t)(expected); \
        uintmax_t _a = (uintmax_t)(actual); \
        if (_e != _a) { \
            fprintf(stderr, "ASSERT FAILED: %s:%d: expected %ju actual %ju: %s\n", \
                    __FILE__, __LINE__, _e, _a, #actual); \
            return 1; \
        } \
    } while (0)

#define TEST_ASSERT_EQ_STR(expected, actual) \
    do { \
        const char* _e = (expected); \
        const char* _a = (actual); \
        if (strcmp(_e, _a) != 0) { \
            fprintf(stderr, "ASSERT FAILED: %s:%d: expected \"%s\" actual \"%s\": %s\n", \
                    __FILE__, __LINE__, _e, _a, #actual); \
            return 1; \
        } \
    } while (0)

#define RUN_TEST(fn) \
    do { \
        printf("[ RUN      ] %s\n", #fn); \
        int _rc = (fn)(); \
        if (_rc != 0) { \
            printf("[  FAILED  ] %s\n", #fn); \
            return _rc; \
        } \
        printf("[       OK ] %s\n", #fn); \
    } while (0)

#endif /* TEST_COMMON_H */
