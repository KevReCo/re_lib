// rjson.h - v0.1 - kevreco - CC0 1.0 Licence (public domain)

/*

NOTES:
=====

- Only a json reader at this moment

CHANGES (DD/MM/YYYY):
====================

- 19/11/2016: First implementation

TODO:
====

- Upload and improve Unit Tests
- Documentation
- Implement better error system.
  - Register an int and then dump the associate string, on demand.
- Add namespace name prior to names ("parse_name" => "_rjson_parse_name").
- Cleanup parse_number.
- Add "RJON_FOR_EACH" macro


DOCUMENTATION:
=============

- @TODO

EXAMPLE:
=======

- @TODO (see the [git_address]/testsuite)
*/

#ifndef RE_RJSON_H
#define RE_RJSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>  // size_t
#include <stdio.h>  // FILE
#include <stdlib.h> // atof

//#define RJSON_STRICT_MODE
#define RJSON_ALLOW_ENDING_COMMA // ex: [0,1,]

#ifdef RJSON_STRICT_MODE

#ifndef RJSON_STRICT_NUMBER_FORMAT
#define RJSON_STRICT_NUMBER_FORMAT
#endif

#ifdef  RJSON_ALLOW_ENDING_COMMA
#undef  RJSON_ALLOW_ENDING_COMMA
#endif

#endif

typedef int             rjson_bool_t;
typedef void*           rej_value_type;
typedef rej_value_type* rej_darray_it;

struct rjson;

// UTF-8 string
typedef struct rjson_string {

    const char* ptr;
    unsigned int length;

} rjson_string; // rjson_string

enum RFlags {
    kRFlags_Float,
    kRFlags_UInt,
    kRFlags_SInt
};

typedef struct rjson_number {

    RFlags flags;

    union {
        unsigned long long u;
        signed long long   s;
        double             d;
    } value;

} rjson_number;

typedef struct rjson rjson;

typedef struct rjson_object {

    rjson_string* name;
    rjson*  value;
    // Linked list, null if last element
    struct rjson_object* next;

} rjson_object;

typedef struct rjson_array {

    rjson* value;
    // Linked list, null if last element
    struct rjson_array* next;

} rjson_array;

//-------------------------------------------------------------------------
// rjson (API)
//-------------------------------------------------------------------------

#define REJ_NULL     0
#define REJ_OBJECT   1
#define REJ_ARRAY    2
#define REJ_STRING   3
#define REJ_NUMBER   4
#define REJ_BOOL     5

typedef struct rjson {

    int type;
    union {
        void* data;
        int bool_value;
    };

} rjson;

rjson* rjson_parse(char* pool, size_t count, const char* src, unsigned int len);
// Print error inforamtion
rjson* rjson_parse_extend(char* pool, size_t count, const char* src, unsigned int len);

void rjson_write_in_file(FILE* f, const rjson* value);

// Get value from object
static inline rjson_bool_t  rjson_get_from_obj(const rjson_object* obj, rjson** value, const char* name);
static inline rjson_string* rjson_obj_name(const rjson_object* obj) {
    return (rjson_string*)&obj->name;
}

static inline rjson_number* rjson_obj_number(const rjson_object* obj) {
    return (rjson_number*)obj->value->data;
}

static inline rjson_string* rjson_obj_string(const rjson_object* obj) {
    return (rjson_string*)obj->value->data;
}

static inline rjson_bool_t rjson_get_from_arr(const rjson_array* arr, rjson** value, size_t index);


// Print json content into f file
static void rjson_to_file(FILE* file, const rjson* value);


//-------------------------------------------------------------------------
// END of rjson (API)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Private Declaration:
//-------------------------------------------------------------------------

#define _rjson_assert(b) if (!(b)) {printf("assert fail: "#b" [%d]\n", __LINE__); return 0;}


//#define RJSON_DEBUG_MESSAGE 1

#ifdef RJSON_DEBUG_MESSAGE // Only for my personnal use, I toggle this when on debugging
#define D(X) X
#else
#define D(X)
#endif

#define RJSON_NEW(parser, type) _rjson_new(parser, sizeof(type))

