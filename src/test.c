#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "include/utility.h"
#include "include/data.h"

#define main __real_main
#include "main.c"
#undef main

static void test_parse_args(void **state) {
    {
        const char *argv[] = {
            "./test", // program name
            "-s&",
            "-pPath1;Path2;Path3",
            "-n",
            "-Ptest"
        };
        int argc = 5;
        struct yamenu_app arguments = parse_args(argc, (char**)argv);

        assert_string_equal(arguments.input_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
        assert_string_equal(arguments.prefix, "test");
        yamenu_app_free(&arguments);
    }
    {
        const char *argv[] = {
            "./test", // program name
            "--separator=&",
            "--path=Path1;Path2;Path3",
            "--nox",
            "--prefix=test"
        };
        int argc = 5;
        struct yamenu_app arguments = parse_args(argc, (char**)argv);

        assert_string_equal(arguments.input_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
        yamenu_app_free(&arguments);
        assert_string_equal(arguments.prefix, "test");
    }
    {
        const char *argv[] = {
            "./test", // program name
        };
        int argc = 1;
        struct yamenu_app arguments = parse_args(argc, (char**)argv);

        assert_null(arguments.input_list);
        assert_false(arguments.nox);
        assert_int_equal(arguments.separator, ';');
        yamenu_app_free(&arguments);
        assert_string_equal(arguments.prefix, "");
    }
}

static void test_linked_list(void **state) {
    linked_list *list = linked_list_create("Hello World");

    assert_int_equal(linked_list_size(list), 1);

    char *items[] = {
        "Test1",
        "Test2",
        "Test3"
    };
    int itemsc = 3;

    for (int i = 0; i < itemsc; i++) {
        linked_list_push(list, items[i]);
    }

    assert_int_equal(linked_list_size(list), 4);
    assert_string_equal(linked_list_get(list, 0)->str, "Hello World");
    for (int i = 0; i < itemsc; i++) {
        assert_string_equal(linked_list_get(list, i+1)->str, items[i]);
    }
    assert_null(linked_list_get(list, 5));

    linked_list_free(list);

    // try push and pop
    assert_null(linked_list_pop(NULL));

    list = linked_list_create("Pop test");
    linked_list *pheadexpected = linked_list_pop(list);
    assert_ptr_equal(list, pheadexpected);

    // push some values
    linked_list *p1expected = linked_list_push(list, "P1");
    linked_list *p2expected = linked_list_push(list, "P2");

    assert_non_null(p1expected);
    assert_non_null(p2expected);

    assert_int_equal(linked_list_size(list), 3);
    linked_list *p2 = linked_list_pop(list);
    assert_ptr_equal(p2, p2expected);
    assert_int_equal(linked_list_size(list), 2);

    linked_list *p1 = linked_list_pop(list);
    assert_ptr_equal(p1, p1expected);
    assert_int_equal(linked_list_size(list), 1);

    linked_list *phead = linked_list_pop(list);
    assert_ptr_equal(phead, pheadexpected);
    assert_int_equal(linked_list_size(list), 1);

    linked_list_free(phead);
    linked_list_free(p1);
    linked_list_free(p2);
}

static void test_create_path_list(void **state) {
    char *pathlist = my_malloc(64);
    strncpy(pathlist, "Path;Second Path;Third Path", 64);

    const char *expected[] = {"Path", "Second Path", "Third Path"};

    // generate a path list of size 3
    linked_list *paths = create_path_list(pathlist, ';');

    assert_int_equal(linked_list_size(paths), 3);
    for (int i = 0; i < linked_list_size(paths); i++) {
        assert_string_equal(linked_list_get(paths, i)->fp->path, expected[i]);
        file_path_free(linked_list_get(paths, i)->fp);
    }

    linked_list_free(paths);
    my_free(pathlist);
}

static void test_filter_path_list(void **state) {
    char *pathlist = my_malloc(64);
    strncpy(pathlist, "Not Path 1;Filter Path 2;Filter Path 3;Not Path 4", 64);

    // generate a path list of size 3
    linked_list *paths = create_path_list(pathlist, ';');


    assert_int_equal(linked_list_size(paths), 4);

    // filter for the string 'Filter'
    linked_list *filtered = filter_path_list(paths, "Filter");
    assert_int_equal(linked_list_size(filtered), 2);

    const char *expected[] = {"Filter Path 2", "Filter Path 3"};
    for (int i = 0; i < linked_list_size(filtered); i++) {
        assert_string_equal(linked_list_get(filtered, i)->fp->path, expected[i]);
    }

    // cleanup
    for (int i = 0; i < linked_list_size(paths); i++) {
        file_path_free(linked_list_get(paths, i)->fp);
    }
    linked_list_free(paths);
    my_free(pathlist);
    linked_list_free(filtered);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_args),
        cmocka_unit_test(test_linked_list),
        cmocka_unit_test(test_create_path_list),
        cmocka_unit_test(test_filter_path_list)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
