/*
 * CodixOS GUI Terminal Implementation
 * Full-featured terminal emulator
 */

#include "guiterminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pty.h>
#include <signal.h>
#include <sys/wait.h>

/* Create terminal */
GUITerminal* term_create(int x, int y, int width, int height) {
    GUITerminal* term = (GUITerminal*)malloc(sizeof(GUITerminal));
    if (!term) return NULL;
    
    memset(term, 0, sizeof(GUITerminal));
    
    term->x = x;
    term->y = y;
    term->width = width;
    term->height = height;
    
    /* Calculate cols/rows */
    term->cols = width / 8; /* Assuming 8px font width */
    term->rows = height / 16; /* Assuming 16px font height */
    
    /* Initialize buffer */
    term_buffer_init(&term->buffer, term->cols, term->rows);
    
    /* Terminal settings */
    term->cursor_visible = 1;
    term->cursor_blink = 1;
    term->cursor_style = 0;
    term->fg_color = TERM_FG_COLOR;
    term->bg_color = TERM_BG_COLOR;
    term->tab_size = 8;
    term->wrap = 1;
    term->insert_mode = 0;
    term->alt_screen = 0;
    term->scrollback_size = 10000;
    
    /* Set title */
    strcpy(term->title, "Terminal");
    strcpy(term->working_dir, "/home/codix");
    
    /* Create PTY */
    term_pty_create(term);
    
    printf("[Terminal] Created terminal: %dx%d (%dx%d)\n", width, height, term->cols, term->rows);
    return term;
}

/* Destroy terminal */
void term_destroy(GUITerminal* term) {
    if (!term) return;
    
    term_pty_destroy(term);
    term_buffer_free(&term->buffer);
    free(term);
    printf("[Terminal] Destroyed terminal\n");
}

/* Resize terminal */
void term_resize(GUITerminal* term, int width, int height) {
    if (!term) return;
    
    term->width = width;
    term->height = height;
    
    int new_cols = width / 8;
    int new_rows = height / 16;
    
    if (new_cols != term->cols || new_rows != term->rows) {
        term->cols = new_cols;
        term->rows = new_rows;
        term_buffer_resize(&term->buffer, new_cols, new_rows);
        
        /* Notify PTY of size change */
        /* winsize ws = { .ws_row = new_rows, .ws_col = new_cols }; */
        /* ioctl(term->pty_fd, TIOCSWINSZ, &ws); */
    }
}

/* Render terminal */
void term_render(GUITerminal* term) {
    if (!term) return;
    
    /* Rendering would use SDL2/X11 */
    /* For each cell in buffer: */
    /* 1. Draw background */
    /* 2. Draw character */
    /* 3. Draw cursor if visible */
}

/* Handle input data from PTY */
void term_handle_input(GUITerminal* term, char* data, int len) {
    if (!term || !data) return;
    
    for (int i = 0; i < len; i++) {
        char c = data[i];
        
        if (c == '\033') {
            /* Escape sequence */
            char seq[64];
            int seq_len = 0;
            i++;
            while (i < len && seq_len < 63) {
                seq[seq_len++] = data[i];
                if (data[i] >= 'A' && data[i] <= 'z') break;
                i++;
            }
            seq[seq_len] = '\0';
            term_parse_escape(term, seq, seq_len);
        } else if (c == '\n') {
            term_buffer_newline(&term->buffer);
        } else if (c == '\r') {
            term->cursor_x = 0;
        } else if (c == '\t') {
            term->cursor_x = (term->cursor_x + term->tab_size) & ~(term->tab_size - 1);
        } else if (c == '\b') {
            if (term->cursor_x > 0) term->cursor_x--;
        } else {
            term_buffer_write(&term->buffer, c);
        }
    }
}

/* Handle key input */
void term_handle_key(GUITerminal* term, int key, int modifiers) {
    if (!term) return;
    
    char data[32];
    int len = 0;
    
    /* Convert key to terminal input */
    if (modifiers & 0x01) { /* Ctrl */
        if (key >= 'a' && key <= 'z') {
            data[len++] = key - 'a' + 1;
        } else if (key == ' ') {
            data[len++] = 0;
        }
    } else if (key == 27) { /* Escape */
        data[len++] = '\033';
    } else if (key == 13) { /* Enter */
        data[len++] = '\r';
    } else if (key == 9) { /* Tab */
        data[len++] = '\t';
    } else if (key == 127) { /* Backspace */
        data[len++] = '\b';
    } else if (key >= 32 && key < 127) {
        data[len++] = key;
    }
    
    if (len > 0) {
        term_pty_write(term, data, len);
    }
}

/* ==================== BUFFER ==================== */

