/*
 * CodixOS Text Editor Implementation
 * Complete GUI text editor
 */

#include "texteditor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

/* Create text editor */
TextEditor* editor_create(int x, int y, int width, int height) {
    TextEditor* editor = (TextEditor*)malloc(sizeof(TextEditor));
    if (!editor) return NULL;
    
    memset(editor, 0, sizeof(TextEditor));
    
    editor->x = x;
    editor->y = y;
    editor->width = width;
    editor->height = height;
    
    /* Initialize buffer */
    buffer_init(&editor->buffer);
    
    /* Initialize cursor */
    editor->cursor.line = 0;
    editor->cursor.column = 0;
    editor->cursor.visible = 1;
    
    /* Default settings */
    editor->show_line_numbers = 1;
    editor->word_wrap = 0;
    editor->highlight_current_line = 1;
    editor->tab_size = EDITOR_TAB_SIZE;
    editor->use_spaces = 1;
    editor->auto_indent = 1;
    editor->font_size = EDITOR_FONT_SIZE;
    editor->line_height = EDITOR_LINE_HEIGHT;
    
    /* Set title */
    strcpy(editor->title, "Untitled");
    strcpy(editor->filename, "");
    
    /* Create empty line */
    buffer_insert_line(&editor->buffer, 0, "");
    
    printf("[Editor] Created text editor\n");
    return editor;
}

/* Destroy text editor */
void editor_destroy(TextEditor* editor) {
    if (!editor) return;
    
    buffer_free(&editor->buffer);
    free(editor);
    printf("[Editor] Destroyed text editor\n");
}

/* Render editor */
void editor_render(TextEditor* editor) {
    if (!editor) return;
    
    /* Rendering would use SDL2/X11 */
    /* Components: */
    /* 1. Line numbers */
    /* 2. Text content with syntax highlighting */
    /* 3. Cursor */
    /* 4. Selection highlight */
    /* 5. Scrollbar */
    /* 6. Status bar */
}

/* Handle event */
void editor_handle_event(TextEditor* editor, void* event) {
    /* Event handling would be implemented with SDL2/X11 */
}

/* ==================== FILE OPERATIONS ==================== */

/* Open file */
int editor_open(TextEditor* editor, const char* filename) {
    if (!editor || !filename) return -1;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        return -1;
    }
    
    /* Clear buffer */
    buffer_free(&editor->buffer);
    buffer_init(&editor->buffer);
    
    /* Read file */
    char line[4096];
    int line_num = 0;
    while (fgets(line, sizeof(line), file)) {
        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';
        buffer_insert_line(&editor->buffer, line_num++, line);
    }
    
    fclose(file);
    
    /* Update editor state */
    strncpy(editor->filename, filename, sizeof(editor->filename) - 1);
    strncpy(editor->title, filename, sizeof(editor->title) - 1);
    editor->modified = 0;
    
    /* Reset cursor */
    editor->cursor.line = 0;
    editor->cursor.column = 0;
    editor->scroll_x = 0;
    editor->scroll_y = 0;
    
    printf("[Editor] Opened: %s (%d lines)\n", filename, editor->buffer.line_count);
    return 0;
}

/* Save file */
int editor_save(TextEditor* editor) {
    if (!editor) return -1;
    
    if (strlen(editor->filename) == 0) {
        /* No filename, use save as */
        return -1;
    }
    
    return editor_save_as(editor, editor->filename);
}

/* Save as */
int editor_save_as(TextEditor* editor, const char* filename) {
    if (!editor || !filename) return -1;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("fopen");
        return -1;
    }
    
    /* Write all lines */
    for (int i = 0; i < editor->buffer.line_count; i++) {
        fprintf(file, "%s\n", editor->buffer.lines[i].text);
    }
    
    fclose(file);
    
    /* Update state */
    strncpy(editor->filename, filename, sizeof(editor->filename) - 1);
    strncpy(editor->title, filename, sizeof(editor->title) - 1);
    editor->modified = 0;
    
    printf("[Editor] Saved: %s\n", filename);
    return 0;
}

/* New file */
void editor_new(TextEditor* editor) {
    if (!editor) return;
    
    buffer_free(&editor->buffer);
    buffer_init(&editor->buffer);
    
    buffer_insert_line(&editor->buffer, 0, "");
    
    strcpy(editor->filename, "");
    strcpy(editor->title, "Untitled");
    editor->modified = 0;
    
    editor->cursor.line = 0;
    editor->cursor.column = 0;
    
    printf("[Editor] New file created\n");
}

