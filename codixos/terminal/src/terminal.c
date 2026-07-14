/*
 * CodixOS Terminal Emulator Implementation
 * Full-featured terminal emulator for CodixOS
 */

#include "../include/terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

/* Terminal state */
static terminal_t terminal;
static struct termios original_termios;

/* Initialize terminal */
void terminal_init() {
    /* Get terminal size */
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        terminal.width = ws.ws_col;
        terminal.height = ws.ws_row;
    } else {
        terminal.width = TERMINAL_WIDTH;
        terminal.height = TERMINAL_HEIGHT;
    }
    
    /* Initialize state */
    terminal.cursor_x = 0;
    terminal.cursor_y = 0;
    terminal.foreground = TERM_COLOR_WHITE;
    terminal.background = TERM_COLOR_BLACK;
    terminal.bold = 0;
    terminal.underline = 0;
    terminal.blink = 0;
    terminal.reverse = 0;
    terminal.hidden = 0;
    terminal.tab_size = 8;
    terminal.wrap = 1;
    terminal.insert_mode = 0;
    terminal.auto_wrap = 1;
    terminal.origin_mode = 0;
    terminal.cursor_visible = 1;
    terminal.cursor_shape = 0;
    
    /* Set title */
    strcpy(terminal.title, TERMINAL_TITLE);
    
    /* Allocate buffer */
    terminal.buffer_size = terminal.width * terminal.height * 4;
    terminal.buffer = (char*)malloc(terminal.buffer_size);
    if (terminal.buffer) {
        memset(terminal.buffer, 0, terminal.buffer_size);
    }
    
    /* Set scroll region */
    terminal.scroll_top = 0;
    terminal.scroll_bottom = terminal.height - 1;
    terminal.scroll_lines = 0;
    
    /* Save terminal settings */
    tcgetattr(STDIN_FILENO, &original_termios);
    
    /* Set raw mode */
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
    /* Clear screen */
    terminal_clear();
    
    /* Enable alternate screen buffer */
    printf("\033[?1049h");
    fflush(stdout);
}

/* Cleanup terminal */
void terminal_cleanup() {
    /* Restore terminal settings */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
    
    /* Disable alternate screen buffer */
    printf("\033[?1049l");
    fflush(stdout);
    
    /* Free buffer */
    if (terminal.buffer) {
        free(terminal.buffer);
    }
}

/* Clear terminal */
void terminal_clear() {
    printf("\033[2J");
    printf("\033[H");
    terminal.cursor_x = 0;
    terminal.cursor_y = 0;
    fflush(stdout);
}

/* Reset terminal */
void terminal_reset() {
    printf("\033c");
    fflush(stdout);
}

/* Set cursor position */
void terminal_set_cursor(int x, int y) {
    terminal.cursor_x = x;
    terminal.cursor_y = y;
    printf("\033[%d;%dH", y + 1, x + 1);
    fflush(stdout);
}

/* Get cursor position */
void terminal_get_cursor(int* x, int* y) {
    *x = terminal.cursor_x;
    *y = terminal.cursor_y;
}

/* Move cursor */
void terminal_move_cursor(int dx, int dy) {
    terminal.cursor_x += dx;
    terminal.cursor_y += dy;
    
    /* Handle wrapping */
    if (terminal.cursor_x >= terminal.width) {
        terminal.cursor_x = 0;
        terminal.cursor_y++;
    }
    if (terminal.cursor_x < 0) {
        terminal.cursor_x = terminal.width - 1;
        terminal.cursor_y--;
    }
    if (terminal.cursor_y >= terminal.height) {
        terminal.scroll_up(1);
        terminal.cursor_y = terminal.height - 1;
    }
    if (terminal.cursor_y < 0) {
        terminal.cursor_y = 0;
    }
    
    terminal_set_cursor(terminal.cursor_x, terminal.cursor_y);
}

/* Show cursor */
void terminal_show_cursor() {
    terminal.cursor_visible = 1;
    printf("\033[?25h");
    fflush(stdout);
}

/* Hide cursor */
void terminal_hide_cursor() {
    terminal.cursor_visible = 0;
    printf("\033[?25l");
    fflush(stdout);
}