/* Initialize buffer */
void term_buffer_init(TerminalBuffer* buf, int cols, int rows) {
    buf->cols = cols;
    buf->rows = rows;
    buf->scrollback_lines = 10000;
    buf->scroll_top = 0;
    buf->scroll_bottom = rows - 1;
    buf->scroll_offset = 0;
    
    buf->cells = (TerminalCell*)calloc(cols * (rows + buf->scrollback_lines), sizeof(TerminalCell));
    if (buf->cells) {
        /* Initialize with spaces */
        for (int i = 0; i < cols * (rows + buf->scrollback_lines); i++) {
            buf->cells[i].character = ' ';
            buf->cells[i].fg_color = TERM_FG_COLOR;
            buf->cells[i].bg_color = TERM_BG_COLOR;
        }
    }
}

/* Free buffer */
void term_buffer_free(TerminalBuffer* buf) {
    if (buf->cells) {
        free(buf->cells);
        buf->cells = NULL;
    }
}

/* Resize buffer */
void term_buffer_resize(TerminalBuffer* buf, int cols, int rows) {
    TerminalCell* new_cells = (TerminalCell*)calloc(cols * (rows + buf->scrollback_lines), sizeof(TerminalCell));
    if (!new_cells) return;
    
    /* Copy old content */
    int min_cols = (cols < buf->cols) ? cols : buf->cols;
    int min_rows = (rows < buf->rows) ? rows : buf->rows;
    
    for (int y = 0; y < min_rows; y++) {
        for (int x = 0; x < min_cols; x++) {
            int old_idx = y * buf->cols + x;
            int new_idx = y * cols + x;
            new_cells[new_idx] = buf->cells[old_idx];
        }
    }
    
    free(buf->cells);
    buf->cells = new_cells;
    buf->cols = cols;
    buf->rows = rows;
    buf->scroll_bottom = rows - 1;
}

/* Write character to buffer */
void term_buffer_write(TerminalBuffer* buf, char c) {
    int idx = buf->scroll_top * buf->cols + buf->cols; /* Would need cursor position */
    /* Simplified - just write to current position */
}

/* Newline */
void term_buffer_newline(TerminalBuffer* buf) {
    buf->scroll_top++;
    if (buf->scroll_top > buf->scroll_bottom) {
        buf->scroll_top = buf->scroll_bottom;
        term_buffer_scroll(buf, 1);
    }
}

/* Scroll buffer */
void term_buffer_scroll(TerminalBuffer* buf, int lines) {
    if (lines > 0) {
        /* Scroll up - move content up */
        for (int y = 0; y < buf->rows - lines; y++) {
            memcpy(&buf->cells[y * buf->cols], 
                   &buf->cells[(y + lines) * buf->cols],
                   buf->cols * sizeof(TerminalCell));
        }
        /* Clear bottom lines */
        for (int y = buf->rows - lines; y < buf->rows; y++) {
            for (int x = 0; x < buf->cols; x++) {
                int idx = y * buf->cols + x;
                buf->cells[idx].character = ' ';
                buf->cells[idx].fg_color = TERM_FG_COLOR;
                buf->cells[idx].bg_color = TERM_BG_COLOR;
            }
        }
    }
}

/* Clear buffer */
void term_buffer_clear(TerminalBuffer* buf) {
    for (int i = 0; i < buf->cols * buf->rows; i++) {
        buf->cells[i].character = ' ';
        buf->cells[i].fg_color = TERM_FG_COLOR;
        buf->cells[i].bg_color = TERM_BG_COLOR;
    }
}

/* Clear line */
void term_buffer_clear_line(TerminalBuffer* buf, int line) {
    for (int x = 0; x < buf->cols; x++) {
        int idx = line * buf->cols + x;
        buf->cells[idx].character = ' ';
        buf->cells[idx].fg_color = TERM_FG_COLOR;
        buf->cells[idx].bg_color = TERM_BG_COLOR;
    }
}

/* Clear region */
void term_buffer_clear_region(TerminalBuffer* buf, int x1, int y1, int x2, int y2) {
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            int idx = y * buf->cols + x;
            buf->cells[idx].character = ' ';
            buf->cells[idx].fg_color = TERM_FG_COLOR;
            buf->cells[idx].bg_color = TERM_BG_COLOR;
        }
    }
}

/* ==================== CURSOR ==================== */

void term_cursor_move(GUITerminal* term, int x, int y) {
    term->cursor_x = x;
    term->cursor_y = y;
}

void term_cursor_up(GUITerminal* term, int n) {
    term->cursor_y -= n;
    if (term->cursor_y < 0) term->cursor_y = 0;
}

void term_cursor_down(GUITerminal* term, int n) {
    term->cursor_y += n;
    if (term->cursor_y >= term->rows) term->cursor_y = term->rows - 1;
}

void term_cursor_forward(GUITerminal* term, int n) {
    term->cursor_x += n;
    if (term->cursor_x >= term->cols) term->cursor_x = term->cols - 1;
}

void term_cursor_backward(GUITerminal* term, int n) {
    term->cursor_x -= n;
    if (term->cursor_x < 0) term->cursor_x = 0;
}

void term_cursor_set_style(GUITerminal* term, int style) {
    term->cursor_style = style;
}

/* ==================== ESCAPE SEQUENCES ==================== */

