#include "include/commandline.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <ctype.h>
#include <sys/ioctl.h>

#include "include/sysio.h"
#define update() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", x, y)
#define start_of_line '\r'

#define STYLE_BOLD "\033[1m"
#define STYLE_NO_BOLD "\033[22m"
#define STYLE_REVERSE "\x1b[7m"
#define STYLE_RESET "\x1b[0m"

enum special_keys {
  KEY_ARROW_LEFT = 1001,
  KEY_ARROW_RIGHT,
  KEY_ARROW_UP,
  KEY_ARROW_DOWN
};
#define KEY_ESC 27
#define KEY_BS 127

#define INPUT_BUFFER_SIZE 128

volatile sig_atomic_t sigint_flag = 0;

void sigint_handler(int sig) {
    sigint_flag = 1;
}

void disable_raw_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

struct termios enable_raw_mode() {
    struct termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    return orig_termios;
}

int print_to_screen(char *str, int current_pos, int max_pos) {
    int len = strlen(str)+1;
    // will it fit?
    if (current_pos+len >= max_pos) {
        len = max_pos-current_pos-1;
    }
    if (len == 0) {
        return 0;
    }
    if (current_pos > 0) {
        putchar(' ');
    }
    printf("%*.*s", len, len, str);
    return len+1;
}

int read_char() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1 && !sigint_flag) {}
    if (sigint_flag) {
        return KEY_ESC;
    }

    // c = getchar();
    if (c == '\x1b') {
        char seq[3]; // read two additional bytes
        // if additional read times out return ESC
        if (read(STDIN_FILENO, &seq[0], 1) != 1) { return '\x1b'; }
        if (read(STDIN_FILENO, &seq[1], 1) != 1) { return '\x1b'; }
        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_ARROW_UP;
                case 'B': return KEY_ARROW_DOWN;
                case 'C': return KEY_ARROW_RIGHT;
                case 'D': return KEY_ARROW_LEFT;
            }
        }
        return '\x1b';
    } else {
        return (char)c;
    }
}

void output_all(linked_list *filtered, size_t filtered_len,
        int draws_this_iteration, int selection, int cols, char *input_buffer) {
    // start of screen
    printf("\r");
    draws_this_iteration = 0;
    draws_this_iteration += print_to_screen(input_buffer, draws_this_iteration, cols);
    // draw list until we're out of space
    for (size_t c = selection; c < filtered_len; c++) {
        if (c == selection) {
            printf(STYLE_REVERSE);
        }
        linked_list *cur = linked_list_get(filtered, c);
        int this_print = print_to_screen(file_path_get_real_name(cur->fp), draws_this_iteration, cols);
        draws_this_iteration += this_print;
        if (c == selection) {
            printf(STYLE_RESET);
        }
        if (this_print == 0 || draws_this_iteration >= cols-1) { break; }
    }

    while (draws_this_iteration < cols) {
        printf(" ");
        draws_this_iteration++;
    }
    fflush(stdout);
}

void command_line_interface(yamenu_app *app) {
    if (!app->path_list) {
        fprintf(stderr, "No input paths\n");
        return;
    }

    // get term size
    struct winsize w;

    signal(SIGINT, sigint_handler);

    struct termios orgi_termios = enable_raw_mode();

    size_t selection = 0;
    // where is the input right now
    size_t input_buffer_index = 0;
    size_t draws_this_iteration = 0;

    char input_buffer[INPUT_BUFFER_SIZE];
    memset(input_buffer, 0, INPUT_BUFFER_SIZE);

    linked_list *filtered = filter_path_list(app->path_list, "");
    size_t filtered_len = linked_list_size(filtered);

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    output_all(filtered, filtered_len, draws_this_iteration, selection, w.ws_col, input_buffer);
    int c;
    while ((c = read_char())) {
        // get new terminal size. this takes care of resize events (hopefully)
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        size_t cols = w.ws_col;
        // input
        if (c == '\n') {
            break;
        } else if (c == KEY_ESC || sigint_flag == 1 || c == '\0') {
            disable_raw_mode(&orgi_termios);
            return;
        } else if (c == KEY_BS) {
            if (input_buffer_index > 0) {
                input_buffer[--input_buffer_index] = '\0';
                // re-filtered
                linked_list_free(filtered);
                filtered = filter_path_list(app->path_list, input_buffer);
                filtered_len = linked_list_size(filtered);
                selection = 0;
            }
        } else if (isprint(c)) {
            if (input_buffer_index < INPUT_BUFFER_SIZE) {
                input_buffer[input_buffer_index++] = c;
                // re-filtered
                linked_list_free(filtered);
                filtered = filter_path_list(app->path_list, input_buffer);
                filtered_len = linked_list_size(filtered);
                selection = 0;
            }
        } else {
            switch (c) {
                case KEY_ARROW_LEFT:
                    if (selection > 0) {
                        selection--;
                    }
                    break;
                case KEY_ARROW_RIGHT:
                    if (selection < filtered_len-1) {
                        selection++;
                    }
                    break;
            }
        }
        output_all(filtered, filtered_len, draws_this_iteration, selection, cols, input_buffer);
    }


    /* linked_list *filtered = filter_path_list(app->path_list, input_buffer);
    for (size_t i = 0; i < linked_list_size(filtered); i++) {
        linked_list *l = linked_list_get(filtered, i);
        if (!l->fp->no_show) {
            printf("%ld) %s\n", i+1, file_path_get_real_name(l->fp));
        }
    }

    fgets(input_buffer, INPUT_BUFFER_SIZE, stdin);

    selection = atoi(input_buffer);*/
    linked_list *path_list = linked_list_get(filtered, selection);
    disable_raw_mode(&orgi_termios);
    printf("\n");
    if (path_list) {
        execute_path(app, path_list->fp);
    }

    linked_list_free(filtered);

}
