#include "include/utility.h"

#include "include/utility.h"
#include "include/data.h"
#include "include/commandline.h"
#include "include/gui.h"
#include "include/sysio.h"

int main(int argc, char **argv) {
    yamenu_app app = parse_args(argc, argv, getenv("DISPLAY") == NULL);
    yamenu_app_init_paths(&app);

    if (app.nox) {
        command_line_interface(&app);
    } else {
        graphical_interface(argc, argv, &app);
    }

    yamenu_app_free(&app);

    return 0;
}
