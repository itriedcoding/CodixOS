/*
 * CodixOS Terminal Emulator Header
 */

#ifndef TERMINAL_H
#define TERMINAL_H

/* Terminal configuration */
#define TERMINAL_WIDTH   80
#define TERMINAL_HEIGHT  25
#define TERMINAL_TITLE   "CodixOS Terminal"

/* Terminal colors */
#define TERM_COLOR_BLACK      0
#define TERM_COLOR_RED        1
#define TERM_COLOR_GREEN      2
#define TERM_COLOR_YELLOW     3
#define TERM_COLOR_BLUE       4
#define TERM_COLOR_MAGENTA    5
#define TERM_COLOR_CYAN       6
#define TERM_COLOR_WHITE      7
#define TERM_COLOR_BRIGHT_BLACK  8
#define TERM_COLOR_BRIGHT_RED    9
#define TERM_COLOR_BRIGHT_GREEN  10
#define TERM_COLOR_BRIGHT_YELLOW 11
#define TERM_COLOR_BRIGHT_BLUE   12
#define TERM_COLOR_BRIGHT_MAGENTA 13
#define TERM_COLOR_BRIGHT_CYAN   14
#define TERM_COLOR_BRIGHT_WHITE  15

/* Terminal state */
typedef struct {
    int width;
    int height;
    int cursor_x;
    int cursor_y;
    int foreground;
    int background;
    int bold;
    int underline;
    int blink;
    int reverse;
    int hidden;
    char title[256];
    char* buffer;
    int buffer_size;
    int scroll_top;
    int scroll_bottom;
    int scroll_lines;
    int tab_size;
    int wrap;
    int insert_mode;
    int auto_wrap;
    int origin_mode;
    int cursor_visible;
    int cursor_shape;
} terminal_t;

/* Terminal functions */
void terminal_init();
void terminal_cleanup();
void terminal_clear();
void terminal_reset();

/* Cursor functions */
void terminal_set_cursor(int x, int y);
void terminal_get_cursor(int* x, int* y);
void terminal_move_cursor(int dx, int dy);
void terminal_show_cursor();
void terminal_hide_cursor();

/* Output functions */
void terminal_putchar(char c);
void terminal_puts(const char* str);
void terminal_print(const char* str);
void terminal_printf(const char* format, ...);

/* Color functions */
void terminal_set_color(int fg, int bg);
void terminal_set_foreground(int color);
void terminal_set_background(int color);
void terminal_reset_color();

/* Style functions */
void terminal_set_bold(int enable);
void terminal_set_underline(int enable);
void terminal_set_blink(int enable);
void terminal_set_reverse(int enable);
void terminal_set_hidden(int enable);

/* Screen functions */
void terminal_scroll_up(int lines);
void terminal_scroll_down(int lines);
void terminal_scroll_to(int line);
void terminal_erase_line(int mode);
void terminal_erase_display(int mode);
void terminal_erase_chars(int count);

/* Input functions */
int terminal_get_key();
int terminal_get_key_nonblock();
char* terminal_read_line(char* buffer, int max_len);

/* Title functions */
void terminal_set_title(const char* title);
void terminal_set_icon_name(const char* name);

/* Tab functions */
void terminal_set_tab_size(int size);
void terminal_tab();

/* Window functions */
void terminal_resize(int width, int height);
void terminal_fullscreen();
void terminal_windowed();

#endif /* TERMINAL_H */
