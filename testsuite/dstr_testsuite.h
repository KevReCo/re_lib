
#include "string.h"
#include "assert.h"

#include "../dstr.h"
#include "../runit.h"

void print_dstr(const dstr* s);
void print_dstr_ln(const dstr* s);


void dstr_compare_test();
void dstr_constructor_test();

void dstr_reserve_test();
void dstr_append_test();
void dstr_shrink_to_fit_test();
void dstr_insert_test();
void dstr_erase_test();
void dstr_resize_test();
void dstr_replace_test();
void dstr_find_test();

void dstr_trim_test();
void dstr_find_and_replace_test();

void dstr_testsuite() {

    printf("dstr_testsuite\n");
    printf("sizeof_dstr: %d\n", sizeof(dstr));

    // standard api
    dstr_constructor_test();
    dstr_reserve_test();
    dstr_compare_test();
    dstr_append_test();
    dstr_shrink_to_fit_test();
    dstr_insert_test();
    dstr_erase_test();
    dstr_resize_test();
    dstr_replace_test();
    dstr_find_test();

    // extended api
    dstr_trim_test();
    dstr_find_and_replace_test();

}



void dstr_compare_test() {

    printf("dstr_compare_test\n");

    enum {
        SIZE = 6
    };

    dstr str[SIZE];


    for (size_t i = 0; i < SIZE; ++i) {
        dstr_init(&str[i]);
    }

    dstr_append_str(&str[0], "aaa");
    dstr_append_str(&str[1], "zzzzzz");
    dstr_append_str(&str[2], "zzz");
    dstr_append_str(&str[3], "aaaaaa");
    dstr_append_str(&str[4], "zzz\0zzz");
    dstr_append_str(&str[5], "aaa\0aaa");

    RUNIT_ASSERT(!!dstr_compare_dstr(&str[0], &str[1]) == !!strcmp(str[0].data, str[1].data),  "test");
    RUNIT_ASSERT(!!dstr_compare_str(&str[0], str[1].data) == !!strcmp(str[0].data, str[1].data));
    RUNIT_ASSERT(!!dstr_compare_str(&str[4], str[5].data) == !!strcmp(str[4].data, str[5].data));

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        RUNIT_ASSERT(dstr_size(&str[i]) == 0);
    }
} // dstr_compare_test


void dstr_constructor_test() {

    printf("dstr_constructor_test\n");

    enum {
        SIZE = 6
    };

    dstr dummy = dstr_make_from_str("Hello World!");

    
    dstr str[SIZE] = {
        dstr_make(),
        dstr_make_reserve(64),
        dstr_make_from_dstr(&dummy),
        dstr_make_from_str("Hello Everybody!"),
        dstr_make_from_nchar(6, 'd'),
        dstr_make_from_range(&dummy.data[6], &dummy.data[6+5])
    };

    const char* cstr[SIZE] = {
        "",
        "",
        "Hello World!",
        "Hello Everybody!",
        "dddddd",
        "World"
    };

    for (size_t i = 0; i < SIZE; ++i) {
        RUNIT_ASSERT(str[i].size == strlen(cstr[i]));
        RUNIT_ASSERT(dstr_compare_str(&str[i], cstr[i]) == 0);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        RUNIT_ASSERT(dstr_size(&str[i]) == 0);
    }

    // Ref
    {
        dstr_ref ref = dstr_make_ref("Hello World!");
        RUNIT_ASSERT(dstr_compare_str(&ref, "Hello World!") == 0);
    }
    
    // Buffer
    {
        char buffer[256] = "Hello World!";
        dstr buf = dstr_make_with_buffer(buffer, 256);
        RUNIT_ASSERT(dstr_compare_str(&buf, "Hello World!") == 0);
    }
} // dstr_constructor_test


