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
            "-n"
        };
        int argc = 4;
        struct arguments arguments = parse_args(argc, (char**)argv);

        assert_string_equal(arguments.path_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
    }
    {
        const char *argv[] = {
            "./test", // program name
            "--separator=&",
            "--path=Path1;Path2;Path3",
            "--nox"
        };
        int argc = 4;
        struct arguments arguments = parse_args(argc, (char**)argv);

        assert_string_equal(arguments.path_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
    }
    {
        const char *argv[] = {
            "./test", // program name
        };
        int argc = 1;
        struct arguments arguments = parse_args(argc, (char**)argv);

        assert_string_equal(arguments.path_list, "");
        assert_false(arguments.nox);
        assert_int_equal(arguments.separator, ';');
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
    assert_string_equal(linked_list_get(list, 0)->path, "Hello World");
    for (int i = 0; i < itemsc; i++) {
        assert_string_equal(linked_list_get(list, i+1)->path, items[i]);
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
        assert_string_equal(linked_list_get(paths, i)->path, expected[i]);
    }

    linked_list_free(paths);
    my_free(pathlist);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_args),
        cmocka_unit_test(test_linked_list),
        cmocka_unit_test(test_create_path_list)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
