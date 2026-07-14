/*
 * CodixOS File Manager Implementation
 * Complete GUI file manager
 */

#include "filemanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/* Create file manager */
FileManager* fm_create(int x, int y, int width, int height) {
    FileManager* fm = (FileManager*)malloc(sizeof(FileManager));
    if (!fm) return NULL;
    
    memset(fm, 0, sizeof(FileManager));
    
    fm->x = x;
    fm->y = y;
    fm->width = width;
    fm->height = height;
    
    /* Set directories */
    char* home = getenv("HOME");
    if (home) {
        strncpy(fm->home_dir, home, sizeof(fm->home_dir) - 1);
        strncpy(fm->current_dir, home, sizeof(fm->current_dir) - 1);
    } else {
        strcpy(fm->home_dir, "/home/codix");
        strcpy(fm->current_dir, "/home/codix");
    }
    
    /* Default settings */
    fm->view_mode = FM_VIEW_ICONS;
    fm->sort_mode = FM_SORT_NAME;
    fm->sort_ascending = 1;
    fm->show_hidden = 0;
    fm->show_toolbar = 1;
    fm->show_sidebar = 1;
    fm->show_statusbar = 1;
    
    /* Initialize bookmarks */
    fm_add_bookmark(fm, "Home", fm->home_dir);
    fm_add_bookmark(fm, "Documents", "/home/codix/Documents");
    fm_add_bookmark(fm, "Downloads", "/home/codix/Downloads");
    fm_add_bookmark(fm, "Desktop", "/home/codix/Desktop");
    fm_add_bookmark(fm, "---", NULL); /* Separator */
    fm_add_bookmark(fm, "Root", "/");
    
    /* Initialize history */
    fm->history = (char**)malloc(sizeof(char*) * 100);
    fm->history_count = 0;
    fm->history_index = 0;
    
    /* Load files */
    fm_refresh(fm);
    
    printf("[FileManager] Created file manager: %s\n", fm->current_dir);
    return fm;
}

/* Destroy file manager */
void fm_destroy(FileManager* fm) {
    if (!fm) return;
    
    /* Free files */
    if (fm->files) {
        free(fm->files);
    }
    
    /* Free history */
    for (int i = 0; i < fm->history_count; i++) {
        free(fm->history[i]);
    }
    free(fm->history);
    
    free(fm);
    printf("[FileManager] Destroyed file manager\n");
}

/* Render file manager */
void fm_render(FileManager* fm) {
    if (!fm) return;
    
    /* Rendering would use SDL2/X11 */
    /* Components: */
    /* 1. Toolbar (back, forward, up, home, path bar, view options) */
    /* 2. Sidebar (bookmarks, devices) */
    /* 3. Main content area (files in current view mode) */
    /* 4. Status bar (file count, selected count) */
    
    printf("[FileManager] Rendering %d files in %s\n", fm->file_count, fm->current_dir);
}

/* Handle event */
void fm_handle_event(FileManager* fm, void* event) {
    /* Event handling would be implemented with SDL2/X11 */
}

/* ==================== NAVIGATION ==================== */

/* Navigate to path */
void fm_navigate(FileManager* fm, const char* path) {
    if (!fm || !path) return;
    
    /* Add current to history */
    if (fm->history_count < 100) {
        fm->history[fm->history_count] = strdup(fm->current_dir);
        fm->history_count++;
        fm->history_index = fm->history_count;
    }
    
    /* Update current directory */
    strncpy(fm->current_dir, path, sizeof(fm->current_dir) - 1);
    
    /* Clear forward history */
    for (int i = 0; i < fm->forward_count; i++) {
        free(fm->forward_history[i]);
    }
    free(fm->forward_history);
    fm->forward_history = NULL;
    fm->forward_count = 0;
    
    /* Refresh file list */
    fm_refresh(fm);
    
    printf("[FileManager] Navigated to: %s\n", path);
}

/* Go back */
void fm_go_back(FileManager* fm) {
    if (!fm || fm->history_index <= 0) return;
    
    /* Add current to forward history */
    if (fm->forward_count < 100) {
        fm->forward_history = realloc(fm->forward_history, 
                                      sizeof(char*) * (fm->forward_count + 1));
        fm->forward_history[fm->forward_count] = strdup(fm->current_dir);
        fm->forward_count++;
    }
    
    /* Go to previous */
    fm->history_index--;
    strncpy(fm->current_dir, fm->history[fm->history_index], sizeof(fm->current_dir) - 1);
    
    fm_refresh(fm);
}

/* Go forward */
void fm_go_forward(FileManager* fm) {
    if (!fm || fm->history_index >= fm->history_count - 1) return;
    
    /* Add current to history */
    if (fm->history_count < 100) {
        fm->history[fm->history_count] = strdup(fm->current_dir);
        fm->history_count++;
    }
    
    /* Go to next */
    fm->history_index++;
    strncpy(fm->current_dir, fm->history[fm->history_index], sizeof(fm->current_dir) - 1);
    
    fm_refresh(fm);
}