void dstr_reserve_test() {

    printf("dstr_reserve_test\n");

    // use 'enum' instead of 'static const int' to be C89 compliant
    enum {
        SIZE = 9
    };

    dstr str[SIZE] = {
        dstr_make(),
        dstr_make(),
        dstr_make_reserve(32),
        dstr_make_reserve(32),
        dstr_make_reserve(32),
        dstr_make_reserve(32),
        dstr_make_from_str("abcdefg"),
        dstr_make_from_str("abcdefg"),
        dstr_make_from_str("abcdefg")
    };

    // reserve 0 from 0 capacity dstr
    {
        dstr_reserve(&str[0], 0);

        RUNIT_ASSERT(str[0].capacity == 0);
        RUNIT_ASSERT(dstr_compare_str(&str[1], "") == 0);
    }
    // reserve 32 from 0 capacity dstr
    {
        dstr_reserve(&str[1], 32);

        RUNIT_ASSERT(str[1].capacity == 32);
        RUNIT_ASSERT(dstr_compare_str(&str[1], "") == 0);
    }
    
    // reserve 0 from 32 capacity empty dstr
    {
        dstr_reserve(&str[2], 0);

        RUNIT_ASSERT(str[2].capacity == 0);
        RUNIT_ASSERT(dstr_compare_str(&str[2], "") == 0);
    }

    // reserve 64 from 32 capacity empty dstr
    {
        dstr_reserve(&str[3], 64);

        RUNIT_ASSERT(str[3].capacity == 64);
        RUNIT_ASSERT(dstr_compare_str(&str[3], "") == 0);
    }
    
    // reserve 16 from 32 capacity empty dstr
    {
        dstr_reserve(&str[4], 16);

        RUNIT_ASSERT(str[4].capacity == 16);
        RUNIT_ASSERT(dstr_compare_str(&str[4], "") == 0);
    }

    // reserve 32 from 32 capacity empty dstr
    {
        dstr_reserve(&str[5], 32);

        RUNIT_ASSERT(str[5].capacity == 32);
        RUNIT_ASSERT(dstr_compare_str(&str[5], "") == 0);
    }

    // reserve 16 from 8 size dstr
    {
        dstr_reserve(&str[6], 16);

        RUNIT_ASSERT(str[6].capacity == 16);
        RUNIT_ASSERT(dstr_compare_str(&str[6], "abcdefg") == 0);
    }

    // reserve 8 from 16 size dstr
    {
        dstr_reserve(&str[7], 8);

        RUNIT_ASSERT(str[7].capacity == 8);
        RUNIT_ASSERT(dstr_compare_str(&str[7], "abcdefg") == 0);
    }

    // reserve 8 from 8 size dstr
    {
        dstr_reserve(&str[8], 8);

        RUNIT_ASSERT(str[8].capacity == 8);
        RUNIT_ASSERT(dstr_compare_str(&str[8], "abcdefg") == 0);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
} // dstr_reserve_test

void dstr_append_test() {

    printf("dstr_append_test\n");

    enum {
        SIZE = 6
    };
    dstr str[SIZE];

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_init(&str[i]);
    }

    const char* cstr[SIZE] = {
        "",
        "a",
        "abcd",
        "\0",
        "\0a",
        "a\0"
    };

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_append_str(&str[i], cstr[i]);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        RUNIT_ASSERT(str[i].size == strlen(cstr[i]));
        RUNIT_ASSERT(dstr_compare_str(&str[i], cstr[i]) == 0);
    }

    dstr_append_str(&str[0], "a long string");
    dstr_append_str(&str[1], "nother long string");

    RUNIT_ASSERT(str[0].size == strlen("a long string"));
    RUNIT_ASSERT(dstr_compare_str(&str[0], "a long string") == 0);
    RUNIT_ASSERT(str[1].size == strlen("another long string"));
    RUNIT_ASSERT(dstr_compare_str(&str[1], "another long string") == 0);

    dstr_append_char(&str[2], 'e');
    dstr_append_char(&str[2], 'f');
    dstr_append_char(&str[2], 'g');
    dstr_append_char(&str[2], 'h');

    RUNIT_ASSERT(dstr_compare_str(&str[2], "abcdefgh") == 0);

    dstr xyz = dstr_make_from_str("xyz");

    dstr_append_dstr(&str[2], &xyz);

    RUNIT_ASSERT(dstr_compare_str(&str[2], "abcdefghxyz") == 0);

    dstr_append_nchar(&str[2], 3, '0');

    RUNIT_ASSERT(dstr_compare_str(&str[2], "abcdefghxyz000") == 0);

    char array[] = "0123456789";
    dstr_append_range(&str[2], array + 4, array + 7);

    RUNIT_ASSERT(dstr_compare_str(&str[2], "abcdefghxyz000456") == 0);

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        RUNIT_ASSERT(dstr_size(&str[i]) == 0);
    }

} // dstr_append_test

