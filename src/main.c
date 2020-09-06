#include "include/utility.h"

#include "include/utility.h"
#include "include/data.h"
#include "include/commandline.h"
#include "include/gui.h"

int main(int argc, char **argv) {
    yamenu_app app = parse_args(argc, argv);

    if (app.nox) {
        command_line_interface(&app);
    } else {
        gui_interface(argc, argv, &app);
    }

    yamenu_app_free(&app);

    return 0;
}