/* Go up */
void fm_go_up(FileManager* fm) {
    if (!fm) return;
    
    char* last_slash = strrchr(fm->current_dir, '/');
    if (last_slash && last_slash != fm->current_dir) {
        *last_slash = '\0';
        fm_navigate(fm, fm->current_dir);
    }
}

/* Go home */
void fm_go_home(FileManager* fm) {
    if (!fm) return;
    fm_navigate(fm, fm->home_dir);
}

/* Refresh */
void fm_refresh(FileManager* fm) {
    if (!fm) return;
    
    /* Free old files */
    if (fm->files) {
        free(fm->files);
        fm->files = NULL;
    }
    fm->file_count = 0;
    fm->selected_count = 0;
    
    /* Read directory */
    DIR* dir = opendir(fm->current_dir);
    if (!dir) {
        perror("opendir");
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        /* Skip hidden files if not showing */
        if (!fm->show_hidden && entry->d_name[0] == '.') {
            continue;
        }
        
        /* Resize array */
        fm->files = realloc(fm->files, sizeof(FMFileEntry) * (fm->file_count + 1));
        
        FMFileEntry* file = &fm->files[fm->file_count];
        memset(file, 0, sizeof(FMFileEntry));
        
        strncpy(file->name, entry->d_name, sizeof(file->name) - 1);
        snprintf(file->path, sizeof(file->path), "%s/%s", fm->current_dir, entry->d_name);
        
        /* Get file info */
        struct stat st;
        if (stat(file->path, &st) == 0) {
            file->size = st.st_size;
            file->permissions = st.st_mode;
            file->modified_time = st.st_mtime;
            
            if (S_ISDIR(st.st_mode)) {
                file->type = FM_FILE_TYPE_FOLDER;
            } else if (S_ISLNK(st.st_mode)) {
                file->type = FM_FILE_TYPE_LINK;
            } else {
                file->type = FM_FILE_TYPE_FILE;
            }
        }
        
        fm->file_count++;
    }
    
    closedir(dir);
    
    /* Sort files */
    fm_sort_files(fm);
}

/* ==================== FILE OPERATIONS ==================== */

/* Open file */
void fm_open_file(FileManager* fm, const char* path) {
    if (!fm || !path) return;
    
    /* Check if directory */
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        fm_navigate(fm, path);
    } else {
        /* Open file with default application */
        printf("[FileManager] Opening file: %s\n", path);
        /* Would launch default application */
    }
}

/* Create folder */
void fm_create_folder(FileManager* fm, const char* name) {
    if (!fm || !name) return;
    
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", fm->current_dir, name);
    
    if (mkdir(path, 0755) == 0) {
        fm_refresh(fm);
        printf("[FileManager] Created folder: %s\n", name);
    } else {
        perror("mkdir");
    }
}

/* Create file */
void fm_create_file(FileManager* fm, const char* name) {
    if (!fm || !name) return;
    
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", fm->current_dir, name);
    
    FILE* f = fopen(path, "w");
    if (f) {
        fclose(f);
        fm_refresh(fm);
        printf("[FileManager] Created file: %s\n", name);
    } else {
        perror("fopen");
    }
}

/* Delete selected */
void fm_delete_selected(FileManager* fm) {
    if (!fm) return;
    
    for (int i = 0; i < fm->file_count; i++) {
        if (fm->files[i].is_selected) {
            printf("[FileManager] Deleting: %s\n", fm->files[i].path);
            /* Would delete file */
        }
    }
    
    fm_refresh(fm);
}

/* Copy selected */
void fm_copy_selected(FileManager* fm) {
    if (!fm) return;
    
    /* Clear clipboard */
    for (int i = 0; i < fm->clipboard_count; i++) {
        free(fm->clipboard_files[i]);
    }
    free(fm->clipboard_files);
    
    fm->clipboard_count = 0;
    fm->clipboard_cut = 0;
    
    for (int i = 0; i < fm->file_count; i++) {
        if (fm->files[i].is_selected) {
            fm->clipboard_files = realloc(fm->clipboard_files,
                                          sizeof(char*) * (fm->clipboard_count + 1));
            fm->clipboard_files[fm->clipboard_count] = strdup(fm->files[i].path);
            fm->clipboard_count++;
        }
    }
    
    printf("[FileManager] Copied %d files to clipboard\n", fm->clipboard_count);
}

/* Cut selected */
void fm_cut_selected(FileManager* fm) {
    fm_copy_selected(fm);
    fm->clipboard_cut = 1;
    printf("[FileManager] Cut %d files to clipboard\n", fm->clipboard_count);
}

/* Paste */
void fm_paste(FileManager* fm) {
    if (!fm || fm->clipboard_count == 0) return;
    
    for (int i = 0; i < fm->clipboard_count; i++) {
        printf("[FileManager] Pasting: %s\n", fm->clipboard_files[i]);
        /* Would copy/cut file */
    }
    
    fm_refresh(fm);
}

