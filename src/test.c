#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "include/utility.h"
#include "include/data.h"
#include "include/logger.h"

#define main __real_main
#include "main.c"
#undef main

/**
 * The following files cannot be tested:
 *  commandline.c - the terminal interface
 *  gui.c - the graphical interface
 *  sysio.c - system IO functionality
 */

static void test_parse_args(void **state) {
    {
        const char *argv[] = {
            "./test", // program name
            "-s&",
            "-pPath1;Path2;Path3",
            "-n",
            "-Ptest",
            "-FpostTest",
            "-v",
            "-Stestpath",
            "-b",
            "-a"
        };
        int argc = 10;
        struct yamenu_app arguments = parse_args(argc, (char**)argv);

        assert_string_equal(arguments.input_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
        assert_string_equal(arguments.prefix, "test");
        assert_string_equal(arguments.postfix, "postTest");
        assert_int_equal(arguments.log_level, 0);
        assert_string_equal(arguments.search_path, "testpath");
        assert_true(arguments.show_hidden);
        assert_false(arguments.base_name_only);

        yamenu_app_free(&arguments);
    }
    {
        const char *argv[] = {
            "./test", // program name
            "--separator=&",
            "--path=Path1;Path2;Path3",
            "--nox",
            "--prefix=test",
            "--postfix=postTest",
            "--verbose",
            "--search=testpath",
            "--base",
            "--all"
        };
        int argc = 10;
        struct yamenu_app arguments = parse_args(argc, (char**)argv);

        assert_string_equal(arguments.input_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
        assert_string_equal(arguments.prefix, "test");
        assert_string_equal(arguments.postfix, "postTest");
        assert_int_equal(arguments.log_level, 0);
        assert_string_equal(arguments.search_path, "testpath");
        assert_true(arguments.show_hidden);
        assert_false(arguments.base_name_only);

        yamenu_app_free(&arguments);
    }
    {
        const char *argv[] = {
            "./test" // program name
        };
        int argc = 1;
        struct yamenu_app arguments = parse_args(argc, (char**)argv);

        assert_null(arguments.input_list);
        assert_false(arguments.nox);
        assert_int_equal(arguments.separator, ';');
        assert_string_equal(arguments.prefix, "");
        assert_string_equal(arguments.postfix, "");
        assert_int_equal(arguments.log_level, 3);
        assert_string_equal(arguments.search_path, YAMENU_DEFAULT_SEARCH_PATH);
        assert_false(arguments.show_hidden);
        assert_true(arguments.base_name_only);

        yamenu_app_free(&arguments);
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

static void test_my_malloc_and_free(void **state) {
    // my_malloc should memset to 0
    char *test = my_malloc(64);
    for (int i = 0; i < 64; i++) {
        assert_int_equal(test[i], 0);
    }
    my_free(test);
}

static void test_build_command(void **state) {
    yamenu_app app;
    app.prefix = "prefix for app";
    app.postfix = "Test postfix";

    file_path *path = file_path_create("/test/path");

    char *to_exec = build_command(&app, path);

    assert_string_equal("prefix for app /test/path Test postfix", to_exec);

    file_path_free(path);
    my_free(to_exec);
}

static void test_should_log(void **state) {
    assert_true(should_log(3, 0));
    assert_true(should_log(3, 1));
    assert_true(should_log(3, 2));

    assert_true(should_log(3, 3));

    assert_false(should_log(3, 4));
    assert_false(should_log(3, 5));
}

static void test_basefilename(void **state) {
    char *t1 = basefilename("test.txt");
    assert_string_equal(t1, "test");
    my_free(t1);

    char *t2 = basefilename("test.txt.dat");
    assert_string_equal(t2, "test.txt");
    my_free(t2);

    char *t3 = basefilename("test_no_ext");
    assert_string_equal(t3, "test_no_ext");
    my_free(t3);

    char *this_dir = basefilename(".");
    assert_string_equal(this_dir, ".");
    my_free(this_dir);

    char *parent_dir = basefilename("..");
    assert_string_equal(parent_dir, "..");
    my_free(parent_dir);

    char *hidden = basefilename(".hidden");
    assert_string_equal(hidden, ".hidden");
    my_free(hidden);

    assert_null(basefilename(NULL));
}

static void test_strstr_last(void **state) {
    assert_null(strstr_last("notfound", "404"));

    {
        char *res = strstr_last(".last.here", ".");
        assert_non_null(res);
        assert_string_equal(res, ".here");
    }
    {
        char *res = strstr_last(".last", ".");
        assert_non_null(res);
        assert_string_equal(res, ".last");
    }
    {
        char *res = strstr_last("this_is.test", ".");
        assert_non_null(res);
        assert_string_equal(res, ".test");
    }
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_args),
        cmocka_unit_test(test_linked_list),
        cmocka_unit_test(test_create_path_list),
        cmocka_unit_test(test_filter_path_list),
        cmocka_unit_test(test_my_malloc_and_free),
        cmocka_unit_test(test_build_command),
        cmocka_unit_test(test_should_log),
        cmocka_unit_test(test_basefilename),
        cmocka_unit_test(test_strstr_last)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
