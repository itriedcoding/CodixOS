/*
 * CodixOS Text Editor Header
 * Simple GUI text editor
 */

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

/* Editor configuration */
#define EDITOR_DEFAULT_WIDTH    800
#define EDITOR_DEFAULT_HEIGHT   600
#define EDITOR_MIN_WIDTH        400
#define EDITOR_MIN_HEIGHT       300
#define EDITOR_TAB_SIZE         4
#define EDITOR_FONT_SIZE        14
#define EDITOR_LINE_HEIGHT      20
#define EDITOR_MARGIN_WIDTH     60

/* Colors (Catppuccin Mocha) */
#define EDITOR_BG_COLOR         0x1E1E2E
#define EDITOR_FG_COLOR         0xCDD6F4
#define EDITOR_CURSOR_COLOR     0xF5E0DC
#define EDITOR_SELECTION_COLOR  0x585B70
#define EDITOR_LINE_NUM_COLOR   0x585B70
#define EDITOR_KEYWORD_COLOR    0x89B4FA
#define EDITOR_STRING_COLOR     0xA6E3A1
#define EDITOR_COMMENT_COLOR   0x6C7086
#define EDITOR_NUMBER_COLOR     0xFAB387
#define EDITOR_FUNCTION_COLOR   0x89DCEB

/* Text line */
typedef struct {
    char* text;
    int length;
    int capacity;
    int modified;
} TextLine;

/* Text buffer */
typedef struct {
    TextLine* lines;
    int line_count;
    int capacity;
    int modified;
    char* filename;
    char* language;
} TextBuffer;

/* Cursor */
typedef struct {
    int line;
    int column;
    int desired_column;
    int visible;
    int blink_state;
} EditorCursor;

/* Selection */
typedef struct {
    int start_line, start_column;
    int end_line, end_column;
    int active;
} EditorSelection;

/* Syntax highlighting */
typedef struct {
    int enabled;
    char keywords[256][32];
    int keyword_count;
    char* comment_start;
    char* comment_end;
    char* string_delimiters;
} SyntaxHighlighting;

/* Editor state */
typedef struct {
    int id;
    int x, y, width, height;
    char title[256];
    char filename[512];
    int modified;
    int readonly;
    
    /* Buffer */
    TextBuffer buffer;
    
    /* Cursor */
    EditorCursor cursor;
    
    /* Selection */
    EditorSelection selection;
    
    /* View */
    int scroll_x, scroll_y;
    int visible_lines;
    int visible_columns;
    int show_line_numbers;
    int show_whitespace;
    int show_end_of_line;
    int word_wrap;
    int highlight_current_line;
    
    /* Syntax */
    SyntaxHighlighting syntax;
    
    /* Settings */
    int tab_size;
    int use_spaces;
    int auto_indent;
    int auto_save;
    int auto_save_interval;
    int font_size;
    int line_height;
    
    /* History */
    int** undo_stack;
    int** redo_stack;
    int undo_count;
    int redo_count;
    
    /* Search */
    char search_query[256];
    int search_mode;
    int search_wrap;
    int search_case_sensitive;
    int search_regex;
} TextEditor;

/* Function prototypes */
TextEditor* editor_create(int x, int y, int width, int height);
void editor_destroy(TextEditor* editor);
void editor_render(TextEditor* editor);
void editor_handle_event(TextEditor* editor, void* event);

/* File operations */
int editor_open(TextEditor* editor, const char* filename);
int editor_save(TextEditor* editor);
int editor_save_as(TextEditor* editor, const char* filename);
void editor_new(TextEditor* editor);
int editor_close(TextEditor* editor);

/* Text operations */
void editor_insert_text(TextEditor* editor, const char* text);
void editor_delete_selection(TextEditor* editor);
void editor_delete_char(TextEditor* editor);
void editor_delete_line(TextEditor* editor);
void editor_duplicate_line(TextEditor* editor);
void editor_move_line_up(TextEditor* editor);
void editor_move_line_down(TextEditor* editor);

/* Cursor operations */
void editor_cursor_move(TextEditor* editor, int line, int column);
void editor_cursor_up(TextEditor* editor);
void editor_cursor_down(TextEditor* editor);
void editor_cursor_left(TextEditor* editor);
void editor_cursor_right(TextEditor* editor);
void editor_cursor_home(TextEditor* editor);
void editor_cursor_end(TextEditor* editor);
void editor_cursor_page_up(TextEditor* editor);
void editor_cursor_page_down(TextEditor* editor);
void editor_cursor_word_left(TextEditor* editor);
void editor_cursor_word_right(TextEditor* editor);

/* Selection operations */
void editor_select_all(TextEditor* editor);
void editor_select_line(TextEditor* editor);
void editor_select_word(TextEditor* editor);
void editor_select_to_end(TextEditor* editor);
void editor_select_to_beginning(TextEditor* editor);

/* Clipboard operations */
void editor_copy(TextEditor* editor);
void editor_cut(TextEditor* editor);
void editor_paste(TextEditor* editor);

/* Undo/Redo */
void editor_undo(TextEditor* editor);
void editor_redo(TextEditor* editor);

/* Search */
void editor_find(TextEditor* editor, const char* query);
void editor_find_next(TextEditor* editor);
void editor_find_previous(TextEditor* editor);
void editor_replace(TextEditor* editor, const char* find, const char* replace);
void editor_replace_all(TextEditor* editor, const char* find, const char* replace);

/* View */
void editor_scroll(TextEditor* editor, int dx, int dy);
void editor_goto_line(TextEditor* editor, int line);
void editor_goto_column(TextEditor* editor, int column);
void editor_center_cursor(TextEditor* editor);

/* Syntax highlighting */
void editor_set_language(TextEditor* editor, const char* language);
void editor_highlight_syntax(TextEditor* editor);
int editor_get_token_type(TextEditor* editor, int line, int column);

/* Buffer operations */
void buffer_init(TextBuffer* buffer);
void buffer_free(TextBuffer* buffer);
void buffer_insert_line(TextBuffer* buffer, int index, const char* text);
void buffer_delete_line(TextBuffer* buffer, int index);
void buffer_insert_char(TextBuffer* buffer, int line, int column, char c);
void buffer_delete_char(TextBuffer* buffer, int line, int column);
char* buffer_get_text(TextBuffer* buffer, int start_line, int start_col, int end_line, int end_col);

#endif /* TEXTEDITOR_H */
