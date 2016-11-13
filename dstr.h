// dstr.h - v0.1 - KevReCo - public domain

/*

NOTES:
=====

- Allocators are not implemented.
   - Btw, only 'dstr_reserve' and 'dstr_clear' contains 'malloc' and 'free'.
- npos (std::string::npos) is NOT taken into account yet.
- Unit tests are made in ./testsuite/

CHANGES (DD/MM/YYYY):
====================

- 13/11/2016 -

TODO:
====

- function dstr_substr.
- function dstr_find.

*/

#ifndef RE_DSTR_H
#define RE_DSTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h" // size_t malloc free
#include "string.h" // strlen, memcpy, memmove, memset
#include "ctype.h"  // isspace
#include "assert.h" // assert
#include <stddef.h> // ptrdiff_t


//-------------------------------------------------------------------------
// dstr: Dynamic String - a C partially equivalent of std::string
//-------------------------------------------------------------------------

#define DSTR_MIN_ALLOC 8

typedef char dstr_char_t;
typedef dstr_char_t* dstr_it;

typedef struct dstr {
    
    dstr_char_t* data;
    size_t size;
    size_t capacity;

} dstr;

//-------------------------------------------------------------------------
// Forward Function Declaration - API
//-------------------------------------------------------------------------

void dstr_init(dstr* s, size_t capacity);
void dstr_clear(dstr* s);

/// Constructor
///
// Default constructor. Constructs empty string (zero size and unspecified capacity).
dstr dstr_make(size_t capacity);
// Copy
dstr dstr_make_from_dstr(const dstr* dstr);
// Copy
dstr dstr_make_from_str(const dstr_char_t* str);
// Fill
dstr dstr_make_from_nchar(size_t count, dstr_char_t ch);
// Range
dstr dstr_make_from_range(const dstr_it first, const dstr_it last);

/// Element access

// Access specified character with bounds checking
dstr_char_t dstr_at(const dstr* s, size_t index);
// Access specified character
dstr_char_t dstr_get(const dstr* s, size_t index);
// Returns a pointer to the first character of a string
inline dstr_char_t* dstr_data(dstr* s) { return s->data; }
// Returns a non-modifiable standard C character array version of the string
inline dstr_char_t* dstr_c_str(dstr* s) { return s->data; }

void dstr_reserve(dstr* s, size_t capacity);

// Appends dstr
void dstr_append_dstr(dstr* s, const dstr* dstr);
// Appends str.
void dstr_append_str(dstr* s, const dstr_char_t* str);
// Appends character ch.
void dstr_append_char(dstr* s, const dstr_char_t ch);
// Appends count copies of character ch
void dstr_append_nchar(dstr* s, size_t count, const dstr_char_t ch);
// Appends characters in the range [first, last]
void dstr_append_range(dstr* s, const dstr_it first, const dstr_it last);

inline void dstr_push_back(dstr* s, const dstr_char_t ch) { dstr_append_char(s, ch); }
// Removes the last character from the string.
void dstr_pop_back(dstr* s);


// Replaces the contents with a copy of dstr.
void dstr_assign_dstr(dstr* s, const dstr* dstr);
// Replaces the contents with a copy of str.
void dstr_assign_str(dstr* s, const dstr_char_t* str);
// Replaces the contents with character ch.
void dstr_assign_char(dstr* s, dstr_char_t ch);
// Replaces the contents with count copies of character ch.
void dstr_assign_nchar(dstr* s, size_t count, dstr_char_t ch);
// Replaces the contents with copies of the characters in the range [first, last]
void dstr_assign_range(dstr* s, const dstr_it first, const dstr_it last);

/// Compare

int dstr_compare_dstr(const dstr* s, const dstr* dstr);
int dstr_compare_str(const dstr* s, const dstr_char_t* str);
//int dstr_compare_range(const dstr* s, const dstr_it first, const dstr_it last);
//int dstr_compare_ndstr(const dstr* s, size_t pos, size_t count, const dstr* dstr);
//int dstr_compare_nstr(const dstr* s, size_t pos, size_t count, const dstr_char_t* str);
//int dstr_compare_nrange(const dstr* s, size_t pos, size_t count, const dstr_it first, const dstr_it last);