void dstr_shrink_to_fit_test() {

    printf("dstr_shrink_to_fit_test\n");

    enum {
        SIZE = 4
    };

    dstr str[SIZE] = {
        dstr_make(),
        dstr_make_reserve(64),
        dstr_make(),
        dstr_make()
    };

    for (size_t i = 2; i < SIZE; ++i) {
        dstr_append_nchar(&str[i], 10, 'a');
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_shrink_to_fit(&str[i]);
    }

    RUNIT_ASSERT(str[0].size == 0);
    RUNIT_ASSERT(str[0].capacity == 0);

    RUNIT_ASSERT(str[1].size == 0);
    RUNIT_ASSERT(str[1].capacity == 0);

    for (size_t i = 2; i < SIZE; ++i) {
        RUNIT_ASSERT(str[i].size == 10);
        // +1 for '\0'
        RUNIT_ASSERT((str[i].size + 1) == str[i].capacity);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
} // dstr_shrink_to_fit_test

void dstr_insert_test() {

    printf("dstr_insert_test\n");

    enum {
        SIZE = 3
    };

    dstr str[SIZE] = {
        dstr_make(),
        dstr_make_reserve(64),
        dstr_make_from_str("llo Worl"),
    };

    dstr_insert(&str[2], dstr_begin(&str[2]), 'e');
    dstr_insert(&str[2], dstr_begin(&str[2]), 'H');
    dstr_insert(&str[2], dstr_end(&str[2]), 'd');
    dstr_insert(&str[2], dstr_end(&str[2]), '!');


    RUNIT_ASSERT(dstr_compare_str(&str[2], "Hello World!") == 0 );

    char hello[] = {"Hello World!"};

    dstr_insert_range(&str[0], dstr_begin(&str[0]), hello, hello + sizeof(hello));
    dstr_insert_range(&str[1], dstr_begin(&str[1]), hello, hello + sizeof(hello));

    RUNIT_ASSERT(dstr_compare_str(&str[0], "Hello World!") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[1], "Hello World!") == 0);


    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
} // dstr_insert_test

void dstr_erase_test() {

    printf("dstr_erase_test\n");

    enum {
        SIZE = 4
    };

    dstr str[SIZE] = {
        dstr_make(),
        dstr_make_from_str("Hello World!"),
        dstr_make_from_str("Hello World!"),
        dstr_make_from_str("Hello World!")
    };

    for (size_t i = 0; i < 6; ++i) {
        dstr_erase(&str[1], dstr_begin(&str[1]));
    }
    RUNIT_ASSERT(dstr_compare_str(&str[1], "World!") == 0 );

    dstr_erase_range(&str[2], str[2].data + 5, str[2].data + 5 + 7);
    RUNIT_ASSERT(dstr_compare_str(&str[2], "Hello") == 0 );

    dstr_erase_range(&str[2], str[2].data , str[2].data + str[2].size);

    RUNIT_ASSERT(dstr_compare_str(&str[2], "") == 0 );

    dstr_erase_range(&str[3], str[3].data , str[3].data + str[3].size);

    RUNIT_ASSERT(dstr_compare_str(&str[3], "") == 0 );

    dstr_erase_range(&str[0], str[0].data , str[0].data + str[0].size);

    RUNIT_ASSERT(dstr_compare_str(&str[0], "") == 0 );

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
} // dstr_erase_test

