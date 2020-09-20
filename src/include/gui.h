#ifndef __GUI_H__
#define __GUI_H__

#include "data.h"

// uncomment to disable all GTK related code at compile time
// #define YAMENU_NO_GTK
#ifndef YAMENU_NO_GTK



#ifdef DEBUG
#define MAIN_WIN_VIEW "view/window_main.glade"
#else
#define MAIN_WIN_VIEW "/usr/local/bin/view/window_main.glade"
#endif

#include <gtk/gtk.h>

void on_window_main_destroy();

void on_window_main_focus_out();

void on_main_window_application_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column);

gboolean on_window_main_key_press_event(GtkWidget *widget, GdkEventKey *key, gpointer user_data);

#endif

int graphical_interface(int argc, char **argv, yamenu_app *app);

#endif 
