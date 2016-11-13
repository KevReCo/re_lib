
#include "string.h"
#include "assert.h"

#include "../dstr.h"
#include "../runit.h"

void print_dstr(const dstr* s);
void dstr_compare_test();
void dstr_constructor_test();
void dstr_append_test();
void dstr_shrink_to_fit_test();
void dstr_insert_test();
void dstr_erase_test();
void dstr_resize_test();
void dstr_replace_test();

void dstr_trim_test();
void dstr_find_and_replace_test();

void dstr_testsuite() {

    printf("dstr_testsuite\n");

    dstr_compare_test();
    dstr_constructor_test();
    dstr_append_test();
    dstr_shrink_to_fit_test();
    dstr_insert_test();
    dstr_erase_test();
    dstr_resize_test();
    dstr_replace_test();

    dstr_trim_test();
    dstr_find_and_replace_test();
}
void dstr_compare_test() {

    printf("dstr_compare_test\n");

    static const int SIZE = 6;
    dstr str[SIZE];


    for (size_t i = 0; i < SIZE; ++i) {
        dstr_init(&str[i], 0);
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
}

void dstr_constructor_test() {

    printf("dstr_constructor_test\n");

    static const int SIZE = 6;

    dstr dummy = dstr_make_from_str("Hello World!");

    dstr str[SIZE] = {
        dstr_make(0),
        dstr_make(64),
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
}

void dstr_append_test() {

    printf("dstr_append_test\n");

    static const int SIZE = 6;
    dstr str[SIZE];

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_init(&str[i], 0);
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

}

void dstr_shrink_to_fit_test() {

    printf("dstr_shrink_to_fit_test\n");

    static const int SIZE = 6;
    dstr str[SIZE];

    dstr_init(&str[0], 0);
    dstr_init(&str[1], 64);
    for (size_t i = 2; i < SIZE; ++i) {
        dstr_init(&str[i], 0);
    }
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
        // +1 for non-null terminated string
        RUNIT_ASSERT((str[i].size + 1) == str[i].capacity);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
}

void dstr_insert_test() {

    printf("dstr_insert_test\n");

    static const int SIZE = 4;
    dstr str[SIZE] {dstr_make(0),
                dstr_make(64),
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
}

void dstr_erase_test() {

    printf("dstr_erase_test\n");

    static const int SIZE = 4;
    dstr str[SIZE] = {
        dstr_make(0),
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
}

void dstr_resize_test() {
    printf("dstr_resize_test\n");

    static const int SIZE = 12;
    dstr str[SIZE] = {
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
        dstr_make(8),
    };

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
}


void dstr_replace_test() {

    printf("dstr_replace_test\n");

    static const int SIZE = 10;
    dstr str[SIZE] = {

        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),

        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),
        dstr_make_from_str("aaaaaaaa"),
        dstr_make(0),
        dstr_make(64),
        dstr_make(0),
        dstr_make(64),
    };

    dstr replacing_equal_len = dstr_make_from_str("bbbbbbbb");
    dstr replacing_less_len = dstr_make_from_str("bbbb");
    dstr replacing_greater_len = dstr_make_from_str("bbbbbbbbbbbb");

    // dstr replacing_bbbb = dstr_make_from_str("bbbb");
    
    dstr_replace_with_dstr(&str[0], 0, str[0].size, &replacing_equal_len);
    dstr_replace_with_dstr(&str[1], 0, str[1].size, &replacing_less_len);
    dstr_replace_with_dstr(&str[2], 0, str[2].size, &replacing_greater_len);

    dstr_replace_with_str(&str[3], 0, 4, "bbbb");
    dstr_replace_with_str(&str[4], 4, 4, "bbbb");
    dstr_replace_with_str(&str[5], 2, 4, "bbbb");

    dstr_replace_with_str(&str[6], 0, 0, "");
    dstr_replace_with_str(&str[7], 0, 0, "");

    dstr_replace_with_str(&str[8], 0, 0, "abcdefgh");
    dstr_replace_with_str(&str[9], 0, 0, "abcdefgh");



    RUNIT_ASSERT(dstr_compare_str(&str[0], "bbbbbbbb") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[1], "bbbb") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[2], "bbbbbbbbbbbb") == 0);

    RUNIT_ASSERT(dstr_compare_str(&str[3], "bbbbaaaa") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[4], "aaaabbbb") == 0);
    RUNIT_ASSERT(dstr_compare_str(&str[5], "aabbbbaa") == 0);

    for (size_t i = 0; i < SIZE; ++i) {
        print_dstr(&str[i]);
        printf("\n");
    }

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
}

void dstr_trim_test() {

    printf("dstr_trim_test\n");

    static const int SIZE = 4;
    dstr str[SIZE];

    for (size_t i = 0; i < SIZE; ++i) {
        dstr_init(&str[i], 0);
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
}

void dstr_find_and_replace_test() {

    printf("dstr_find_and_replace_test\n");

    static const int SIZE = 7;
    dstr str[SIZE] = {
        dstr_make(0),
        dstr_make(64),
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

    {
        dstr d0 = dstr_make(64);
        dstr d1 = dstr_make_from_str("aaaa");

        dstr_find_and_replace(&d0, "", "");
        dstr_find_and_replace(&d0, "", "test");

        dstr_find_and_replace(&d1, "", "");
        dstr_find_and_replace(&d1, "", "test");

        RUNIT_ASSERT(dstr_compare_str(&d0, "") == 0);
        RUNIT_ASSERT(dstr_compare_str(&d1, "aaaa") == 0);
    }

    {

        dstr d0 = dstr_make_from_str("aaaa");
        dstr d1 = dstr_make_from_str("aaaa");

        dstr_find_and_replace(&d0, "aaaa", "");
        dstr_find_and_replace(&d1, "a", "");


        RUNIT_ASSERT(dstr_compare_str(&d0, "") == 0);
        RUNIT_ASSERT(dstr_compare_str(&d1, "") == 0);
    }
    {

        dstr d0 = dstr_make_from_str("aaaa");

        dstr_find_and_replace(&d0, "a", "aaa");
        RUNIT_ASSERT(dstr_compare_str(&d0, "aaaaaaaaaaaa") == 0);
    }

    for (size_t i = 0; i < SIZE; ++i) {
        print_dstr(&str[i]);
        printf("\n");
    }


    for (size_t i = 0; i < SIZE; ++i) {
        dstr_clear(&str[i]);
    }
}


void print_dstr(const dstr* s) {

    printf("[\"%s\"]", s->data);
    // or printf("[\"%.*s\"]", s->size, s->data);
    printf("[s:%d]", s->size);
    printf("[c:%d]", s->capacity);
}
