#ifndef __GUI_H__
#define __GUI_H__

// uncomment to disable all GTK related code at compile time
// #define YAMENU_NO_GTK
#ifndef YAMENU_NO_GTK

#include "data.h"
#include <gtk/gtk.h>

int gui_interface(int argc, char **argv, yamenu_app *app);

void on_window_main_destroy();

void on_window_main_focus_out();

#endif
#endif 