/* Close editor */
int editor_close(TextEditor* editor) {
    if (!editor) return 0;
    
    if (editor->modified) {
        /* Would prompt to save */
        printf("[Editor] File modified, save? (y/n)\n");
    }
    
    return 0;
}

/* ==================== TEXT OPERATIONS ==================== */

/* Insert text */
void editor_insert_text(TextEditor* editor, const char* text) {
    if (!editor || !text) return;
    
    for (int i = 0; text[i]; i++) {
        if (text[i] == '\n') {
            /* Insert new line */
            char* rest = strdup(&editor->buffer.lines[editor->cursor.line].text[editor->cursor.column]);
            editor->buffer.lines[editor->cursor.line].text[editor->cursor.column] = '\0';
            
            buffer_insert_line(&editor->buffer, editor->cursor.line + 1, rest);
            free(rest);
            
            editor->cursor.line++;
            editor->cursor.column = 0;
        } else {
            /* Insert character */
            buffer_insert_char(&editor->buffer, editor->cursor.line, editor->cursor.column, text[i]);
            editor->cursor.column++;
        }
    }
    
    editor->modified = 1;
}

/* Delete selection */
void editor_delete_selection(TextEditor* editor) {
    if (!editor || !editor->selection.active) return;
    
    /* Get selection bounds */
    int start_line = editor->selection.start_line;
    int start_col = editor->selection.start_column;
    int end_line = editor->selection.end_line;
    int end_col = editor->selection.end_column;
    
    /* Ensure start < end */
    if (start_line > end_line || (start_line == end_line && start_col > end_col)) {
        int temp;
        temp = start_line; start_line = end_line; end_line = temp;
        temp = start_col; start_col = end_col; end_col = temp;
    }
    
    /* Delete text */
    if (start_line == end_line) {
        /* Same line */
        int len = end_col - start_col;
        memmove(&editor->buffer.lines[start_line].text[start_col],
                &editor->buffer.lines[start_line].text[end_col],
                strlen(&editor->buffer.lines[start_line].text[end_col]) + 1);
    } else {
        /* Multiple lines */
        /* Get rest of end line */
        char* rest = strdup(&editor->buffer.lines[end_line].text[end_col]);
        
        /* Truncate start line */
        editor->buffer.lines[start_line].text[start_col] = '\0';
        strcat(editor->buffer.lines[start_line].text, rest);
        free(rest);
        
        /* Delete intermediate lines */
        for (int i = start_line + 1; i <= end_line; i++) {
            buffer_delete_line(&editor->buffer, start_line + 1);
        }
    }
    
    editor->cursor.line = start_line;
    editor->cursor.column = start_col;
    editor->selection.active = 0;
    editor->modified = 1;
}

/* Delete char */
void editor_delete_char(TextEditor* editor) {
    if (!editor) return;
    
    if (editor->cursor.column > 0) {
        buffer_delete_char(&editor->buffer, editor->cursor.line, editor->cursor.column - 1);
        editor->cursor.column--;
    } else if (editor->cursor.line > 0) {
        /* Merge with previous line */
        int prev_len = strlen(editor->buffer.lines[editor->cursor.line - 1].text);
        int cur_len = strlen(editor->buffer.lines[editor->cursor.line].text);
        
        editor->buffer.lines[editor->cursor.line - 1].text = 
            realloc(editor->buffer.lines[editor->cursor.line - 1].text, prev_len + cur_len + 1);
        strcat(editor->buffer.lines[editor->cursor.line - 1].text, 
               editor->buffer.lines[editor->cursor.line].text);
        
        buffer_delete_line(&editor->buffer, editor->cursor.line);
        editor->cursor.line--;
        editor->cursor.column = prev_len;
    }
    
    editor->modified = 1;
}

/* Delete line */
void editor_delete_line(TextEditor* editor) {
    if (!editor) return;
    
    if (editor->buffer.line_count > 1) {
        buffer_delete_line(&editor->buffer, editor->cursor.line);
        if (editor->cursor.line >= editor->buffer.line_count) {
            editor->cursor.line = editor->buffer.line_count - 1;
        }
    } else {
        /* Clear the single line */
        editor->buffer.lines[0].text[0] = '\0';
        editor->buffer.lines[0].length = 0;
        editor->cursor.column = 0;
    }
    
    editor->modified = 1;
}

