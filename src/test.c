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
// #include "main.c"
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
            "-a",
            "-D",
            "-N",
            "-X100",
            "-Y200",
            "-Etest;Exclude",
            "-Turxvt -e"
        };
        int argc = 16;
        struct yamenu_app arguments = parse_args(argc, (char**)argv, YAMENU_NOX_DEFAULT);

        assert_string_equal(arguments.input_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
        assert_string_equal(arguments.prefix, "test");
        assert_string_equal(arguments.postfix, "postTest");
        assert_int_equal(arguments.log_level, 0);
        assert_string_equal(arguments.search_path, "testpath");
        assert_true(arguments.show_hidden);
        assert_true(arguments.base_name_only);
        assert_true(arguments.dry_run);
        assert_true(arguments.no_desktop_entry);
        assert_int_equal(arguments.x_pos, 100);
        assert_int_equal(arguments.y_pos, 200);
        assert_string_equal(arguments.excludes, "test;Exclude");
        assert_string_equal(arguments.term, "urxvt -e");

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
            "--all",
            "--dry",
            "--no-desktop-entry",
            "--x-pos=100",
            "--y-pos=200",
            "--exclude=test;Exclude",
            "--term=urxvt -e"
        };
        int argc = 16;
        struct yamenu_app arguments = parse_args(argc, (char**)argv, YAMENU_NOX_DEFAULT);

        assert_string_equal(arguments.input_list, "Path1;Path2;Path3");
        assert_true(arguments.nox);
        assert_int_equal(arguments.separator, '&');
        assert_string_equal(arguments.prefix, "test");
        assert_string_equal(arguments.postfix, "postTest");
        assert_int_equal(arguments.log_level, 0);
        assert_string_equal(arguments.search_path, "testpath");
        assert_true(arguments.show_hidden);
        assert_true(arguments.base_name_only);
        assert_true(arguments.dry_run);
        assert_true(arguments.no_desktop_entry);
        assert_int_equal(arguments.x_pos, 100);
        assert_int_equal(arguments.y_pos, 200);
        assert_string_equal(arguments.excludes, "test;Exclude");
        assert_string_equal(arguments.term, "urxvt -e");

        yamenu_app_free(&arguments);
    }
    {
        const char *argv[] = {
            "./test" // program name
        };
        int argc = 1;
        struct yamenu_app arguments = parse_args(argc, (char**)argv, YAMENU_NOX_DEFAULT);

        assert_null(arguments.input_list);
        assert_false(arguments.nox);
        assert_int_equal(arguments.separator, ';');
        assert_string_equal(arguments.prefix, "");
        assert_string_equal(arguments.postfix, "");
        assert_int_equal(arguments.log_level, 3);
        assert_string_equal(arguments.search_path, YAMENU_DEFAULT_SEARCH_PATH);
        assert_false(arguments.show_hidden);
        assert_false(arguments.base_name_only);
        assert_false(arguments.dry_run);
        assert_false(arguments.no_desktop_entry);
        assert_int_equal(arguments.x_pos, DEFAULT_X_Y_POS);
        assert_int_equal(arguments.y_pos, DEFAULT_X_Y_POS);
        assert_null(arguments.excludes);
        assert_string_equal(arguments.term, "xterm -e");

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

    // try cat
    linked_list *c1 = linked_list_create("p1");
    linked_list_push(c1, "p2");
    linked_list *c2 = linked_list_create("p3");
    linked_list_push(c2, "p4");
    linked_list *cat = linked_list_cat(c1, c2);

    assert_int_equal(linked_list_size(cat), 4);
    assert_string_equal(linked_list_get(cat, 0)->str, "p1");
    assert_string_equal(linked_list_get(cat, 1)->str, "p2");
    assert_string_equal(linked_list_get(cat, 2)->str, "p3");
    assert_string_equal(linked_list_get(cat, 3)->str, "p4");
    linked_list_free(cat);
}