//-------------------------------------------------------------------------
// Json Memory Pool (fixed size)
//-------------------------------------------------------------------------

typedef struct _rjson_mem_pool {

    char* ptr;
    size_t capacity;
    size_t size;     // define the next allocation ptr

} _rjson_mem_pool;

static inline void _rjson_mem_pool_init(_rjson_mem_pool* pool, char* data, size_t data_count);

//-------------------------------------------------------------------------
// Parser
//-------------------------------------------------------------------------

typedef struct _rjson_parser {

    const char* begin, *end, *cursor;
    size_t row_count, col_count, depth;

    _rjson_mem_pool pool;

} _rjson_parser;


static inline void  _rjson_parser_init(_rjson_parser* parser, const char* src, unsigned int len);
static inline void* _rjson_new(_rjson_parser* parser, size_t bytes);

// assert and consume one char
static inline void _rjson_expect_and_consume(_rjson_parser* parser, char ch);
// assert and goto next token
static inline void _rjson_expect_and_next_token(_rjson_parser* parser, char ch);

static inline void _rjson_consume_one(_rjson_parser* parser);
static inline void _rjson_consume_digits(_rjson_parser* parser);
static inline rjson_bool_t _rjson_consume_4hex(_rjson_parser* parser);
static inline void _rjson_skip_whitespace(_rjson_parser* parser);
static inline void _rjson_goto_next_token(_rjson_parser* parser); // Skip the current token and skip whitespaces

// Parser & json stuff

static inline rjson_object* _rjson_parse_object(_rjson_parser* p);
static inline rjson_array*  _rjson_parse_array(_rjson_parser* p);
static inline rjson*        _rjson_parse_value(_rjson_parser* p);
static inline rjson_number* _rjson_parse_number(_rjson_parser* p);
static inline rjson_string* _rjson_parse_name(_rjson_parser* p);
static inline rjson_string* _rjson_parse_string(_rjson_parser* p);

//-------------------------------------------------------------------------
// Str utils
//-------------------------------------------------------------------------

static inline rjson_bool_t _rjson_is_end_line(char c);
static inline rjson_bool_t _rjson_is_whitespace(char c);
static inline rjson_bool_t _rjson_is_digit(char c);
static inline rjson_bool_t _rjson_is_alpha_hex(char c);
static inline rjson_bool_t _rjson_is_control_char(char c);
// Returns the decimal precision needed to represent the double "losslessly"
static inline int _rjson_get_precision(double x);

static void _rjson_print_string(FILE* f, const rjson_string* str);



//-------------------------------------------------------------------------
// rjson
//-------------------------------------------------------------------------

rjson* rjson_parse(char* pool, size_t count, const char* src, unsigned int len) {

    rjson* result = 0;

    if (!src || !len || !pool || !count) {
        return result;
    }

    _rjson_parser parser;
    // Init parser
    _rjson_parser_init(&parser, src, len);
    // Init memory pool
    _rjson_mem_pool_init(&(parser.pool), pool, count);

    // Skip UTF-8 BOM: EF BB BF
    if (len >= 3
            && parser.cursor[0] == (char)0xEF
            && parser.cursor[1] == (char)0xBB
            && parser.cursor[2] == (char)0xBF) {
        parser.cursor += 3;
    }

    if (*parser.cursor == '{') {

        result = (rjson*)_rjson_new(&parser, sizeof (rjson));
        result->type = REJ_OBJECT;
        result->data = (void*)_rjson_parse_object(&parser);

    } else if (*parser.cursor == '[') {

        result = (rjson*)_rjson_new(&parser, sizeof (rjson));
        result->type = REJ_ARRAY;
        result->data = (void*)_rjson_parse_array(&parser);
    }


#ifdef RE_JSON_NO_EXTRA_DATA

    skip_whitespace(&parser); // check

    json_assert(parser.cursor == parser.end); // check

#endif // RE_JSON_NO_EXTRA_DATA

    return result;
}