/* Print character */
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal.cursor_x = 0;
        terminal.cursor_y++;
    } else if (c == '\r') {
        terminal.cursor_x = 0;
    } else if (c == '\t') {
        terminal.tab();
    } else if (c == '\b') {
        if (terminal.cursor_x > 0) {
            terminal.cursor_x--;
        }
    } else if (c == '\033') {
        /* Escape sequence - handled separately */
        return;
    } else {
        /* Print character */
        putchar(c);
        terminal.cursor_x++;
        
        /* Handle line wrapping */
        if (terminal.cursor_x >= terminal.width && terminal.auto_wrap) {
            terminal.cursor_x = 0;
            terminal.cursor_y++;
        }
    }
    
    /* Handle scroll */
    if (terminal.cursor_y >= terminal.height) {
        terminal.scroll_up(1);
        terminal.cursor_y = terminal.height - 1;
    }
    
    /* Update cursor position */
    terminal_set_cursor(terminal.cursor_x, terminal.cursor_y);
}

/* Print string */
void terminal_puts(const char* str) {
    while (*str) {
        terminal_putchar(*str++);
    }
}

/* Print formatted string */
void terminal_print(const char* str) {
    terminal_puts(str);
}

/* Printf-style output */
void terminal_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

/* Set color */
void terminal_set_color(int fg, int bg) {
    terminal.foreground = fg;
    terminal.background = bg;
    printf("\033[%d;%dm", fg + 30, bg + 40);
    fflush(stdout);
}

/* Set foreground color */
void terminal_set_foreground(int color) {
    terminal.foreground = color;
    printf("\033[%dm", color + 30);
    fflush(stdout);
}

/* Set background color */
void terminal_set_background(int color) {
    terminal.background = color;
    printf("\033[%dm", color + 40);
    fflush(stdout);
}

/* Reset color */
void terminal_reset_color() {
    terminal.foreground = TERM_COLOR_WHITE;
    terminal.background = TERM_COLOR_BLACK;
    printf("\033[0m");
    fflush(stdout);
}

/* Set bold */
void terminal_set_bold(int enable) {
    terminal.bold = enable;
    if (enable) {
        printf("\033[1m");
    } else {
        printf("\033[22m");
    }
    fflush(stdout);
}

/* Set underline */
void terminal_set_underline(int enable) {
    terminal.underline = enable;
    if (enable) {
        printf("\033[4m");
    } else {
        printf("\033[24m");
    }
    fflush(stdout);
}

/* Set blink */
void terminal_set_blink(int enable) {
    terminal.blink = enable;
    if (enable) {
        printf("\033[5m");
    } else {
        printf("\033[25m");
    }
    fflush(stdout);
}

/* Set reverse */
void terminal_set_reverse(int enable) {
    terminal.reverse = enable;
    if (enable) {
        printf("\033[7m");
    } else {
        printf("\033[27m");
    }
    fflush(stdout);
}

/* Set hidden */
void terminal_set_hidden(int enable) {
    terminal.hidden = enable;
    if (enable) {
        printf("\033[8m");
    } else {
        printf("\033[28m");
    }
    fflush(stdout);
}

/* Scroll up */
void terminal_scroll_up(int lines) {
    printf("\033[%dS", lines);
    fflush(stdout);
}

/* Scroll down */
void terminal_scroll_down(int lines) {
    printf("\033[%dT", lines);
    fflush(stdout);
}

/* Scroll to line */
void terminal_scroll_to(int line) {
    printf("\033[%d;%dr", line, terminal.height);
    fflush(stdout);
}

/* Erase line */
void terminal_erase_line(int mode) {
    printf("\033[%dK", mode);
    fflush(stdout);
}

/* Erase display */
void terminal_erase_display(int mode) {
    printf("\033[%dJ", mode);
    fflush(stdout);
}

/* Erase characters */
void terminal_erase_chars(int count) {
    printf("\033[%dX", count);
    fflush(stdout);
}

/* Get key */
int terminal_get_key() {
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}