// also tests unescape_str and shift_str_left
static void test_create_path_list(void **state) {
    char *pathlist = my_malloc(128);
    strncpy(pathlist, "Path;Second Path;Third Path;\\\"Escape;Quote;\"Quoted; List\";Escaped\\; split", 128);

    const char *expected[] = {"Path", "Second Path", "Third Path", "\"Escape", "Quote", "Quoted; List",
        "Escaped; split"};

    // generate a path list of size 3
    linked_list *paths = create_path_list(pathlist, ';', true, NULL);

    assert_int_equal(linked_list_size(paths), 7);
    for (int i = 0; i < linked_list_size(paths); i++) {
        assert_string_equal(linked_list_get(paths, i)->fp->path, expected[i]);
        file_path_free(linked_list_get(paths, i)->fp);
    }

    linked_list_free(paths);
    my_free(pathlist);
}

static void test_apply_excludes(void **state) {
    char *pathlist = my_malloc(128);
    strncpy(pathlist, "Path;Second Path;Third Path;\\\"Escape;Quote;\"Quoted; List\";Escaped\\; split", 128);
    char *excludelist = strdup("Third Path;\\\"Escape");

    const char *expected[] = {"Path", "Second Path", "Quote", "Quoted; List",
        "Escaped; split"};

    // generate a path list of size 3
    linked_list *paths = create_path_list(pathlist, ';', true, NULL);

    paths = apply_exclude_list(paths, excludelist, ';');

    assert_int_equal(linked_list_size(paths), 5);
    for (int i = 0; i < linked_list_size(paths); i++) {
        assert_string_equal(linked_list_get(paths, i)->fp->path, expected[i]);
        file_path_free(linked_list_get(paths, i)->fp);
    }

    linked_list_free(paths);
    my_free(pathlist);
    my_free(excludelist);
}

// dummy returns the same entry every time
linked_list *read_file_dummy(char *path) {
    linked_list *list = linked_list_create(strdup("[Desktop Entry]"));
    linked_list_push(list, strdup("Exec=Test"));
    linked_list_push(list, strdup("Name=Hello"));
    linked_list_push(list, strdup("NoDisplay=true"));
    linked_list_push(list, strdup("Terminal=true"));
    return list;
}

linked_list *read_file_not_found_dummy(char *path) {
    return NULL;
}

static void test_create_path_list_desktop_entry(void **state) {
    // not a .desktop file
    {
        file_path *fp = file_path_create("test/path", false, NULL);
        assert_string_equal(fp->path, "test/path");
        assert_null(fp->name);
        assert_null(fp->executable);
        assert_false(fp->no_show);
        assert_false(fp->terminal);
        file_path_free(fp);
    }
    {
        file_path *fp = file_path_create("test/path", true, read_file_dummy);
        assert_string_equal(fp->path, "test/path");
        assert_null(fp->name);
        assert_null(fp->executable);
        assert_false(fp->no_show);
        assert_false(fp->terminal);
        file_path_free(fp);
    }
    {
        file_path *fp = file_path_create("test/path", false, read_file_dummy);
        assert_string_equal(fp->path, "test/path");
        assert_null(fp->name);
        assert_null(fp->executable);
        assert_false(fp->no_show);
        assert_false(fp->terminal);
        file_path_free(fp);
    }

    // do not read file because no function was provided
    {
        file_path *fp = file_path_create("test/path.desktop", false, NULL);
        assert_string_equal(fp->path, "test/path.desktop");
        assert_null(fp->name);
        assert_null(fp->executable);
        assert_false(fp->no_show);
        assert_false(fp->terminal);
        file_path_free(fp);
    }
    // do not read file because of false
    {
        file_path *fp = file_path_create("test/path.desktop", true, read_file_dummy);
        assert_string_equal(fp->path, "test/path.desktop");
        assert_null(fp->name);
        assert_null(fp->executable);
        assert_false(fp->no_show);
        assert_false(fp->terminal);
        file_path_free(fp);
    }
    // read dummy file entry
    {
        file_path *fp = file_path_create("test/path.desktop", false, read_file_dummy);
        assert_string_equal(fp->path, "test/path.desktop");
        assert_string_equal(fp->name, "Hello");
        assert_string_equal(fp->executable, "Test");
        assert_true(fp->no_show);
        assert_true(fp->terminal);
        file_path_free(fp);
    }
    // file not found
    {
        file_path *fp = file_path_create("test/path.desktop", true, read_file_not_found_dummy);
        assert_string_equal(fp->path, "test/path.desktop");
        assert_null(fp->name);
        assert_null(fp->executable);
        assert_false(fp->no_show);
        assert_false(fp->terminal);
        file_path_free(fp);
    }
}