static inline rjson_bool_t rjson_get_from_obj(const rjson_object* obj, rjson** valu, const char* name) {

    while (obj) {

        rjson_string * const s = (rjson_string * const)&obj->name;

        if (*s->ptr == *name && strncmp(s->ptr, name, s->length) == 0) {
            *valu = obj->value;
            return 1;
        }

        obj = obj->next;
    };

    return 0;
}

static inline rjson_bool_t rjson_get_from_arr(const rjson_array* arr, rjson** value, size_t index) {

    size_t i = 0;
    while(arr) {


        if (i == index) {
            *value = arr->value;
            return 1;
        }

        arr = arr->next;
        ++i;
    }

    return 0;
}

static inline void rjson_to_file(FILE* f, const rjson* value)
{

    if (value) {
        switch (value->type) {

        case REJ_OBJECT: {
            fputc('{', f);

            const rjson_object* obj = (const rjson_object*)value->data;

            while(obj) {
                if (obj->name && obj->value) {
                    //printf("print obj %p, %p\n", value->data, obj->name);

                    _rjson_print_string(f, obj->name);
                    fputc(':', f);
                    rjson_to_file(f, obj->value);

                    if (obj->next) {
                        fputc(',', f);
                    }
                }
                obj = obj->next;
            };

            fputc('}', f);

            break;
        }

        case REJ_ARRAY: {
            fputc('[', f);

            const rjson_array* arr = (const rjson_array*)value->data;
            while(arr) {
                rjson_to_file(f, arr->value);

                if (arr->next) {
                    fputc(',', f);
                }

                arr = arr->next;
            }
            fputc(']', f);
            break;
        }

        case REJ_STRING:
            _rjson_print_string(f, (const rjson_string*)value->data);
            break;

        case REJ_NUMBER: {
            const rjson_number* num = (const rjson_number*)value->data;
            switch(num->flags) {
            case kRFlags_Float: {
                const double dbl = num->value.d;
                const size_t precision = _rjson_get_precision(dbl);
                if (precision == 1) {

                    fprintf(f, "%.1f", dbl);
                } else {
                    //fprintf(stdout, "%.*g\n", precision, dbl);
                    fprintf(f, "%.*g", precision, dbl);
                }
                break;
            }

#if defined(_WIN32) || defined(_WIN64)
#define FORMAT_S64 "%I64d"
#define FORMAT_U64 "%I64u"
#elif
#define FORMAT_S64 "%lld"
#define FORMAT_U64 "%llu"
#endif

            case kRFlags_UInt: {
                //fprintf(stdout, "%llu\n", num->value.u);
                fprintf(f, FORMAT_U64, num->value.u);
                break;
            }
            case kRFlags_SInt: {
                //fprintf(stdout, "%lld\n", num->value.s);

                fprintf(f, FORMAT_S64, num->value.s);
                break;
            }

            } // switch(num->flags)
            break;
        } // case REJ_NUMBER

        case REJ_BOOL: {
            int boolean = (int)value->data;
            boolean ? fputs("true", f) : fputs("false", f);
            break;
        }
        case REJ_NULL:
            fputs("null", f);
            break;
        default:
            // @TODO better error handling
            fprintf(f, "<unknown-type> %d\n", value->type);
            break;
        }
    }
} // rjson_to_file


//-------------------------------------------------------------------------
// Private Definition
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Json Memory Pool (fixed size)
//-------------------------------------------------------------------------

static inline void _rjson_mem_pool_init(_rjson_mem_pool* pool, char* data, size_t data_count) {

    pool->ptr = data;
    pool->capacity = data_count;
    pool->size = 0;
}

//-------------------------------------------------------------------------
// Parser
//-------------------------------------------------------------------------

static inline void _rjson_parser_init(_rjson_parser* parser, const char* src, unsigned int len) {

    parser->begin = src;
    parser->end = src + len;
    parser->cursor = src;

    parser->row_count = 0;
    parser->col_count = 0;
    parser->depth  = 0;
}


// Memory
static inline void* _rjson_new(_rjson_parser* parser, size_t bytes) {

    void* result = 0;

    if (parser->pool.size + bytes > parser->pool.capacity) {
        fprintf(stderr, "memory pool error: can't allocate memory. needed (%d). capacity (%d)\n", parser->pool.size + bytes, parser->pool.capacity);

    } else {

        result = &parser->pool.ptr[parser->pool.size];
        parser->pool.size += bytes;
    }

    return result;
}