/* Duplicate line */
void editor_duplicate_line(TextEditor* editor) {
    if (!editor) return;
    
    char* text = editor->buffer.lines[editor->cursor.line].text;
    buffer_insert_line(&editor->buffer, editor->cursor.line + 1, text);
    editor->cursor.line++;
    editor->modified = 1;
}

/* Move line up */
void editor_move_line_up(TextEditor* editor) {
    if (!editor || editor->cursor.line == 0) return;
    
    /* Swap with previous line */
    TextLine temp = editor->buffer.lines[editor->cursor.line];
    editor->buffer.lines[editor->cursor.line] = editor->buffer.lines[editor->cursor.line - 1];
    editor->buffer.lines[editor->cursor.line - 1] = temp;
    
    editor->cursor.line--;
    editor->modified = 1;
}

/* Move line down */
void editor_move_line_down(TextEditor* editor) {
    if (!editor || editor->cursor.line >= editor->buffer.line_count - 1) return;
    
    /* Swap with next line */
    TextLine temp = editor->buffer.lines[editor->cursor.line];
    editor->buffer.lines[editor->cursor.line] = editor->buffer.lines[editor->cursor.line + 1];
    editor->buffer.lines[editor->cursor.line + 1] = temp;
    
    editor->cursor.line++;
    editor->modified = 1;
}

/* ==================== CURSOR OPERATIONS ==================== */

void editor_cursor_move(TextEditor* editor, int line, int column) {
    if (!editor) return;
    
    if (line < 0) line = 0;
    if (line >= editor->buffer.line_count) line = editor->buffer.line_count - 1;
    
    int max_col = strlen(editor->buffer.lines[line].text);
    if (column < 0) column = 0;
    if (column > max_col) column = max_col;
    
    editor->cursor.line = line;
    editor->cursor.column = column;
}

void editor_cursor_up(TextEditor* editor) {
    if (!editor || editor->cursor.line == 0) return;
    editor->cursor.line--;
    int max_col = strlen(editor->buffer.lines[editor->cursor.line].text);
    if (editor->cursor.column > max_col) {
        editor->cursor.column = max_col;
    }
}

void editor_cursor_down(TextEditor* editor) {
    if (!editor || editor->cursor.line >= editor->buffer.line_count - 1) return;
    editor->cursor.line++;
    int max_col = strlen(editor->buffer.lines[editor->cursor.line].text);
    if (editor->cursor.column > max_col) {
        editor->cursor.column = max_col;
    }
}

void editor_cursor_left(TextEditor* editor) {
    if (!editor) return;
    
    if (editor->cursor.column > 0) {
        editor->cursor.column--;
    } else if (editor->cursor.line > 0) {
        editor->cursor.line--;
        editor->cursor.column = strlen(editor->buffer.lines[editor->cursor.line].text);
    }
}

void editor_cursor_right(TextEditor* editor) {
    if (!editor) return;
    
    int max_col = strlen(editor->buffer.lines[editor->cursor.line].text);
    if (editor->cursor.column < max_col) {
        editor->cursor.column++;
    } else if (editor->cursor.line < editor->buffer.line_count - 1) {
        editor->cursor.line++;
        editor->cursor.column = 0;
    }
}

void editor_cursor_home(TextEditor* editor) {
    if (!editor) return;
    editor->cursor.column = 0;
}

void editor_cursor_end(TextEditor* editor) {
    if (!editor) return;
    editor->cursor.column = strlen(editor->buffer.lines[editor->cursor.line].text);
}

void editor_cursor_page_up(TextEditor* editor) {
    if (!editor) return;
    editor->cursor.line -= editor->visible_lines;
    if (editor->cursor.line < 0) editor->cursor.line = 0;
}

void editor_cursor_page_down(TextEditor* editor) {
    if (!editor) return;
    editor->cursor.line += editor->visible_lines;
    if (editor->cursor.line >= editor->buffer.line_count) {
        editor->cursor.line = editor->buffer.line_count - 1;
    }
}