// Reduces memory usage by freeing unused memory
void   dstr_shrink_to_fit(dstr* s);
inline int    dstr_empty(const dstr* s) { return !s->size; }
inline size_t dstr_size(const dstr* s) { return s->size; }
inline size_t dstr_length(const dstr* s) { return s->size; }
inline size_t dstr_capacity(const dstr* s) { return s->capacity; }

// Iterator
inline dstr_it dstr_begin(const dstr* s) { return s->data; }
inline dstr_it dstr_end(const dstr* s) { return s->data + s->size; }


dstr_it dstr_insert(dstr* s, const dstr_it pos, const dstr_char_t value);
dstr_it dstr_insert_range(dstr* s, const dstr_it pos, const dstr_it first, const dstr_it last);

dstr_it dstr_erase(dstr* s, const dstr_it it);
dstr_it dstr_erase_range(dstr* s, const dstr_it first, const dstr_it last);

// Resizes the string to contain count characters.
void dstr_resize(dstr* s, size_t size);
// Resizes and fills extra spaces with 'ch' value
void dstr_resize_fill(dstr* s, size_t size, dstr_char_t ch);

dstr_it _dstr_search(const dstr_it first, const dstr_it last, const dstr_it sfirst, const dstr_it slast);
//basic_string& replace( const_iterator first, const_iterator last, const CharT* cstr );

void dstr_replace_with_dstr(dstr* s, size_t pos, size_t count, const dstr* replacing);
void dstr_replace_with_str(dstr* s, size_t pos, size_t count, const dstr_char_t* replacing);
void dstr_replace_with_nchar(dstr* s, size_t pos, size_t count, dstr_char_t ch, size_t ch_count);

//void dstr_substr();

void dstr_copy(const dstr* s, dstr* other);
void dstr_swap(dstr* s, dstr* other);

// Non standard function

void dstr_trim(dstr *s);
void dstr_ltrim(dstr* s);
void dstr_rtrim(dstr* s);

void dstr_find_and_replace(dstr*s, const dstr_char_t* to_replaced, const dstr_char_t* with);

//-------------------------------------------------------------------------
// TEST: (this section should be commented)
//-------------------------------------------------------------------------

void dstr_testsuite();

//-------------------------------------------------------------------------
// Private Declaration
//-------------------------------------------------------------------------

// Find a memory block
size_t _dstr_growing_policy(dstr* s, size_t capacity);
void* _dstr_mem_find(const void *memory_ptr, size_t mem_len, const void *pattern_ptr, size_t pattern_len);
//-------------------------------------------------------------------------
// Function Defintion
//-------------------------------------------------------------------------


static const size_t npos = -1;

// Default value to ensure that s->data is always valid with a '\0' char at initialization
char _DSTR_DEFAULT_DATA[1] = {'\0'};

#define DSTR_ZERO { .data = _DSTR_DEFAULT_DATA, .size = 0, .capacity = 0}

#define DSTR_GROW(s, needed) \
    dstr_reserve(s, _dstr_growing_policy(s, needed));

#define DSTR_GROW_IF_NEEDED(s, needed) \
    if (needed > s->capacity) {   \
    DSTR_GROW(s, needed)           \
}

inline void dstr_init(dstr* s, size_t capacity) {

    s->size = 0;
    s->capacity = 0;

    if (capacity) {
        dstr_reserve(s, capacity);
        s->data[0] = '\0';
    } else {
        s->data = _DSTR_DEFAULT_DATA;
    }

} // dstr_init

inline void dstr_clear(dstr* s) {

    // dstr is initialized
    if (s->capacity) {
        free(s->data);
        dstr_init(s, 0);
    }
} // dstr_clear

dstr dstr_make(size_t capacity) {
    dstr result;
    dstr_init(&result, capacity);

    return result;
} // dstr_make

