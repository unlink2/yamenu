#include "include/gui.h"
#include <gdk/gdkkeysyms.h>
#include <libgen.h>

enum APP_LIST_COLS {
    PATH_COL,
    META_COL,
    APP_COL
};

void add_app_to_list(linked_list *paths, yamenu_app *app, GtkListStore *list_store, GtkTreeIter *iter) {
    gtk_list_store_append(list_store, iter);
    gtk_list_store_set(list_store, iter,
            PATH_COL, basename(paths->fp->path),
            META_COL, paths,
            APP_COL, app,
            -1);
}

int gui_interface(int argc, char **argv, yamenu_app *app) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkTreeIter iter;
    GtkListStore *app_list_store;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, MAIN_WIN_MODEL, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);

    // add items to list mode l
    app_list_store = (GtkListStore*)gtk_builder_get_object(builder, "appplication_list_model");

    linked_list *paths = app->path_list;
    while (paths) {
        add_app_to_list(paths, app, app_list_store, &iter);
        paths = linked_list_get(paths, 1);
    }


    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

// called when window is closed
void on_window_main_destroy() {
    gtk_main_quit();
}

void on_window_main_focus_out() {
    // quit as soon as the user removes focus from window
    gtk_main_quit();
}

void on_main_window_application_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column) {
    GtkTreeIter   iter;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model(tree_view);

    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gpointer meta;
        gpointer app;

        gtk_tree_model_get(model, &iter, META_COL, &meta, -1);
        gtk_tree_model_get(model, &iter, APP_COL, &app, -1);

        linked_list *path = (linked_list*)meta;
        execute_path((yamenu_app*)app, path->fp);
    }
    gtk_main_quit();
}

gboolean on_window_main_key_press_event(GtkWidget *widget, GdkEventKey *key, gpointer user_data) {
    switch (key->keyval) {
        case GDK_KEY_Escape:
            gtk_main_quit();
            return true;
    }
    return false;
}