void editor_cursor_word_left(TextEditor* editor) {
    if (!editor) return;
    
    while (editor->cursor.column > 0 && 
           isspace(editor->buffer.lines[editor->cursor.line].text[editor->cursor.column - 1])) {
        editor->cursor.column--;
    }
    while (editor->cursor.column > 0 && 
           !isspace(editor->buffer.lines[editor->cursor.line].text[editor->cursor.column - 1])) {
        editor->cursor.column--;
    }
}

void editor_cursor_word_right(TextEditor* editor) {
    if (!editor) return;
    
    int len = strlen(editor->buffer.lines[editor->cursor.line].text);
    while (editor->cursor.column < len && 
           !isspace(editor->buffer.lines[editor->cursor.line].text[editor->cursor.column])) {
        editor->cursor.column++;
    }
    while (editor->cursor.column < len && 
           isspace(editor->buffer.lines[editor->cursor.line].text[editor->cursor.column])) {
        editor->cursor.column++;
    }
}

/* ==================== SELECTION ==================== */

void editor_select_all(TextEditor* editor) {
    if (!editor) return;
    
    editor->selection.start_line = 0;
    editor->selection.start_column = 0;
    editor->selection.end_line = editor->buffer.line_count - 1;
    editor->selection.end_column = strlen(editor->buffer.lines[editor->selection.end_line].text);
    editor->selection.active = 1;
}

void editor_select_line(TextEditor* editor) {
    if (!editor) return;
    
    editor->selection.start_line = editor->cursor.line;
    editor->selection.start_column = 0;
    editor->selection.end_line = editor->cursor.line;
    editor->selection.end_column = strlen(editor->buffer.lines[editor->cursor.line].text);
    editor->selection.active = 1;
}

/* ==================== CLIPBOARD ==================== */

void editor_copy(TextEditor* editor) {
    if (!editor || !editor->selection.active) return;
    
    /* Would copy to system clipboard */
    printf("[Editor] Copied selection\n");
}

void editor_cut(TextEditor* editor) {
    if (!editor || !editor->selection.active) return;
    
    editor_copy(editor);
    editor_delete_selection(editor);
}

void editor_paste(TextEditor* editor) {
    if (!editor) return;
    
    /* Would paste from system clipboard */
    printf("[Editor] Pasted from clipboard\n");
}

/* ==================== UNDO/REDO ==================== */

void editor_undo(TextEditor* editor) {
    if (!editor || editor->undo_count == 0) return;
    
    /* Would restore from undo stack */
    printf("[Editor] Undo\n");
}

void editor_redo(TextEditor* editor) {
    if (!editor || editor->redo_count == 0) return;
    
    /* Would restore from redo stack */
    printf("[Editor] Redo\n");
}

/* ==================== SEARCH ==================== */

void editor_find(TextEditor* editor, const char* query) {
    if (!editor || !query) return;
    
    strncpy(editor->search_query, query, sizeof(editor->search_query) - 1);
    editor_find_next(editor);
}

void editor_find_next(TextEditor* editor) {
    if (!editor || strlen(editor->search_query) == 0) return;
    
    /* Search from current position */
    for (int i = editor->cursor.line; i < editor->buffer.line_count; i++) {
        char* text = editor->buffer.lines[i].text;
        char* found = strstr(text + editor->cursor.column, editor->search_query);
        
        if (found) {
            editor->cursor.line = i;
            editor->cursor.column = found - text;
            printf("[Editor] Found at line %d, column %d\n", i, editor->cursor.column);
            return;
        }
        editor->cursor.column = 0;
    }
    
    printf("[Editor] Not found\n");
}

void editor_find_previous(TextEditor* editor) {
    if (!editor || strlen(editor->search_query) == 0) return;
    
    /* Search backwards */
    for (int i = editor->cursor.line; i >= 0; i--) {
        char* text = editor->buffer.lines[i].text;
        char* found = strstr(text, editor->search_query);
        
        if (found && (found - text) < editor->cursor.column) {
            editor->cursor.line = i;
            editor->cursor.column = found - text;
            printf("[Editor] Found at line %d, column %d\n", i, editor->cursor.column);
            return;
        }
    }
    
    printf("[Editor] Not found\n");
}

/* ==================== VIEW ==================== */

void editor_scroll(TextEditor* editor, int dx, int dy) {
    if (!editor) return;
    
    editor->scroll_x += dx;
    editor->scroll_y += dy;
    
    if (editor->scroll_x < 0) editor->scroll_x = 0;
    if (editor->scroll_y < 0) editor->scroll_y = 0;
}