dstr dstr_make_from_dstr(const dstr* other) {
    dstr result;
    dstr_init(&result, other->size);

    dstr_assign_dstr(&result, other);
    return result;
} // dstr_make_from_dstr


dstr dstr_make_from_str(const dstr_char_t* str) {

    dstr result;
    result = DSTR_ZERO;

    dstr_assign_str(&result, str);

    return result;
} // dstr_make_from_str

dstr dstr_make_from_nchar(size_t count, dstr_char_t ch) {

    dstr result;
    dstr_init(&result, count);

    dstr_assign_nchar(&result, count, ch);

    return result;
} // dstr_make_from_nchar

dstr dstr_make_from_range(const dstr_it first, const dstr_it last) {
    dstr result;
    
    // @TODO init with designate initializer
    dstr_init(&result, 0);
    dstr_assign_range(&result, first, last);

    return result;
} // dstr_make_from_range

void dstr_reserve(dstr* s, size_t new_capacity) {

    if (new_capacity <= s->capacity) {
        return;  // Do nothing
    }

    dstr_char_t* new_data = (dstr_char_t*)malloc(new_capacity * sizeof(dstr_char_t));

    // dstr is initialized
    if (s->capacity) {
        memcpy(new_data, s->data, s->size * sizeof(dstr_char_t));
        free(s->data);
    }

    s->data = new_data;
    s->capacity = new_capacity;
} // dstr_reserve

void dstr_append_dstr(dstr* s, const dstr* other) {

    size_t capacity_needed = s->size + other->size + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data + s->size, other->data, ((other->size + 1) * sizeof(dstr_char_t))); // +1 for '\0'

    s->size += other->size;
} // dstr_append_dstr

void dstr_append_str(dstr* s, const dstr_char_t* str) {

    size_t str_len = strlen(str);
    size_t capacity_needed = s->size + str_len + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data + s->size, str, str_len + 1); // for the '\0'

    s->size += str_len;
} // dstr_append_str

void dstr_append_char(dstr* s, const dstr_char_t ch) {

    // +1 for char, another +1 for '\0'
    size_t capacity_needed = s->size  + 1 + 1;

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    s->data[s->size] = ch;
    ++s->size;
    s->data[s->size] = '\0';
} // dstr_append_char

void dstr_append_nchar(dstr* s, size_t count, const dstr_char_t ch) {

    size_t capacity_needed = s->size + count + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    dstr_char_t* first = s->data + s->size;
    dstr_char_t* last = first + count;
    for (; first != last; ++first) {
        *first = ch;
    }

    s->size += count;
    s->data[s->size] = '\0';
} // dstr_append_nchar

void dstr_append_range(dstr* s, const dstr_it first, const dstr_it last) {

    size_t count = (last - first);

    size_t capacity_needed = s->size + count + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data + s->size, first, (count + 1) * sizeof(dstr_char_t));

    s->size += count;
    s->data[s->size] = '\0';
} // dstr_append_range

void dstr_pop_back(dstr* s) {
    assert(s->size);

    --s->size;
    s->data[s->size] = '\0';
} // dstr_pop_back

void dstr_assign_dstr(dstr* s, const dstr* other) {

    dstr_clear(s);

    size_t other_size = other->size;

    size_t capacity_needed = other_size + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data, other->data, capacity_needed * sizeof(dstr_char_t));
    s->size = other_size;

} // dstr_assign_dstr

void dstr_assign_str(dstr* s, const dstr_char_t* str) {

    dstr_clear(s);

    size_t str_len = strlen(str);
    size_t capacity_needed = str_len + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data, str, capacity_needed * sizeof(dstr_char_t));

    s->size = str_len;
} // dstr_assign_str

void dstr_assign_char(dstr* s, const dstr_char_t ch) {

    dstr_clear(s);

    size_t capacity_needed = 1 + 1; // +1 for char, +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    s->data[0] = ch;
    s->data[1] = '\0';
    s->size = 1;
} // dstr_assign_char