static void _rjson_expect_and_consume(_rjson_parser* parser, char ch) {

    if (*parser->cursor == ch) {
        _rjson_consume_one(parser);
    } else {
        fprintf(stderr, "Internal Error(%d:%d): Unexpected char '%c' instead of '%c'\n",
                parser->row_count,
                parser->col_count,
                *parser->cursor,
                ch);
    }
}

static void _rjson_expect_and_next_token(_rjson_parser* parser, char ch) {

    if (*parser->cursor == ch) {
        _rjson_goto_next_token(parser);
    } else {
        fprintf(stderr, "Internal Error(%d:%d): Unexpected char '%c' instead of '%c'\n",
                parser->row_count,
                parser->col_count,
                *parser->cursor,
                ch);
    }
}

static inline void _rjson_consume_one(_rjson_parser* parser) {

    if (*parser->cursor == '\n' || *parser->cursor == '\r') {

        // \r\n is considered as "one" but two characters are skipped
        // thx to stb_c_lexer for this nice 'trick'
        parser->cursor += (parser->cursor[0]+parser->cursor[1] == '\r'+'\n' ? 2 : 1); // skip newline
        ++parser->row_count;
        parser->col_count = 0;
    } else {

        ++parser->cursor;
        ++parser->col_count; // column couting
    }

}


static inline void _rjson_consume_digits(_rjson_parser* parser) {
    while (_rjson_is_digit(*parser->cursor)) {
        ++parser->cursor;
    }
}

static inline rjson_bool_t _rjson_consume_4hex(_rjson_parser* parser) {
    for (int i = 0; i < 4; ++i) {
        _rjson_assert( _rjson_is_alpha_hex(*parser->cursor) );
        _rjson_consume_one(parser);
    }

    return 1;
}


static inline void _rjson_skip_whitespace(_rjson_parser* parser) {

    while(_rjson_is_whitespace(*parser->cursor)) {
        _rjson_consume_one(parser);
    }
}

// Skip the current token and skip whitespaces
static inline void _rjson_goto_next_token(_rjson_parser* parser) {
    _rjson_consume_one(parser);
    _rjson_skip_whitespace(parser);
}


static inline rjson_number* _rjson_parse_number(_rjson_parser* parser) {

    enum SIGN { kPositive, kNegative } sign = kPositive;

    rjson_number* number = (rjson_number*)_rjson_new(parser, sizeof(rjson_number));
    number->flags = kRFlags_UInt;

    const char* start = parser->cursor;



#ifdef RJSON_STRICT_NUMBER_FORMAT
    if (*parser->cursor == '0') { // if the number starts with a zero, we assume that is a single 0 number

        consume_one(parser);
        if (is_digit(*parser->cursor)) {
            return 0;
        }
    } else {
        sign = *parser->cursor == '-' ? kNegative : kPositive;
        if (*parser->cursor == '-' || *parser->cursor == '+') {
            consume_one(parser);
        }
        consume_digits(parser);
    }
#else
    sign = *parser->cursor == '-' ? kNegative : kPositive;

    if (*parser->cursor == '-' || *parser->cursor == '+') {
        _rjson_consume_one(parser);
    }
    _rjson_consume_digits(parser);
#endif // RE_JSON_STRICT_NUMBER_FORMAT

    if (*parser->cursor == '.') {
        _rjson_consume_one(parser); // skip '.'
        _rjson_consume_digits(parser);
        number->flags = kRFlags_Float;

    }

    if (*parser->cursor == 'e' || *parser->cursor == 'E') {

        _rjson_consume_one(parser); // skip 'e' or 'E'

        if (*parser->cursor == '+' || *parser->cursor == '-') {
            _rjson_consume_one(parser);
        }
        _rjson_assert(_rjson_is_digit(*parser->cursor));
        _rjson_consume_digits(parser);
        number->flags = kRFlags_Float;
    }



    D(printf("number: %.*s\n", number->length, start));

    if (number->flags == kRFlags_Float) {
        number->value.d = strtod(start, (char**)&parser->cursor);
    } else if (sign == kPositive) {
        number->flags = kRFlags_UInt;
        number->value.u = strtoull(start, (char**)&parser->cursor, 10);
    } else if (sign == kNegative) {
        number->flags = kRFlags_SInt;
        number->value.s = strtoll(start, (char**)&parser->cursor, 10);
    }

    if (_rjson_is_whitespace(*parser->cursor)) {
        _rjson_goto_next_token(parser); // skip whitespace and goto next token
    }

    return number;
}