static void test_filter_path_list(void **state) {
    char *pathlist = my_malloc(64);
    strncpy(pathlist, "Not Path 1;Filter Path 2;Filter Path 3;Not Path 4", 64);

    // generate a path list of size 3
    linked_list *paths = create_path_list(pathlist, ';', true, NULL);


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
    // no shell
    {
        yamenu_app app;
        app.prefix = "prefix for app";
        app.postfix = "Test postfix";
        app.term = "xterm -e";
        app.nox = false;

        file_path *path = file_path_create("/test/path", false, NULL);

        char *to_exec = build_command(&app, path);

        assert_string_equal(" prefix for app /test/path Test postfix", to_exec);

        file_path_free(path);
        my_free(to_exec);
    }
    // shell and not nox
    {
        yamenu_app app;
        app.prefix = "prefix for app";
        app.postfix = "Test postfix";
        app.term = "xterm -e";
        app.nox = false;

        file_path *path = file_path_create("/test/path", false, NULL);
        path->terminal = true;

        char *to_exec = build_command(&app, path);

        assert_string_equal("xterm -e prefix for app /test/path Test postfix", to_exec);

        file_path_free(path);
        my_free(to_exec);
    }
    // shell mode and nox
    {
        yamenu_app app;
        app.prefix = "prefix for app";
        app.postfix = "Test postfix";
        app.term = "xterm -e";
        app.nox = true;

        file_path *path = file_path_create("/test/path", false, NULL);
        path->terminal = true;

        char *to_exec = build_command(&app, path);

        assert_string_equal(" prefix for app /test/path Test postfix", to_exec);

        file_path_free(path);
        my_free(to_exec);
    }
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
    {
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
    {
        char *t1 = fileext("test.txt");
        assert_string_equal(t1, ".txt");
        my_free(t1);

        char *t2 = fileext("test.txt.dat");
        assert_string_equal(t2, ".dat");
        my_free(t2);

        char *t3 = fileext("test_no_ext");
        assert_null(t3);

        char *this_dir = fileext(".");
        assert_string_equal(this_dir, ".");
        my_free(this_dir);

        char *parent_dir = fileext("..");
        assert_string_equal(parent_dir, "..");
        my_free(parent_dir);

        char *hidden = fileext(".hidden");
        assert_string_equal(hidden, ".hidden");
        my_free(hidden);

        assert_null(basefilename(NULL));
    }
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

static void test_linked_list_swap(void **state) {
    // make a list
    linked_list *list = linked_list_create("Test");
    linked_list_push(list, "2");
    linked_list_push(list, "3");

    // test oob swap
    assert_false(linked_list_swap(list, 0, 50));
    assert_false(linked_list_swap(list, 50, 1));

    // valid swap
    assert_string_equal(linked_list_get(list, 2)->str, "3");
    assert_string_equal(linked_list_get(list, 0)->str, "Test");
    assert_true(linked_list_swap(list, 0, 2));
    assert_string_equal(linked_list_get(list, 0)->str, "3");
    assert_string_equal(linked_list_get(list, 2)->str, "Test");

    linked_list_free(list);
}

// this also tests path_list_compare
static void test_string_sort_helper(void **state) {
    // equality
    assert_int_equal(string_sort_helper("test", "test"), 0);

    // b is greater than a
    assert_int_equal(string_sort_helper("BCE", "ABC"), 1);
    assert_int_equal(string_sort_helper("acc", "abc"), 1);

    // a is greater
    assert_int_equal(string_sort_helper("ABC", "BCE"), -1);
    assert_int_equal(string_sort_helper("abc", "acc"), -1);

    // equal but different lenght
    assert_int_equal(string_sort_helper("test", "testab"), 2);
    assert_int_equal(string_sort_helper("testab", "test"), -2);
}

// test compare function for quick sort test
int test_compare(linked_list *l1, linked_list *l2) {
    return *(int*)l1->generic - *(int*)l2->generic;
}

static void test_linked_list_quick_sort(void **state) {
    // attempt to sort a list of integers
    int arr[] = {10, 7, 8, 9, 1, 5, 5};

    // add to linked list
    linked_list *list = linked_list_create(&arr[0]);
    for (int i = 1; i < 7; i++) {
        linked_list_push(list, &arr[i]);
    }

    linked_list_quick_sort(list, 0, linked_list_size(list)-1, test_compare);

    int expected[] = {1, 5, 5, 7, 8, 9, 10};
    for (int i = 0; i < linked_list_size(list); i++) {
        assert_int_equal(*(int*)linked_list_get(list, i)->generic, expected[i]);
    }

    linked_list_free(list);
}

static void test_str_replace(void **state) {
    // test invalid token
    {
        char *str = str_replace("Test token", "key", "value");
        assert_null(str);
    }

    // shorter than original

    // in the middle of the string
    {
        char *str = str_replace("This is a test %% string", "%%", "%");
        assert_string_equal("This is a test % string", str);
        my_free(str);
    }
    // at the start
    {
        char *str = str_replace("%% This is a test string", "%%", "%");
        assert_string_equal("% This is a test string", str);
        my_free(str);
    }
    // end of string
    {
        char *str = str_replace("This is a test string %%", "%%", "%");
        assert_string_equal("This is a test string %", str);
        my_free(str);
    }

    // longer than original
    // in the middle of the string
    {
        char *str = str_replace("This is a test %f string", "%f", "Longer");
        assert_string_equal("This is a test Longer string", str);
        my_free(str);
    }
    // at the start
    {
        char *str = str_replace("%f This is a test string", "%f", "Longer");
        assert_string_equal("Longer This is a test string", str);
        my_free(str);
    }
    // end of string
    {
        char *str = str_replace("This is a test string %f", "%f", "Longer");
        assert_string_equal("This is a test string Longer", str);
        my_free(str);
    }

}

static void test_parse_desktop_entry(void **state) {
    // invalid entry
    linked_list *invalid_entry = linked_list_create("Not");
    linked_list_push(invalid_entry, "An");
    linked_list_push(invalid_entry, "Entry");
    assert_null(parse_desktop_entry("Application", invalid_entry));
    linked_list_free(invalid_entry);

    // valid entry
    {
        linked_list *valid_entry = linked_list_create("[Desktop Entry]");
        linked_list_push(valid_entry, "Exec=App %% %f %F %% %u %U %d %D %n %N %k %v");
        char *parsed = parse_desktop_entry("Exec=", valid_entry);
        assert_string_equal("App %   %        ", parsed);
        my_free(parsed);
        linked_list_free(valid_entry);
    }
    {
        linked_list *valid_entry = linked_list_create("# Comment");
        linked_list_push(valid_entry, "");
        linked_list_push(valid_entry, "[Desktop Entry]");
        linked_list_push(valid_entry, "Exec=App %% %f %F %% %u %U %d %D %n %N %k %v");
        char *parsed = parse_desktop_entry("Exec=", valid_entry);
        assert_string_equal("App %   %        ", parsed);
        my_free(parsed);
        linked_list_free(valid_entry);
    }
}

static void test_path_combine(void **state) {
    char *p1 = path_combine("/test/", "/append", '/');
    assert_string_equal("/test/append", p1);
    my_free(p1);

    char *p2 = path_combine("/test", "append", '/');
    assert_string_equal("/test/append", p2);
    my_free(p2);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_args),
        cmocka_unit_test(test_linked_list),
        cmocka_unit_test(test_create_path_list),
        cmocka_unit_test(test_apply_excludes),
        cmocka_unit_test(test_create_path_list_desktop_entry),
        cmocka_unit_test(test_filter_path_list),
        cmocka_unit_test(test_my_malloc_and_free),
        cmocka_unit_test(test_build_command),
        cmocka_unit_test(test_should_log),
        cmocka_unit_test(test_basefilename),
        cmocka_unit_test(test_strstr_last),
        cmocka_unit_test(test_linked_list_swap),
        cmocka_unit_test(test_string_sort_helper),
        cmocka_unit_test(test_linked_list_quick_sort),
        cmocka_unit_test(test_str_replace),
        cmocka_unit_test(test_parse_desktop_entry),
        cmocka_unit_test(test_path_combine)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
