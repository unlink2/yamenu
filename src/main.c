#include "include/utility.h"

#include "include/utility.h"
#include "include/data.h"
#include "include/commandline.h"

int main(int argc, char **argv) {
    yamenu_app app = parse_args(argc, argv);

    if (app.nox) {
        command_line_interface(&app);
    }

    yamenu_app_free(&app);

    return 0;
}