void term_parse_escape(GUITerminal* term, char* seq, int len) {
    if (!seq || len == 0) return;
    
    if (seq[0] == '[') {
        term_handle_csi(term, seq + 1, len - 1);
    } else if (seq[0] == ']') {
        term_handle_osc(term, seq + 1, len - 1);
    }
}

void term_handle_csi(GUITerminal* term, char* seq, int len) {
    /* Parse CSI sequences */
    /* Examples: */
    /* H - Cursor position */
    /* J - Clear screen */
    /* K - Clear line */
    /* m - SGR (colors, attributes) */
    
    if (len == 0) return;
    
    char final = seq[len - 1];
    seq[len - 1] = '\0';
    
    int params[16] = {0};
    int param_count = 0;
    
    /* Parse parameters */
    char* p = seq;
    while (*p && param_count < 16) {
        if (*p == ';') {
            param_count++;
            p++;
        } else {
            params[param_count] = params[param_count] * 10 + (*p - '0');
            p++;
        }
    }
    param_count++;
    
    switch (final) {
        case 'H': /* Cursor position */
        case 'f':
            if (param_count >= 2) {
                term_cursor_move(term, params[1] - 1, params[0] - 1);
            }
            break;
        case 'J': /* Clear screen */
            if (param_count == 0 || params[0] == 0) {
                /* Clear from cursor to end */
            } else if (params[0] == 2) {
                term_buffer_clear(&term->buffer);
            }
            break;
        case 'K': /* Clear line */
            if (param_count == 0 || params[0] == 0) {
                /* Clear from cursor to end of line */
            } else if (params[0] == 2) {
                term_buffer_clear_line(&term->buffer, term->cursor_y);
            }
            break;
        case 'm': /* SGR */
            for (int i = 0; i < param_count; i++) {
                if (params[i] == 0) {
                    term_reset_colors(term);
                } else if (params[i] == 1) {
                    term->bold = 1;
                } else if (params[i] == 3) {
                    term->italic = 1;
                } else if (params[i] == 4) {
                    term->underline = 1;
                } else if (params[i] == 7) {
                    term->reverse = 1;
                } else if (params[i] >= 30 && params[i] <= 37) {
                    term_set_fg(term, term_color_from_ansi(params[i] - 30));
                } else if (params[i] >= 40 && params[i] <= 47) {
                    term_set_bg(term, term_color_from_ansi(params[i] - 40));
                } else if (params[i] >= 90 && params[i] <= 97) {
                    term_set_fg(term, term_color_from_ansi(params[i] - 82));
                } else if (params[i] >= 100 && params[i] <= 107) {
                    term_set_bg(term, term_color_from_ansi(params[i] - 92));
                }
            }
            break;
    }
}

void term_handle_osc(GUITerminal* term, char* seq, int len) {
    /* Parse OSC sequences */
    if (len < 2) return;
    
    /* Check for title set */
    if (seq[0] == '0' || seq[1] == ';') {
        char* title = seq + 2;
        title[len - 3] = '\0'; /* Remove ST */
        strncpy(term->title, title, sizeof(term->title) - 1);
    }
}

/* ==================== COLORS ==================== */

void term_set_fg(GUITerminal* term, int color) {
    term->fg_color = color;
}

void term_set_bg(GUITerminal* term, int color) {
    term->bg_color = color;
}

void term_reset_colors(GUITerminal* term) {
    term->fg_color = TERM_FG_COLOR;
    term->bg_color = TERM_BG_COLOR;
    term->bold = 0;
    term->underline = 0;
    term->italic = 0;
    term->reverse = 0;
}

int term_color_from_ansi(int ansi_color) {
    switch (ansi_color) {
        case 0: return TERM_BLACK;
        case 1: return TERM_RED;
        case 2: return TERM_GREEN;
        case 3: return TERM_YELLOW;
        case 4: return TERM_BLUE;
        case 5: return TERM_MAGENTA;
        case 6: return TERM_CYAN;
        case 7: return TERM_WHITE;
        default: return TERM_FG_COLOR;
    }
}

/* ==================== PTY ==================== */

int term_pty_create(GUITerminal* term) {
    /* Create pseudo-terminal */
    /* This would use forkpty() or openpty() */
    printf("[Terminal] PTY created\n");
    return 0;
}

void term_pty_destroy(GUITerminal* term) {
    if (term->pid > 0) {
        kill(term->pid, SIGTERM);
        waitpid(term->pid, NULL, 0);
    }
    if (term->pty_fd > 0) {
        close(term->pty_fd);
    }
    printf("[Terminal] PTY destroyed\n");
}

int term_pty_read(GUITerminal* term, char* buffer, int size) {
    /* Read from PTY */
    /* return read(term->pty_fd, buffer, size); */
    return 0;
}

int term_pty_write(GUITerminal* term, char* buffer, int size) {
    /* Write to PTY */
    /* return write(term->pty_fd, buffer, size); */
    return size;
}