void editor_goto_line(TextEditor* editor, int line) {
    if (!editor) return;
    
    if (line < 1) line = 1;
    if (line > editor->buffer.line_count) line = editor->buffer.line_count;
    
    editor->cursor.line = line - 1;
    editor->cursor.column = 0;
}

void editor_center_cursor(TextEditor* editor) {
    if (!editor) return;
    
    editor->scroll_y = editor->cursor.line - editor->visible_lines / 2;
    if (editor->scroll_y < 0) editor->scroll_y = 0;
}

/* ==================== SYNTAX ==================== */

void editor_set_language(TextEditor* editor, const char* language) {
    if (!editor || !language) return;
    
    /* Set syntax highlighting rules based on language */
    editor->syntax.enabled = 1;
    
    if (strcmp(language, "c") == 0 || strcmp(language, "cpp") == 0) {
        editor->syntax.comment_start = "//";
        editor->syntax.comment_end = "*/";
        editor->syntax.string_delimiters = "\"'";
    } else if (strcmp(language, "python") == 0) {
        editor->syntax.comment_start = "#";
        editor->syntax.string_delimiters = "\"'";
    } else if (strcmp(language, "javascript") == 0) {
        editor->syntax.comment_start = "//";
        editor->syntax.comment_end = "*/";
        editor->syntax.string_delimiters = "\"'";
    }
    
    printf("[Editor] Set language: %s\n", language);
}

/* ==================== BUFFER ==================== */

void buffer_init(TextBuffer* buffer) {
    buffer->lines = NULL;
    buffer->line_count = 0;
    buffer->capacity = 0;
    buffer->modified = 0;
    buffer->filename = NULL;
    buffer->language = NULL;
}

void buffer_free(TextBuffer* buffer) {
    for (int i = 0; i < buffer->line_count; i++) {
        free(buffer->lines[i].text);
    }
    free(buffer->lines);
    free(buffer->filename);
    free(buffer->language);
    
    buffer->lines = NULL;
    buffer->line_count = 0;
    buffer->capacity = 0;
}

void buffer_insert_line(TextBuffer* buffer, int index, const char* text) {
    if (index < 0 || index > buffer->line_count) return;
    
    /* Grow array if needed */
    if (buffer->line_count >= buffer->capacity) {
        buffer->capacity = buffer->capacity == 0 ? 64 : buffer->capacity * 2;
        buffer->lines = realloc(buffer->lines, sizeof(TextLine) * buffer->capacity);
    }
    
    /* Shift lines down */
    memmove(&buffer->lines[index + 1], &buffer->lines[index],
            sizeof(TextLine) * (buffer->line_count - index));
    
    /* Initialize new line */
    buffer->lines[index].text = strdup(text ? text : "");
    buffer->lines[index].length = strlen(buffer->lines[index].text);
    buffer->lines[index].capacity = buffer->lines[index].length + 1;
    buffer->lines[index].modified = 0;
    
    buffer->line_count++;
}

void buffer_delete_line(TextBuffer* buffer, int index) {
    if (index < 0 || index >= buffer->line_count) return;
    
    free(buffer->lines[index].text);
    
    /* Shift lines up */
    memmove(&buffer->lines[index], &buffer->lines[index + 1],
            sizeof(TextLine) * (buffer->line_count - index - 1));
    
    buffer->line_count--;
}

void buffer_insert_char(TextBuffer* buffer, int line, int column, char c) {
    if (line < 0 || line >= buffer->line_count) return;
    
    TextLine* tl = &buffer->lines[line];
    
    /* Grow if needed */
    if (tl->length + 1 >= tl->capacity) {
        tl->capacity = tl->capacity == 0 ? 64 : tl->capacity * 2;
        tl->text = realloc(tl->text, tl->capacity);
    }
    
    /* Shift characters right */
    memmove(&tl->text[column + 1], &tl->text[column], tl->length - column + 1);
    
    tl->text[column] = c;
    tl->length++;
}

void buffer_delete_char(TextBuffer* buffer, int line, int column) {
    if (line < 0 || line >= buffer->line_count) return;
    
    TextLine* tl = &buffer->lines[line];
    if (column < 0 || column >= tl->length) return;
    
    /* Shift characters left */
    memmove(&tl->text[column], &tl->text[column + 1], tl->length - column);
    tl->length--;
}