static inline rjson_string* _rjson_parse_name(_rjson_parser* parser) {

    _rjson_assert(*parser->cursor == '"');
    _rjson_consume_one(parser); // Skip only the '"'

    rjson_string* result = (rjson_string*)_rjson_new(parser, sizeof(rjson_string));

    const char* start = parser->cursor;

    if (*parser->cursor == '"') {
        result->ptr = 0;
        result->length = 0;
        _rjson_goto_next_token(parser);
        return result; // empty string termintated
    }

    while(*parser->cursor != '"') {

        if (*parser->cursor == '\\')
        {
            _rjson_consume_one(parser); // skip current char
            switch (*parser->cursor)
            {
            case '"': case '\\': case '/':  case 'b':
            case 'f': case 'n': case 'r': case 't': {
                _rjson_consume_one(parser);
                break;
            }
            case 'u': {
                _rjson_consume_one(parser); // skip 'u'
                _rjson_assert(_rjson_consume_4hex(parser));
                break;

            } // case 'u'

            default: { // unexpected escaped char
                return 0;
            }
            }
        } else // a simple ASCII char
        {
            _rjson_assert(!_rjson_is_control_char(*parser->cursor));

            _rjson_consume_one(parser);
        }

    }  // @TODO do we really need this function ?

    result->ptr = start;
    result->length = parser->cursor - start;

    //expect_and_consume(parser, '"');
    _rjson_assert(*parser->cursor == '"');
    _rjson_goto_next_token(parser);

    return result;
} // parse_name

static inline rjson_string* _rjson_parse_string(_rjson_parser* parser) {
    return _rjson_parse_name(parser);
} // parse_string



static inline rjson* _rjson_parse_value(_rjson_parser* parser) {


    D(printf("parse_value current %c...\n", *parser->cursor));
    rjson* result = (rjson*)_rjson_new(parser, sizeof (rjson));

    switch(*parser->cursor) {

    case '{':
        result->data = (void*)_rjson_parse_object(parser);
        result->type = REJ_OBJECT;
        break;
    case '[':{
        result->data = (void*)_rjson_parse_array(parser);
        result->type = REJ_ARRAY;
        break;
    }

    case '"': {
        result->data = (void*)_rjson_parse_string(parser);
        result->type = REJ_STRING;
        break;
    }


    case '-': case '+': case '.':
    case '0': case '1': case '2':
    case '3': case '4': case '5':
    case '6': case '7': case '8':
    case '9':
    {
        result->data = (void*)_rjson_parse_number(parser);
        result->type = REJ_NUMBER;
        break;
    }

        // @TODO optimize this 3 functions below
    case 't':
        if(strncmp(parser->cursor, "true", 4) == 0) {

            result->bool_value = 1;
            result->type = REJ_BOOL;

            for (int i = 0; i < 3; ++i)
                _rjson_consume_one(parser);
            _rjson_goto_next_token(parser);
        }
        break;

    case 'f':
        if(strncmp(parser->cursor, "false", 5) == 0) {
            result->bool_value = 0;
            result->type = REJ_BOOL;

            for (int i = 0; i < 4; ++i)
                _rjson_consume_one(parser);
            _rjson_goto_next_token(parser);
        }
        break;
    case 'n':
        if(strncmp(parser->cursor, "null", 4) == 0) {
            result->type = REJ_NULL;

            for (int i = 0; i < 3; ++i)
                _rjson_consume_one(parser);
            _rjson_goto_next_token(parser);
        }
        break;

    default:
        fprintf(stderr, "parse_value error, case not handled yet\n");
        result = 0;
    }

    return result;
} // parse_value