void dstr_assign_nchar(dstr* s, size_t count, const dstr_char_t ch) {

    dstr_clear(s);

    size_t capacity_needed = count + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    dstr_char_t* first = s->data;
    dstr_char_t* last = s->data + count;
    for (; first != last; ++first) {
        *first = ch;
    }

    s->size = count;
    s->data[s->size] = '\0';
} // dstr_assign_nchar

void dstr_assign_range(dstr* s, const dstr_it first, const dstr_it last) {

    dstr_clear(s);

    size_t count = ((size_t)last - (size_t)first);

    size_t capacity_needed = count + 1; // +1 for '\0'

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    s->size = count;

    memcpy(s->data, first, count * sizeof(dstr_char_t));

    s->data[s->size] = '\0';
} // dstr_assign_range


int dstr_compare_dstr(const dstr* s, const dstr* other) {

    int result;
    int min = s->size < other->size ? s->size: other->size;
    // memcmp is used because strncmp terminates on '\0'
    // a dstr can be "aaa\0bbb" with a size of 7
    int cmp = memcmp(s->data, other->data, min);

    if (cmp) {
        result = cmp;
    } else { // strings are equal until 'min' chars count
        result = s->size < other->size ? -1: s->size != other->size;
    }

    return result;
} // dstr_compare_dstr

int dstr_compare_str(const dstr* s, const dstr_char_t* str) {
    return strcmp(s->data, str);
} // dstr_compare_str

void dstr_shrink_to_fit(dstr* s) {
    if (!s->size) {
        if (s->capacity) {
            dstr_clear(s);
        }
    } else {
        size_t byte_used = (s->size * sizeof(dstr_char_t)) + sizeof(dstr_char_t); // + 1 for non-null termintating char
        if (byte_used < s->capacity) {
            dstr_reserve(s, byte_used);
        }
        s->capacity = s->size + 1;
        s->data[s->size] = '\0';
    }
} // dstr_shrink_to_fit

dstr_it dstr_insert(dstr* s, const dstr_it pos, const dstr_char_t ch) {

    assert(pos >= s->data && pos <= s->data + s->size);
    const ptrdiff_t offset = pos - s->data;

    // + 1 for ch, +1 for '\0'
    size_t capacity_needed = s->size + 1 + 1;

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    if (offset < (ptrdiff_t)s->size) {
        memmove(s->data + offset + 1, s->data + offset, ((size_t)s->size - (size_t)offset) * sizeof(dstr_char_t));
    }

    s->data[offset] = ch;
    ++s->size;

    s->data[s->size] = '\0';

    return s->data + offset;
} // dstr_insert

dstr_it dstr_insert_range(dstr* s, const dstr_it pos, const dstr_it first, const dstr_it last) {

    assert(pos >= s->data && pos <= s->data + s->size);

    const size_t count = last - first;
    const ptrdiff_t offset = pos - s->data;

    // +1 for '\0'
    size_t capacity_needed = s->size + count + 1;

    DSTR_GROW_IF_NEEDED(s, capacity_needed);

    if (offset < (ptrdiff_t)s->size) {
        memmove(s->data + offset + count, s->data + offset, ((size_t)s->size - (size_t)offset) * sizeof(dstr_char_t));
    }

    memcpy(s->data + offset, first, count * sizeof(dstr_char_t));

    s->size += count;

    s->data[s->size] = '\0';

    return s->data + offset;
} // dstr_insert_range

dstr_it dstr_erase(dstr* s, const dstr_it pos) {
    assert(pos >= s->data && pos < (s->data + s->size));

    const ptrdiff_t off = pos - s->data;

    size_t count_to_move = ((size_t)s->size - (size_t)off - 1) + 1; // +1 for '\0'
    memmove(s->data + off, s->data + off + 1, count_to_move * sizeof(dstr_char_t));

    --s->size;

    return s->data + off;

} // dstr_erase