/* Get key non-blocking */
int terminal_get_key_nonblock() {
    fd_set fds;
    struct timeval tv;
    
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        char c;
        read(STDIN_FILENO, &c, 1);
        return c;
    }
    
    return -1;
}

/* Read line */
char* terminal_read_line(char* buffer, int max_len) {
    int pos = 0;
    int c;
    
    while (1) {
        c = terminal_get_key();
        
        if (c == '\n' || c == '\r') {
            buffer[pos] = '\0';
            terminal_puts("\r\n");
            return buffer;
        } else if (c == 127 || c == '\b') {
            if (pos > 0) {
                pos--;
                terminal_puts("\b \b");
            }
        } else if (c == 27) {
            /* Escape sequence */
            char seq[3];
            seq[0] = terminal_get_key();
            seq[1] = terminal_get_key();
            
            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': /* Up arrow */
                        break;
                    case 'B': /* Down arrow */
                        break;
                    case 'C': /* Right arrow */
                        break;
                    case 'D': /* Left arrow */
                        break;
                }
            }
        } else if (c == 3) {
            /* Ctrl+C */
            buffer[0] = '\0';
            terminal_puts("^C\r\n");
            return buffer;
        } else if (c == 4) {
            /* Ctrl+D */
            buffer[0] = '\0';
            terminal_puts("\r\n");
            return buffer;
        } else if (c >= 32 && pos < max_len - 1) {
            buffer[pos++] = c;
            terminal_putchar(c);
        }
    }
}

/* Set title */
void terminal_set_title(const char* title) {
    strcpy(terminal.title, title);
    printf("\033]0;%s\007", title);
    fflush(stdout);
}

/* Set icon name */
void terminal_set_icon_name(const char* name) {
    printf("\033]1;%s\007", name);
    fflush(stdout);
}

/* Set tab size */
void terminal_set_tab_size(int size) {
    terminal.tab_size = size;
}

/* Tab */
void terminal_tab() {
    int spaces = terminal.tab_size - (terminal.cursor_x % terminal.tab_size);
    for (int i = 0; i < spaces; i++) {
        terminal_putchar(' ');
    }
}

/* Resize terminal */
void terminal_resize(int width, int height) {
    terminal.width = width;
    terminal.height = height;
    
    /* Reallocate buffer */
    terminal.buffer_size = width * height * 4;
    terminal.buffer = (char*)realloc(terminal.buffer, terminal.buffer_size);
    
    printf("\033[8;%d;%dt", height, width);
    fflush(stdout);
}

/* Fullscreen mode */
void terminal_fullscreen() {
    printf("\033[?1049h");
    fflush(stdout);
}

/* Windowed mode */
void terminal_windowed() {
    printf("\033[?1049l");
    fflush(stdout);
}

/* Main function */
int main(int argc, char** argv) {
    terminal_init();
    
    terminal_set_title("CodixOS Terminal");
    
    terminal_set_foreground(TERM_COLOR_GREEN);
    terminal_puts("Welcome to CodixOS Terminal!\n");
    terminal_reset_color();
    
    terminal_puts("Type 'help' for commands, 'exit' to quit.\n\n");
    
    char input[256];
    while (1) {
        terminal_set_foreground(TERM_COLOR_CYAN);
        terminal_puts("codix@terminal:~$ ");
        terminal_reset_color();
        
        if (terminal_read_line(input, sizeof(input)) == NULL) {
            break;
        }
        
        if (strlen(input) == 0) continue;
        
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            break;
        } else if (strcmp(input, "help") == 0) {
            terminal_puts("Available commands:\n");
            terminal_puts("  help     - Show this help\n");
            terminal_puts("  clear    - Clear screen\n");
            terminal_puts("  echo     - Print text\n");
            terminal_puts("  exit     - Exit terminal\n");
        } else if (strcmp(input, "clear") == 0) {
            terminal_clear();
        } else if (strncmp(input, "echo ", 5) == 0) {
            terminal_puts(input + 5);
            terminal_puts("\n");
        } else {
            terminal_set_foreground(TERM_COLOR_RED);
            terminal_printf("Unknown command: %s\n", input);
            terminal_reset_color();
        }
    }
    
    terminal_cleanup();
    return 0;
}
