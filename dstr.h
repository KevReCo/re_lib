// dstr.h - v0.2 - kevreco - CC0 1.0 Licence (public domain)
// Close C89 implementation of std::string
// https://github.com/kevreco/re_lib

/*

NOTES:
=====

- Allocators are not implemented.
   - Only '_dstr_reserve' and 'dstr_clear' contains 'malloc' and 'free'.
- npos (std::string::npos) is NOT taken into account yet.
- Unit tests are made in ./testsuite/

CHANGES (DD/MM/YYYY):
====================

- 28/11/2016 (0.2):
  - Make dstr lib C89 compliant.
  - Split dstr_make(size_t capacity) into two other constructor 'dstr_make' and 'dstr_make_reserve'.
  - Add dstr_make_ref. To use non owning str
  - Add dstr_make_with_buffer. To initialize dstr with a non owning buffer (useful for stack buffer)
  - Add dstr_append_fmt.
  - More testsuites.
  - Update notes.

- 13/11/2016 (0.1) - First implementation

TODO:
====

- Implement STD function:
  - dstr_substr.

- Implement Extend function:
  - dstr_assign_fmt

- Find a name for non growing fmt functions (don't realloc if capacity is reached)
  - dstr_append_fmtb ? format_bounded
  - dstr_assign_fmtb ? format_bounded

- testsuite:
  - dstr_find
  - dstr_copy
  - dstr_swap
  - dstr_make_ref
  - dstr_with_buffer
  - dstr_append_fmt

- Add more "dstr_find_and_replace" functions
- Add more "dstr_compare" functions
- Add more "dstr_find" functions
- Use "dstr_replace_with_dstr" as main func instead of "dstr_replace_with_str"

- Add "DSTR_FOR_EACH" macro

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
#include <stdarg.h> // ..., va_list

//-------------------------------------------------------------------------
// dstr - STD API - BEGIN
//-------------------------------------------------------------------------

typedef size_t dstr_size_t; // Not used yet
typedef char dstr_char_t;
typedef dstr_char_t* dstr_it;

typedef struct dstr {
    size_t size;
    size_t capacity;
    dstr_char_t* data;
    dstr_char_t* non_owned_buffer;
} dstr;


void dstr_init(dstr* s);
void dstr_clear(dstr* s);

/// Constructor

// Default constructor. Constructs empty string (zero size and unspecified capacity).
dstr dstr_make();
// Empty string with 'capacity' capacity
dstr dstr_make_reserve(size_t capacity);
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

//If new_cap is greater than the current capacity(), new storage is allocated, and capacity() is made equal or greater than new_cap.
//If new_cap is less than the current capacity(), this is a non-binding shrink request.
//If new_cap is less than the current size(), this is a non-binding shrink-to-fit request equivalent to shrink_to_fit() (since C++11). // @TODO Implement this
// @TODO testsuite
inline void dstr_reserve(dstr* s, size_t capacity);

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

// @TODO document this function
inline void dstr_push_back(dstr* s, const dstr_char_t ch) { dstr_append_char(s, ch); }
// Removes the last character from the string.
void dstr_pop_back(dstr* s);


// Replaces the contents with a copy of dstr 'other'.
void dstr_assign_dstr(dstr* s, const dstr* other);
// Replaces the contents with a copy of str.
void dstr_assign_str(dstr* s, const dstr_char_t* str);
// Replaces the contents with character ch.
void dstr_assign_char(dstr* s, dstr_char_t ch);
// Replaces the contents with count copies of character ch.
void dstr_assign_nchar(dstr* s, size_t count, dstr_char_t ch);
// Replaces the contents with copies of the characters in the range [first, last]
void dstr_assign_range(dstr* s, const dstr_it first, const dstr_it last);

/// Compare

int dstr_compare_dstr(const dstr* s, const dstr* other);
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

// Iterators
inline dstr_it dstr_begin(const dstr* s) { return s->data; }
inline dstr_it dstr_end(const dstr* s)   { return s->data + s->size; }

// @TODO use size_t index
dstr_it dstr_insert(dstr* s, const dstr_it index, const dstr_char_t value);
// @TODO use size_t index
dstr_it dstr_insert_range(dstr* s, const dstr_it index, const dstr_it first, const dstr_it last);

dstr_it dstr_erase(dstr* s, const dstr_it it);
dstr_it dstr_erase_range(dstr* s, const dstr_it first, const dstr_it last);

// Resizes the string to contain count characters.
void dstr_resize(dstr* s, size_t size);
// Resizes and fills extra spaces with 'ch' value
void dstr_resize_fill(dstr* s, size_t size, dstr_char_t ch);

void dstr_replace_with_dstr(dstr* s, size_t index, size_t count, const dstr* replacing);
void dstr_replace_with_str(dstr* s, size_t index, size_t count, const dstr_char_t* replacing);
void dstr_replace_with_nchar(dstr* s, size_t index, size_t count, dstr_char_t ch, size_t ch_count);

// @TODO implement this
// @TODO suitetest
// Returns position of the first character of the found substring or npos if no such substring is found.
size_t dstr_find_dstr(const dstr* s, size_t index, const dstr* sub);
//void dstr_find_str(const );
//void dstr_find_char();
//void dstr_find_nchar();

// @TODO implement this
// @TODO suitetest
//void dstr_substr();

void dstr_copy(const dstr* s, dstr* other);
void dstr_swap(dstr* s, dstr* other);

//-------------------------------------------------------------------------
// dstr - STD API - END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// dstr - Extended API - BEGIN
//-------------------------------------------------------------------------

void dstr_trim(dstr *s);
void dstr_ltrim(dstr* s);
void dstr_rtrim(dstr* s);

// @TODO more testsuite
void dstr_find_and_replace(dstr* s, const dstr_char_t* to_replaced, const dstr_char_t* with);
//@TODO Implement this
//@TODO testsuite
//int dstr_assign_fmt(dstr* s, const char* fmt, ...);

// @TODO suitetest
int dstr_append_fmt(dstr* s, const char* fmt, ...);

typedef const dstr dstr_ref;
// Ref
// Non-owning reference to a string, the result is a const dstr
// No need to call dstr_clear
//@TODO testsuite
dstr_ref dstr_make_ref(const dstr_char_t* str);

// Non-owning reference with buffer.
// Another allocated buffer will be used if needed
// dstr_clear must be used if more data then 'capacity' could be written
//@TODO testsuite
dstr dstr_make_with_buffer(const dstr_char_t* buffer, size_t capacity);

//-------------------------------------------------------------------------
// dstr - Extended API - END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// dstr - Private - BEGIN
//-------------------------------------------------------------------------

int    _dstr_allocated_data(dstr* s);

size_t _dstr_growing_policy(dstr* s, size_t capacity);
// Find a memory block
void*  _dstr_memory_find(const void *memory_ptr, size_t mem_len, const void *pattern_ptr, size_t pattern_len);

//-------------------------------------------------------------------------
// dstr - Private - END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// dstr - API Implementation - BEGIN
//-------------------------------------------------------------------------
enum {
    DSTR_NPOS = (size_t)-1
};

#define _DSTR_MIN_ALLOC 8

// Shared default value to ensure that s->data is always valid with a '\0' char at initialization
char _DSTR_DEFAULT_DATA[1] = {'\0'};

#define _DSTR_GROW(s, needed) \
    dstr_reserve(s, _dstr_growing_policy(s, needed));

#define _DSTR_GROW_IF_NEEDED(s, needed) \
    if (needed > s->capacity) {         \
    _DSTR_GROW(s, needed)               \
}

inline void dstr_init(dstr* s) {

    s->size = 0;
    s->capacity = 0;
    s->data = _DSTR_DEFAULT_DATA;
    s->non_owned_buffer = _DSTR_DEFAULT_DATA;

} // dstr_init

inline void dstr_clear(dstr* s) {

    // dstr is initialized
    if (_dstr_allocated_data(s)) {
        free(s->data);
    }
    dstr_init(s);

} // dstr_clear

inline dstr dstr_make() {
    dstr result;

    dstr_init(&result);

    return result;
} // dstr_make

inline dstr dstr_make_reserve(size_t capacity) {
    dstr result;

    dstr_init(&result);

    if (capacity) {
        dstr_reserve(&result, capacity);
    }

    return result;
} // dstr_make_reserve

inline dstr dstr_make_from_dstr(const dstr* other) {
    dstr result;

    dstr_init(&result);
    dstr_assign_dstr(&result, other);

    return result;
} // dstr_make_from_dstr


inline dstr dstr_make_from_str(const dstr_char_t* str) {
    dstr result;

    dstr_init(&result);
    dstr_assign_str(&result, str);

    return result;
} // dstr_make_from_str

inline dstr dstr_make_from_nchar(size_t count, dstr_char_t ch) {
    dstr result;

    dstr_init(&result);
    dstr_assign_nchar(&result, count, ch);

    return result;
} // dstr_make_from_nchar

inline dstr dstr_make_from_range(const dstr_it first, const dstr_it last) {
    dstr result;
    
    dstr_init(&result);
    dstr_assign_range(&result, first, last);

    return result;
} // dstr_make_from_range

void dstr_reserve(dstr* s, size_t new_capacity) {

    if (!s->size && new_capacity == 0) {
        dstr_clear(s);
    } else if (new_capacity != s->capacity) {

        size_t str_capacity_needed = s->size + 1;// +1 for '\0';
        size_t capacity_needed;
        if (new_capacity > s->capacity) {
            // + 1 for '\0'
            capacity_needed = new_capacity;
        } else if (new_capacity < str_capacity_needed) {
            capacity_needed = str_capacity_needed;
        } else {
            capacity_needed = new_capacity;
        }

        dstr_char_t* new_data = (dstr_char_t*)malloc(capacity_needed * sizeof(dstr_char_t));

        memcpy(new_data, s->data, str_capacity_needed * sizeof(dstr_char_t));

        // dstr is initialized
        if (_dstr_allocated_data(s)) {
            free(s->data);
        }

        s->data = new_data;
        s->capacity = capacity_needed;
    }
} // dstr_reserve

void dstr_append_dstr(dstr* s, const dstr* other) {

    size_t capacity_needed = s->size + other->size + 1; // +1 for '\0'

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data + s->size, other->data, ((other->size + 1) * sizeof(dstr_char_t))); // +1 for '\0'

    s->size += other->size;
} // dstr_append_dstr

void dstr_append_str(dstr* s, const dstr_char_t* str) {

    size_t str_len = strlen(str);
    size_t capacity_needed = s->size + str_len + 1; // +1 for '\0'

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data + s->size, str, str_len + 1); // for the '\0'

    s->size += str_len;
} // dstr_append_str

void dstr_append_char(dstr* s, const dstr_char_t ch) {

    // +1 for char, another +1 for '\0'
    size_t capacity_needed = s->size  + 1 + 1;

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    s->data[s->size] = ch;
    ++s->size;
    s->data[s->size] = '\0';
} // dstr_append_char

void dstr_append_nchar(dstr* s, size_t count, const dstr_char_t ch) {

    size_t capacity_needed = s->size + count + 1; // +1 for '\0'

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

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

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

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

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data, other->data, capacity_needed * sizeof(dstr_char_t));
    s->size = other_size;

} // dstr_assign_dstr

void dstr_assign_str(dstr* s, const dstr_char_t* str) {

    dstr_clear(s);

    size_t str_len = strlen(str);
    size_t capacity_needed = str_len + 1; // +1 for '\0'

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    memcpy(s->data, str, capacity_needed * sizeof(dstr_char_t));

    s->size = str_len;
} // dstr_assign_str

void dstr_assign_char(dstr* s, dstr_char_t ch) {

    dstr_clear(s);

    size_t capacity_needed = 1 + 1; // +1 for char, +1 for '\0'

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    s->data[0] = ch;
    s->data[1] = '\0';
    s->size = 1;
} // dstr_assign_char

void dstr_assign_nchar(dstr* s, size_t count, dstr_char_t ch) {

    dstr_clear(s);

    size_t capacity_needed = count + 1; // +1 for '\0'

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    dstr_it first = s->data;
    dstr_it last = s->data + count;
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

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

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
    dstr_reserve(s, 0);
} // dstr_shrink_to_fit

dstr_it dstr_insert(dstr* s, const dstr_it index, const dstr_char_t ch) {

    assert(index >= s->data && index <= s->data + s->size);
    const ptrdiff_t offset = index - s->data;

    // + 1 for ch, +1 for '\0'
    size_t capacity_needed = s->size + 1 + 1;

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    if (offset < (ptrdiff_t)s->size) {
        memmove(s->data + offset + 1, s->data + offset, ((size_t)s->size - (size_t)offset) * sizeof(dstr_char_t));
    }

    s->data[offset] = ch;
    ++s->size;

    s->data[s->size] = '\0';

    return s->data + offset;
} // dstr_insert

dstr_it dstr_insert_range(dstr* s, const dstr_it index, const dstr_it first, const dstr_it last) {

    assert(index >= s->data && index <= s->data + s->size);

    const size_t count = last - first;
    const ptrdiff_t offset = index - s->data;

    // +1 for '\0'
    size_t capacity_needed = s->size + count + 1;

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    if (offset < (ptrdiff_t)s->size) {
        memmove(s->data + offset + count, s->data + offset, ((size_t)s->size - (size_t)offset) * sizeof(dstr_char_t));
    }

    memcpy(s->data + offset, first, count * sizeof(dstr_char_t));

    s->size += count;

    s->data[s->size] = '\0';

    return s->data + offset;
} // dstr_insert_range

dstr_it dstr_erase(dstr* s, const dstr_it index) {
    assert(index >= s->data && index < (s->data + s->size));

    const ptrdiff_t off = index - s->data;

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

            _DSTR_GROW(s, size + 1);
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

            _DSTR_GROW(s, size + 1);
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

        _DSTR_GROW_IF_NEEDED(s, needed_capacity);

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
    const dstr tmp = {
        strlen(replacing),
        0,
        (dstr_char_t*)replacing,
        (dstr_char_t*)replacing
    };

    dstr_replace_with_dstr(s, index, count, &tmp);
} // dstr_replace_with_str

inline void dstr_replace_with_nchar(dstr* s, size_t index, size_t count, dstr_char_t ch, size_t ch_count) {
    // There is an allocation to contruct 'tmp':
    // @TODO: implement this without any alloc.
    dstr tmp = dstr_make_from_nchar(ch_count, ch);

    dstr_replace_with_dstr(s, index, count, &tmp);
} // dstr_replace_with_nchar


size_t dstr_find_dstr(const dstr* s, size_t pos, const dstr* sub) {

    size_t result = DSTR_NPOS;

    int worth_a_try = sub->size
            && (pos <= s->size)
            && (pos <= (s->size - sub->size));

    if (worth_a_try) {

        void* found = _dstr_memory_find(s->data + pos, s->size, sub->data, sub->size);

        if (found) {
            result = (dstr_char_t*)found - s->data;
        }
    }

    return result;
} // dstr_find_dstr

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

void dstr_find_and_replace(dstr* s, const dstr_char_t* to_replaced, const dstr_char_t* with) {

    const void* found;
    
    size_t t_size = strlen(to_replaced);
    size_t with_size = strlen(with);

    const dstr_char_t* s_begin = s->data;
    const dstr_char_t* s_end = s->data + s->size;

    const dstr_char_t* t_begin = to_replaced;


    while((found = _dstr_memory_find((const void*)s_begin, (size_t)s_end - (size_t)s_begin, (void*)t_begin, t_size))) {
        size_t index = (dstr_char_t*)found - s->data;

        dstr_replace_with_str(s, index, t_size, with);

        // reset begin and end, could be invalidated (realloc)
        // put the next position to index + found word lenght
        s_begin = s->data + index + with_size;
        s_end = s->data + s->size;
    }
} // dstr_find_and_replace

int dstr_append_fmt(dstr* s, const char* fmt, ...)
{

    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);

    // +1 for '\0', needed by vsnprintf
    int formated_string_size = vsnprintf(NULL, 0, fmt, args1) + 1;
    assert(formated_string_size >= 0);

    size_t capacity_needed = s->size + formated_string_size;

    _DSTR_GROW_IF_NEEDED(s, capacity_needed);

    va_end(args1);
    int result = vsnprintf(s->data + s->size, formated_string_size, fmt, args2);
    va_end(args2);

    s->size = capacity_needed - 1;
    return result;
}

//-------------------------------------------------------------------------
// dstr - API Implementation - END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// dstr - Extended Implementation - BEGIN
//-------------------------------------------------------------------------

dstr_ref dstr_make_ref(const dstr_char_t* str) {
    dstr_ref result = {
        0,
        strlen(str),
        (dstr_char_t*)str,
        (dstr_char_t*)str
    };
    return result;
}

dstr dstr_make_with_buffer(const dstr_char_t* buffer, size_t capacity) {
    dstr_ref result = {
        0,
        capacity,
        (dstr_char_t*)buffer,
        (dstr_char_t*)buffer
    };
    return result;
}

//-------------------------------------------------------------------------
// dstr - Extended Implementation - END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// dstr - Private Implementation - BEGIN
//-------------------------------------------------------------------------

int    _dstr_allocated_data(dstr* s) {

    return s->data != s->non_owned_buffer || s->data != _DSTR_DEFAULT_DATA;
} // _dstr_allocated_data

size_t _dstr_growing_policy(dstr* s, size_t needed_size) {

    // If the array is not initialize, set the minimal size.
    // Otherwise increase the size by 50%
    size_t new_capacity = !s->capacity ? _DSTR_MIN_ALLOC : (s->capacity + (s->capacity / 2));

    // Use the greatest of both needed_size and new_capacity
    return new_capacity > needed_size ? new_capacity : needed_size;

} // _dstr_growing_policy

void* _dstr_memory_find(const void* memory_ptr, size_t mem_len, const void* pattern_ptr, size_t pattern_len)
{

    const char *mem_ptr = (const char *)memory_ptr;
    const char *patt_ptr = (const char *)pattern_ptr;

    // pattern_len can't be greater than mem_len
    if ((mem_len == 0) || (pattern_len == 0) || pattern_len > mem_len) {
        return 0;
    }

    // pattern is a char
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

//-------------------------------------------------------------------------
// dstr - Private Implementation - END
//-------------------------------------------------------------------------


#ifdef __cplusplus
} // extern "C"
#endif

#endif // RE_DSTR_H