void dstr_resize_test() {
    printf("dstr_resize_test\n");

    enum {
        SIZE = 12
    };

    dstr str[SIZE];

    for (size_t i = 0; i < SIZE; ++i) {
        str[i] = dstr_make_reserve(8);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_append_str(&str[i], "aaaaaa");
    }

    dstr_resize(&str[0], 0);  // resize to 0
    dstr_resize(&str[1], 8);  // resize at capacity
    dstr_resize(&str[2], 6);  // resize at size
    dstr_resize(&str[3], 4);  // resize to < size
    dstr_resize(&str[4], 7);  // resize between size and capacity
    dstr_resize(&str[5], 16); // resize behond capacity

    RUNIT_ASSERT(str[0].size == 0);
    RUNIT_ASSERT(str[1].size == 8);
    RUNIT_ASSERT(str[2].size == 6);
    RUNIT_ASSERT(str[3].size == 4);
    RUNIT_ASSERT(str[4].size == 7);
    RUNIT_ASSERT(str[5].size == 16);

    dstr_resize_fill(&str[6],  0, 'b');  // resize to 0
    dstr_resize_fill(&str[7],  8, 'b');  // resize at capacity
    dstr_resize_fill(&str[8],  6, 'b');  // resize at size
    dstr_resize_fill(&str[9],  4, 'b');  // resize to < size
    dstr_resize_fill(&str[10], 7, 'b');  // resize between size and capacity
    dstr_resize_fill(&str[11], 16, 'b'); // resize behond capacity

    RUNIT_ASSERT(dstr_compare_str(&str[6], "") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[7], "aaaaaabb") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[8], "aaaaaa") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[9], "aaaa") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[10], "aaaaaab") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[11], "aaaaaabbbbbbbbbb") == 0);

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
} // dstr_resize_test


void dstr_replace_test() {

    printf("dstr_replace_test\n");

    enum {
        SIZE = 10
    };

    dstr str[SIZE] = {

        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),

        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),

        dstr_make(),
        dstr_make_reserve(64),
        dstr_make(),
        dstr_make_reserve(64),
    };

    dstr replacing_equal_len = dstr_make_from_str("bbbbbbbb");
    dstr replacing_less_len = dstr_make_from_str("bbbb");
    dstr replacing_greater_len = dstr_make_from_str("bbbbbbbbbbbb");

    dstr_replace_with_dstr(&str[0], (size_t)0, str[0].size, &replacing_equal_len);
    dstr_replace_with_dstr(&str[1], (size_t)0, str[1].size, &replacing_less_len);
    dstr_replace_with_dstr(&str[2], (size_t)0, str[2].size, &replacing_greater_len);

    dstr_replace_with_str(&str[3], (size_t)0, 4, "bbbb");
    dstr_replace_with_str(&str[4], (size_t)4, 4, "bbbb");
    dstr_replace_with_str(&str[5], (size_t)2, 4, "bbbb");

    dstr_replace_with_str(&str[6], (size_t)0, 0, "");
    dstr_replace_with_str(&str[7], (size_t)0, 0, "");

    dstr_replace_with_str(&str[8], (size_t)0, 0, "abcdefgh");
    dstr_replace_with_str(&str[9], (size_t)0, 0, "abcdefgh");

    RUNIT_ASSERT(dstr_compare_str(&str[0], "bbbbbbbb") == 0, "01");
    RUNIT_ASSERT(dstr_compare_str(&str[1], "bbbb") == 0, "02");
    RUNIT_ASSERT(dstr_compare_str(&str[2], "bbbbbbbbbbbb") == 0, "03");

    RUNIT_ASSERT(dstr_compare_str(&str[3], "bbbbaaaa") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[4], "aaaabbbb") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[5], "aabbbbaa") == 0);

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
} // dstr_replace_test

void dstr_trim_test() {

    printf("dstr_trim_test\n");

    enum {
        SIZE = 4
    };

    dstr str[SIZE];

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_init(&str[i]);
    }

    const char* cstr[SIZE] = {
        "a",
        "a",
        "abcde",
        ""
    };

    dstr_append_str(&str[0], "\t\t\t\ta");
    dstr_append_str(&str[1], "a\t\t\t\t");
    dstr_append_str(&str[2], "\t\tabcde\t\t");
    dstr_append_str(&str[3], "\t\t\t\t\t");

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_trim(&str[i]);
    }
    for (size_t i = 0; i < SIZE; ++i) {
        RUNIT_ASSERT(dstr_compare_str(&str[i], cstr[i]) == 0);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        RUNIT_ASSERT(dstr_size(&str[i]) == 0);
    }
} // dstr_trim_test