dstr_it dstr_erase_range(dstr* s, const dstr_it first, const dstr_it last) {

    assert(first >= s->data && first <= (s->data + s->size));
    assert(last >= s->data && last <= (s->data + s->size));

    const size_t first_index  = (size_t)(first - s->data);
    const size_t last_index   = (size_t)(last - s->data);
    const size_t count_removed = last_index - first_index;
    const size_t count_to_move = (s->size - last_index) + 1; // +1 for '\0'

    memmove(s->data + first_index, s->data + last_index, count_to_move * sizeof(dstr_char_t));

    s->size -= count_removed;

    return s->data + first_index;
} // dstr_erase_range

void dstr_resize(dstr* s, size_t size) {


    if (!size) {
        dstr_clear(s);
    }  else {

        size_t extra_count = 0;

        // +1 for extra char
        if (size + 1 > s->capacity){

            DSTR_GROW(s, size + 1);
            extra_count = s->capacity - s->size;

        } else if (size > s->size){
            extra_count = size - s->size;
        }

        if (extra_count) {
            memset(s->data + s->size, 0, extra_count * sizeof(dstr_char_t));
        }

        s->size = size;
        s->data[s->size] = '\0';
    }
} // dstr_resize

void dstr_resize_fill(dstr* s, size_t size, dstr_char_t ch) {

    if (!size) {
        dstr_clear(s);
    }  else {

        size_t extra_count = 0;

        // +1 for extra char
        if (size + 1 > s->capacity){

            DSTR_GROW(s, size + 1);
            extra_count = s->capacity - s->size;

        } else if (size > s->size){
            extra_count = size - s->size;
        }

        if (extra_count) {

            dstr_char_t* begin = s->data + s->size;
            dstr_char_t* end   = begin + extra_count;
            while (begin != end) {
                *begin = ch;
                ++begin;
            }
        }

        s->size = size;
        s->data[s->size] = '\0';
    }
} // dstr_resize_fill

void dstr_replace_with_dstr(dstr* s, size_t index, size_t count, const dstr* replacing) {

    assert(index <= s->size);
    assert(count <= s->size);
    assert(index + count <= s->size);

    // Juste an alias to make it shorter
    const dstr* r = replacing;

    if (r->size < count) { // mem replacing <  mem to replace

        char* first = s->data + index;
        char* last = (s->data + index + count);

        size_t count_to_move = s->size - (index + count);
        size_t count_removed = count - r->size;

        if (count_to_move) {
            memmove(last - count_removed, last, count_to_move * sizeof(dstr_char_t));
        }
        if (s->size) {
            memcpy(first, r->data, r->size * sizeof(dstr_char_t));
        }

        s->size -= count_removed;
        s->data[s->size] = '\0';

    } else if (r->size > count) { // mem replacing >  mem to replace
        
        size_t extra_count = r->size - count;
        size_t needed_capacity = s->size + extra_count + 1; // +1 for '\0'
        size_t count_to_move = s->size - index - count;

        DSTR_GROW_IF_NEEDED(s, needed_capacity);

        // Need to set this after "grow" because of potential allocation
        char* first = s->data + index;
        char* last = s->data + index + count;

        if (count_to_move) {
            memmove(last + extra_count, last, count_to_move * sizeof(dstr_char_t));
        }

        memcpy(first, r->data, r->size * sizeof(dstr_char_t));

        s->size += extra_count;
        s->data[s->size] = '\0';

    } else { // mem replacing == mem to replace
        char* first = s->data + index;
        memcpy(first, r->data, r->size * sizeof(dstr_char_t));
    }

} // dstr_replace_with_dstr

inline void dstr_replace_with_str(dstr* s, size_t index, size_t count, const dstr_char_t* replacing) {
    // ANSI C99, designated initializer
    const dstr tmp = { .data = (dstr_char_t*)replacing, .size = strlen(replacing), .capacity = 0 };

    dstr_replace_with_dstr(s, index, count, &tmp);
} // dstr_replace_with_str

