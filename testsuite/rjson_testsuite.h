
#include "string.h"
#include "assert.h"

#include "../rjson.h"
#include "../runit.h"

// Helpers
void* _load_file_to_memory(size_t* data_size, const char* filename, const char* open_mode, size_t padding_bytes);
void print_dstr(const dstr* s);

// Tests
void rjson_simple_parse_test();
void rjson_write_file();
void rjson_get_values_from_object();
void rjson_get_values_from_array();

void rjson_testsuite() {

    printf("rjson_testsuite\n");

    rjson_simple_parse_test();

    //FILE* file = tmpfile();
    const char* filename = "testsuite/_complex.json";


    size_t size;
    char* file_content = (char*)_load_file_to_memory(&size, filename, "rb", 1);

    if (!file_content) {
        fprintf(stdout, "re_json_test error: can't load json file '%s'\n", filename);
        return;
    }
    enum {
        BYTE_SIZE = 256*16
    };

    char bytes[BYTE_SIZE];
    rjson* root;
    root = rjson_parse(bytes, BYTE_SIZE, file_content, size);
    if (!root) {
        fprintf(stdout, "re_json_test error: can't parse json file '%s'\n", filename);
        return;
    }

    const char* out_filename = "testsuite/_complex_.json";
    FILE* out_file = fopen(out_filename, "w");

    if (!out_file) {
        fprintf(stderr, "re_json_test error: can't open '%s'\n", out_filename);
        return;
    }

    // rjson_write_in_file(out_file, root);

    fclose(out_file);

    free(file_content);

}

void rjson_simple_parse_test() {
    const char* json_obj_txt = "{ \"name\" : 0.42 }";
    const char* json_arr_txt = "[ \"name\" , 0.42 ]";

    enum {
        MEM_SIZE = 2048
    };

    char mem[MEM_SIZE];

    // Object
    {
        rjson* value_obj;
        RUNIT_ASSERT(value_obj = rjson_parse(mem, MEM_SIZE, json_obj_txt, strlen(json_obj_txt)));
        RUNIT_ASSERT(value_obj->type == RJ_OBJECT);
        rjson_object* obj =  (rjson_object*)value_obj->data;
        RUNIT_ASSERT(strncmp(obj->name->ptr, "name", 4) == 0);

        RUNIT_ASSERT(obj->value->type == RJ_NUMBER);
        rjson_number* number = rjson_obj_number(obj);

        RUNIT_ASSERT(number->value.d == 0.42);
    }

    // Array
    {
        rjson* value_arr;
        RUNIT_ASSERT(value_arr = rjson_parse(mem, MEM_SIZE, json_arr_txt, strlen(json_arr_txt)));
        RUNIT_ASSERT(value_arr->type == RJ_ARRAY);

    }
}

void* _load_file_to_memory(size_t* data_size, const char* filename, const char* open_mode, size_t padding_bytes)
{
    assert(filename && open_mode);

    if (data_size) {
        *data_size = 0;
    }

    FILE* f;
    if (!(f = fopen(filename, open_mode))) {
        return 0;
    }

    long file_size;
    if (fseek(f, 0, SEEK_END) || (file_size = ftell(f)) == -1L || fseek(f, 0, SEEK_SET)) {
        fclose(f);
        return 0;
    }

    void* file_content = malloc((size_t)file_size + padding_bytes);

    if (!file_content) {
        fclose(f);
        return 0;
    }

    // size may differ (for example if we put "w" mode instead of "wb" on windows
    if (fread(file_content, 1, (size_t)file_size, f) != (size_t)file_size) {
        fclose(f);
        free(file_content);
        return 0;
    }

    if (padding_bytes > 0) {
        memset((void *)((char*)file_content + file_size), 0, padding_bytes);
    }

    fclose(f);

    if (data_size) {
        *data_size = file_size;
    }

    return file_content;
}
