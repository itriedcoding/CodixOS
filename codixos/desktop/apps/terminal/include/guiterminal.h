/*
 * CodixOS GUI Terminal Header
 * Full-featured terminal emulator with GUI
 */

#ifndef GUITERMINAL_H
#define GUITERMINAL_H

/* Terminal configuration */
#define TERM_DEFAULT_WIDTH    800
#define TERM_DEFAULT_HEIGHT   600
#define TERM_MIN_WIDTH        400
#define TERM_MIN_HEIGHT       300
#define TERM_DEFAULT_COLS     80
#define TERM_DEFAULT_ROWS     24
#define TERM_FONT_SIZE        14
#define TERM_CURSOR_BLINK_MS  500

/* Colors (Catppuccin Mocha) */
#define TERM_BG_COLOR         0x1E1E2E
#define TERM_FG_COLOR         0xCDD6F4
#define TERM_CURSOR_COLOR     0xF5E0DC
#define TERM_SELECTION_COLOR  0x585B70
#define TERM_BLACK            0x45475A
#define TERM_RED              0xF38BA8
#define TERM_GREEN            0xA6E3A1
#define TERM_YELLOW           0xF9E2AF
#define TERM_BLUE             0x89B4FA
#define TERM_MAGENTA          0xF5C2E7
#define TERM_CYAN             0x94E2D5
#define TERM_WHITE            0xBAC2DE
#define TERM_BRIGHT_BLACK     0x585B70
#define TERM_BRIGHT_RED       0xF38BA8
#define TERM_BRIGHT_GREEN     0xA6E3A1
#define TERM_BRIGHT_YELLOW    0xF9E2AF
#define TERM_BRIGHT_BLUE      0x89B4FA
#define TERM_BRIGHT_MAGENTA   0xF5C2E7
#define TERM_BRIGHT_CYAN      0x94E2D5
#define TERM_BRIGHT_WHITE     0xA6ADC8

/* Terminal cell */
typedef struct {
    char character;
    int fg_color;
    int bg_color;
    int bold;
    int underline;
    int italic;
    int strikethrough;
    int reverse;
} TerminalCell;

/* Terminal buffer */
typedef struct {
    TerminalCell* cells;
    int cols;
    int rows;
    int scrollback_lines;
    int scroll_top;
    int scroll_bottom;
    int scroll_offset;
} TerminalBuffer;

/* Terminal state */
typedef struct {
    int id;
    int x, y, width, height;
    int cols, rows;
    int cursor_x, cursor_y;
    int cursor_visible;
    int cursor_blink;
    int cursor_style; /* 0=block, 1=underline, 2=bar */
    int fg_color, bg_color;
    int bold, underline, italic, reverse;
    TerminalBuffer buffer;
    int scrollback_size;
    int pid;
    int pty_fd;
    char title[256];
    char working_dir[256];
    char* env_vars[64];
    int env_count;
    int tab_size;
    int bell;
    int wrap;
    int insert_mode;
    int alt_screen;
    TerminalCell alt_buffer_cells[1024];
} GUITerminal;

/* Function prototypes */
GUITerminal* term_create(int x, int y, int width, int height);
void term_destroy(GUITerminal* term);
void term_resize(GUITerminal* term, int width, int height);
void term_render(GUITerminal* term);
void term_handle_input(GUITerminal* term, char* data, int len);
void term_handle_key(GUITerminal* term, int key, int modifiers);

/* Buffer management */
void term_buffer_init(TerminalBuffer* buf, int cols, int rows);
void term_buffer_free(TerminalBuffer* buf);
void term_buffer_resize(TerminalBuffer* buf, int cols, int rows);
void term_buffer_write(TerminalBuffer* buf, char c);
void term_buffer_newline(TerminalBuffer* buf);
void term_buffer_scroll(TerminalBuffer* buf, int lines);
void term_buffer_clear(TerminalBuffer* buf);
void term_buffer_clear_line(TerminalBuffer* buf, int line);
void term_buffer_clear_region(TerminalBuffer* buf, int x1, int y1, int x2, int y2);

/* Cursor */
void term_cursor_move(GUITerminal* term, int x, int y);
void term_cursor_up(GUITerminal* term, int n);
void term_cursor_down(GUITerminal* term, int n);
void term_cursor_forward(GUITerminal* term, int n);
void term_cursor_backward(GUITerminal* term, int n);
void term_cursor_set_style(GUITerminal* term, int style);

/* Escape sequences */
void term_parse_escape(GUITerminal* term, char* seq, int len);
void term_handle_csi(GUITerminal* term, char* seq, int len);
void term_handle_osc(GUITerminal* term, char* seq, int len);

/* Colors */
void term_set_fg(GUITerminal* term, int color);
void term_set_bg(GUITerminal* term, int color);
void term_reset_colors(GUITerminal* term);
int term_color_from_ansi(int ansi_color);

/* Selection */
void term_select_start(GUITerminal* term, int x, int y);
void term_select_end(GUITerminal* term, int x, int y);
void term_select_clear(GUITerminal* term);
char* term_get_selection(GUITerminal* term);
void term_copy_selection(GUITerminal* term);
void term_paste(GUITerminal* term);

/* PTY */
int term_pty_create(GUITerminal* term);
void term_pty_destroy(GUITerminal* term);
int term_pty_read(GUITerminal* term, char* buffer, int size);
int term_pty_write(GUITerminal* term, char* buffer, int size);

#endif /* GUITERMINAL_H */