void dstr_find_and_replace_test() {

    printf("dstr_find_and_replace_test\n");

    enum {
        SIZE = 7
    };

    dstr str[SIZE] = {
        dstr_make(),
        dstr_make_reserve(64),
        dstr_make_from_str(""),
        dstr_make_from_str("a"),
        dstr_make_from_str("aa"),
        dstr_make_from_str("aaa"),
        dstr_make_from_str("aaaa"),

    };

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_find_and_replace(&str[i], "aa", "bb");
    }


    RUNIT_ASSERT(dstr_compare_str(&str[0], "") == 0);

    RUNIT_ASSERT(dstr_compare_str(&str[1], "") == 0);

    RUNIT_ASSERT(dstr_compare_str(&str[2], "") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[3], "a") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[4], "bb") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[5], "bba") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[6], "bbbb") == 0);

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }


    // replace nothing from 0 capacity string
    {
        dstr d0 = dstr_make_reserve(0);
        // find nothing and replaced by nothing
        dstr_find_and_replace(&d0, "", "");
        // find nothing and replaced by test
        dstr_find_and_replace(&d0, "", "test");

        RUNIT_ASSERT(dstr_compare_str(&d0, "") == 0);
    }

    // replace nothing from 64 capacity empty string
    {
        dstr d0 = dstr_make_reserve(64);
        // find nothing and replaced by nothing
        dstr_find_and_replace(&d0, "", "");
        // find nothing and replaced by test
        dstr_find_and_replace(&d0, "", "test");

        RUNIT_ASSERT(dstr_compare_str(&d0, "") == 0);
    }

    // replace nothing from "aaaa" string
    {
        dstr d1 = dstr_make_from_str("aaaa");
        // find
        dstr_find_and_replace(&d1, "", "");
        dstr_find_and_replace(&d1, "", "test");

        RUNIT_ASSERT(dstr_compare_str(&d1, "aaaa") == 0);

        dstr_clear(&d1);
    }

    // from "aaaa" replace "aaaa" to ""
    {
        dstr d0 = dstr_make_from_str("aaaa");

        dstr_find_and_replace(&d0, "aaaa", "");

        RUNIT_ASSERT(dstr_compare_str(&d0, "") == 0);

        dstr_clear(&d0);
    }

    // from "aaaa" replace "a" to ""
    {
        dstr d0 = dstr_make_from_str("aaaa");
        dstr_find_and_replace(&d0, "a", "");
        RUNIT_ASSERT(dstr_compare_str(&d0, "") == 0);
        dstr_clear(&d0);
    }

    // from "aaaa" replace "a" by "bbb"
    {
        dstr d0 = dstr_make_from_str("aaaa");

        dstr_find_and_replace(&d0, "a", "bbb");

        RUNIT_ASSERT(dstr_compare_str(&d0, "bbbbbbbbbbbb") == 0);

        dstr_clear(&d0);
    }


} // dstr_find_and_replace_test



void dstr_find_test() {

    printf("dstr_find_test\n");

    {

        dstr str = dstr_make_from_str("Hello World!");
        dstr sub0 = dstr_make_from_str("Hello World!");
        dstr sub1 = dstr_make_from_str("World!");
        dstr sub2 = dstr_make_from_str("World");
        dstr sub3 = dstr_make_from_str("Nothing");
        dstr sub4 = dstr_make_from_str("");

        RUNIT_ASSERT(dstr_find_dstr(&str, 0, &sub0) == 0);
        RUNIT_ASSERT(dstr_find_dstr(&str, 0, &sub1) == 6);
        RUNIT_ASSERT(dstr_find_dstr(&str, 6, &sub1) == 6);
        RUNIT_ASSERT(dstr_find_dstr(&str, 7, &sub1) == DSTR_NPOS);
        RUNIT_ASSERT(dstr_find_dstr(&str, 0, &sub3) == DSTR_NPOS);

        RUNIT_ASSERT(dstr_find_dstr(&str, 0, &sub4) == DSTR_NPOS);



        dstr_clear(&str);
        dstr_clear(&sub1);
        dstr_clear(&sub2);
    }

} // dstr_find_test

void print_dstr(const dstr* s) {

    printf("[\"%s\"]", s->data);
    // or printf("[\"%.*s\"]", s->size, s->data);
    printf("[s:%d]", s->size);
    printf("[c:%d]", s->capacity);
}

void print_dstr_ln(const dstr* s) {

    print_dstr(s);
    printf("\n");

}