static inline rjson_array* _rjson_parse_array(_rjson_parser* parser) {


    D(printf("parse_array\n"));

    rjson_array* result = 0;

    ++parser->depth;

    _rjson_expect_and_next_token(parser, '[');

    if (*parser->cursor != ']') { // empty array

        rjson_array* current_array = 0;
        rjson* value = 0;

        while ((value = _rjson_parse_value(parser))) {


            rjson_array* new_array = (rjson_array*)_rjson_new(parser, sizeof(rjson_array));
            new_array->value = value;
            new_array->next = 0;

            if (!current_array) { // first element
                result = new_array;
            } else {
                current_array->next = new_array;
            }

            current_array = new_array;

            if (*parser->cursor == ']') {
                break;
            } else if (*parser->cursor == ',') {
                _rjson_goto_next_token(parser);

#ifdef RJSON_ALLOW_ENDING_COMMA
                if (*parser->cursor == ']') { // empty between ',' and ']', ends object.
                    break;
                }
#endif

            } else {
                result = 0;
                break;
            }
        }
    }

    --parser->depth;

    _rjson_expect_and_next_token(parser, ']');

    return result;
} // parse_array


static inline rjson_object* _rjson_parse_object(_rjson_parser* parser) {

    rjson_object* result = 0;

    _rjson_expect_and_next_token(parser, '{');

    ++parser->depth;

    if (*parser->cursor != '}') { // empty object, it's ok.


        rjson_object* current_obj = 0;
        rjson_string* name = 0;
        rjson* value = 0;
        do {
            name = _rjson_parse_name(parser);

            _rjson_expect_and_next_token(parser, ':');

            value = _rjson_parse_value(parser);

            if (name && value) {

                rjson_object* new_object = (rjson_object*)_rjson_new(parser, sizeof(rjson_object));

                new_object->name = name;
                new_object->value = value;
                new_object->next = 0;


                if (!current_obj) { // first element
                    result = new_object;
                } else {
                    current_obj->next = new_object;
                }

                current_obj = new_object;

                if (*parser->cursor == '}') {
                    break;
                } else if (*parser->cursor == ',') {
                    _rjson_goto_next_token(parser);

#ifdef RJSON_ALLOW_ENDING_COMMA

                    if (*parser->cursor == '}') { // empty between ',' and '}', ends object.
                        break;
                    }
#endif
                } else {
                    result = 0;
                    break;
                }
            } else {

                result = 0;
                break;
            }

        } while(name && value);

    }

    --parser->depth;

    _rjson_expect_and_next_token(parser, '}');

    return result;
} // parse_object


//-------------------------------------------------------------------------
// Str utils
//-------------------------------------------------------------------------

static inline rjson_bool_t _rjson_is_end_line(char c) {
    return (c == '\n' || c == '\r');
}

static inline rjson_bool_t _rjson_is_whitespace(char c) {
    return (c == ' ' || c == '\n'
            || c == '\t' || c == '\r'
            || c == '\f' || c == '\v');
}

static inline rjson_bool_t _rjson_is_digit(char c) {
    return ('0' <= c && c <= '9');
}

static inline rjson_bool_t _rjson_is_alpha_hex(char c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}

static inline rjson_bool_t _rjson_is_control_char(char c) {

    return c >= 0 && c < ' '; // @TODO find a better way
}

static inline int _rjson_get_precision(double x)
{
    int min = 1, max = 23;
    while (min < max)
    {
        char buf[30];
        int mid = (min + max) >> 1;
        sprintf(buf, "%.*g", mid, x);
        if (atof(buf) != x) {
            min = mid + 1;
        } else {
            max = mid;
        }
    }
    return min;
}


static inline void _rjson_print_string(FILE* f, const rjson_string* str) {

    fprintf(stdout, "\"%.*s\"\n", str->length, str->ptr);
    fprintf(f, "\"%.*s\"", str->length, str->ptr);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RE_RJSON_H