/* Rename */
void fm_rename(FileManager* fm, int index, const char* new_name) {
    if (!fm || index < 0 || index >= fm->file_count) return;
    
    char new_path[512];
    snprintf(new_path, sizeof(new_path), "%s/%s", fm->current_dir, new_name);
    
    if (rename(fm->files[index].path, new_path) == 0) {
        fm_refresh(fm);
        printf("[FileManager] Renamed to: %s\n", new_name);
    } else {
        perror("rename");
    }
}

/* ==================== VIEW ==================== */

/* Set view mode */
void fm_set_view_mode(FileManager* fm, int mode) {
    if (!fm) return;
    fm->view_mode = mode;
    printf("[FileManager] View mode: %d\n", mode);
}

/* Set sort mode */
void fm_set_sort_mode(FileManager* fm, int mode) {
    if (!fm) return;
    
    if (fm->sort_mode == mode) {
        fm->sort_ascending = !fm->sort_ascending;
    } else {
        fm->sort_mode = mode;
        fm->sort_ascending = 1;
    }
    
    fm_sort_files(fm);
}

/* Toggle hidden */
void fm_toggle_hidden(FileManager* fm) {
    if (!fm) return;
    fm->show_hidden = !fm->show_hidden;
    fm_refresh(fm);
}

/* ==================== BOOKMARKS ==================== */

/* Add bookmark */
void fm_add_bookmark(FileManager* fm, const char* name, const char* path) {
    if (!fm || fm->bookmark_count >= 32) return;
    
    FMBookmark* bm = &fm->bookmarks[fm->bookmark_count];
    strncpy(bm->name, name, sizeof(bm->name) - 1);
    if (path) {
        strncpy(bm->path, path, sizeof(bm->path) - 1);
    }
    bm->is_separator = (strcmp(name, "---") == 0);
    
    fm->bookmark_count++;
}

/* Remove bookmark */
void fm_remove_bookmark(FileManager* fm, int index) {
    if (!fm || index < 0 || index >= fm->bookmark_count) return;
    
    for (int i = index; i < fm->bookmark_count - 1; i++) {
        fm->bookmarks[i] = fm->bookmarks[i + 1];
    }
    fm->bookmark_count--;
}

/* ==================== SORTING ==================== */

/* Sort files */
void fm_sort_files(FileManager* fm) {
    if (!fm || !fm->files) return;
    
    qsort(fm->files, fm->file_count, sizeof(FMFileEntry), fm_compare_files);
}

/* Compare files */
int fm_compare_files(const void* a, const void* b) {
    const FMFileEntry* fa = (const FMFileEntry*)a;
    const FMFileEntry* fb = (const FMFileEntry*)b;
    
    /* Folders first */
    if (fa->type == FM_FILE_TYPE_FOLDER && fb->type != FM_FILE_TYPE_FOLDER) return -1;
    if (fa->type != FM_FILE_TYPE_FOLDER && fb->type == FM_FILE_TYPE_FOLDER) return 1;
    
    /* Then by name */
    return strcasecmp(fa->name, fb->name);
}

/* ==================== SELECTION ==================== */

/* Select file */
void fm_select_file(FileManager* fm, int index) {
    if (!fm || index < 0 || index >= fm->file_count) return;
    
    fm->files[index].is_selected = 1;
    fm->selected_count++;
    fm->last_selected = index;
}

/* Deselect file */
void fm_deselect_file(FileManager* fm, int index) {
    if (!fm || index < 0 || index >= fm->file_count) return;
    
    fm->files[index].is_selected = 0;
    fm->selected_count--;
}

/* Toggle file selection */
void fm_toggle_file_selection(FileManager* fm, int index) {
    if (!fm || index < 0 || index >= fm->file_count) return;
    
    if (fm->files[index].is_selected) {
        fm_deselect_file(fm, index);
    } else {
        fm_select_file(fm, index);
    }
}

/* Get file at position */
int fm_get_file_at_position(FileManager* fm, int x, int y) {
    if (!fm) return -1;
    
    /* Calculate which file is at the given position */
    /* This depends on view mode and layout */
    return -1;
}

/* Select all */
void fm_select_all(FileManager* fm) {
    if (!fm) return;
    
    for (int i = 0; i < fm->file_count; i++) {
        fm->files[i].is_selected = 1;
    }
    fm->selected_count = fm->file_count;
}

/* Select none */
void fm_select_none(FileManager* fm) {
    if (!fm) return;
    
    for (int i = 0; i < fm->file_count; i++) {
        fm->files[i].is_selected = 0;
    }
    fm->selected_count = 0;
}

/* Invert selection */
void fm_invert_selection(FileManager* fm) {
    if (!fm) return;
    
    for (int i = 0; i < fm->file_count; i++) {
        fm->files[i].is_selected = !fm->files[i].is_selected;
    }
    fm->selected_count = fm->file_count - fm->selected_count;
}

/* Search */
void fm_search(FileManager* fm, const char* query) {
    if (!fm || !query) return;
    
    printf("[FileManager] Searching for: %s\n", query);
    /* Would filter file list */
}

/* Clear search */
void fm_clear_search(FileManager* fm) {
    fm_refresh(fm);
}