inline void dstr_replace_with_nchar(dstr* s, size_t index, size_t count, dstr_char_t ch, size_t ch_count) {
    // There is an allocation to contruct 'tmp':
    // @TODO: implement this without any alloc.
    dstr tmp = dstr_make_from_nchar(ch_count, ch);

    dstr_replace_with_dstr(s, index, count, &tmp);
} // dstr_replace_with_nchar

inline void dstr_copy(const dstr* s, dstr* other) {

    dstr_clear(other);
    dstr_resize(other, s->size);
    memcpy(other->data, s->data, s->size * sizeof(dstr_char_t) );
} // dstr_copy

inline void dstr_swap(dstr* s, dstr* other) {
    const dstr tmp = *s;
    *s = *other;
    *other = tmp;
} // dstr_swap

void dstr_trim(dstr* s)
{
    dstr_char_t* cursor_left  = s->data;
    dstr_char_t* cursor_right = s->data + (s->size - 1);

    // Trim right
    while (cursor_right >= cursor_left && isspace(*cursor_right)) {
        --cursor_right;
    }

    // Trim left
    while (cursor_right > cursor_left && isspace(*cursor_left)) {
        ++cursor_left;
    }

    s->size = (cursor_right - cursor_left) + 1;
    memmove(s->data, cursor_left, s->size * sizeof(dstr_char_t));

    s->data[s->size] = '\0';
} // dstr_trim

void dstr_ltrim(dstr* s) {
    char *cursor = s->data;

    while (s->size > 0 && isspace(*cursor)) {
        ++cursor;
        --s->size;
    }

    memmove(s->data, cursor, s->size * sizeof(dstr_char_t));
    s->data[s->size] = '\0';
} // dstr_ltrim

void dstr_rtrim(dstr* s)
{
    while (s->size > 0 && isspace(s->data[s->size - 1])) {
        --s->size;
    }
    s->data[s->size] = '\0';
} // dstr_rtrim

void dstr_find_and_replace(dstr*s, const dstr_char_t* to_replaced, const dstr_char_t* with) {

    const void* found;
    
    size_t t_size = strlen(to_replaced);
    size_t with_size = strlen(with);

    const dstr_char_t* s_begin = s->data;
    const dstr_char_t* t_begin = to_replaced;

    while((found = _dstr_mem_find((void*)s_begin, s->size, (void*)t_begin, t_size))) {

        size_t index = (char*)found - s->data;

        dstr_replace_with_str(s, index, t_size, with);

        s_begin = s->data + index + with_size;
    }
} // dstr_find_and_replace

//-------------------------------------------------------------------------
// Private Definition
//-------------------------------------------------------------------------


size_t _dstr_growing_policy(dstr* s, size_t needed_size) {

    // If the array is not initialize, set the minimal size.
    // Otherwise increase the size by 50%
    size_t new_capacity = !s->capacity ? DSTR_MIN_ALLOC : (s->capacity + (s->capacity / 2));

    // Use the greatest of both needed_size and new_capacity
    return new_capacity > needed_size ? new_capacity : needed_size;

} // _dstr_growing_policy

void* _dstr_mem_find(const void *memory_ptr, size_t mem_len, const void *pattern_ptr, size_t pattern_len)
{

    const char *mem_ptr = (const char *)memory_ptr;
    const char *patt_ptr = (const char *)pattern_ptr;

    // pattern_len can't be greater than mem_len
    if ((mem_len == 0) || (pattern_len == 0) || pattern_len > mem_len) {
        return 0;
    }

    if (pattern_len == 1) {
        return memchr((void*)mem_ptr, *patt_ptr, mem_len);
    }

    // Last possible position
    const char* cur = mem_ptr;
    const char* last = mem_ptr + mem_len - pattern_len;

    while(cur <= last) {
        // Test the first char before calling a function
        if (*cur == *patt_ptr && memcmp(cur, pattern_ptr, pattern_len) == 0) {
            return (void*)cur;
        }
        ++cur;
    }

    return 0;
} // _dstr_mem_find

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RE_DSTR_H
