/*
 * CodixOS File Manager Header
 * GUI file manager application
 */

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

/* File manager configuration */
#define FM_DEFAULT_WIDTH    900
#define FM_DEFAULT_HEIGHT   600
#define FM_MIN_WIDTH        600
#define FM_MIN_HEIGHT       400
#define FM_ICON_SIZE        48
#define FM_LIST_ICON_SIZE   16
#define FM_SIDEBAR_WIDTH    200
#define FM_TOOLBAR_HEIGHT   40
#define FM_STATUSBAR_HEIGHT 25

/* View modes */
#define FM_VIEW_ICONS       0
#define FM_VIEW_LIST        1
#define FM_VIEW_COLUMNS     2
#define FM_VIEW_TILES       3

/* Sort modes */
#define FM_SORT_NAME        0
#define FM_SORT_SIZE        1
#define FM_SORT_DATE        2
#define FM_SORT_TYPE        3

/* File types */
#define FM_FILE_TYPE_FILE      0
#define FM_FILE_TYPE_FOLDER    1
#define FM_FILE_TYPE_LINK      2
#define FM_FILE_TYPE_SPECIAL   3

/* Colors (Catppuccin Mocha) */
#define FM_BG_COLOR         0x1E1E2E
#define FM_FG_COLOR         0xCDD6F4
#define FM_SIDEBAR_BG       0x181825
#define FM_SIDEBAR_ITEM     0x313244
#define FM_SIDEBAR_HOVER    0x45475A
#define FM_SIDEBAR_ACTIVE   0x89B4FA
#define FM_TOOLBAR_BG       0x313244
#define FM_TOOLBAR_BUTTON   0x45475A
#define FM_TOOLBAR_HOVER    0x585B70
#define FM_FILE_HOVER       0x313244
#define FM_FILE_SELECTED    0x45475A
#define FM_FILE_TEXT        0xCDD6F4
#define FM_FILE_FOLDER      0x89B4FA
#define FM_FILE_EXECUTABLE  0xA6E3A1
#define FM_FILE_IMAGE       0xF5C2E7
#define FM_FILE_TEXT        0xF9E2AF
#define FM_STATUSBAR_BG     0x313244

/* File entry */
typedef struct {
    char name[256];
    char path[512];
    char mime_type[128];
    long size;
    int type;
    int permissions;
    char owner[64];
    char group[64];
    int modified_time;
    int is_hidden;
    int is_selected;
    int x, y;
    int width, height;
} FMFileEntry;

/* Bookmark */
typedef struct {
    char name[64];
    char path[512];
    char icon[256];
    int is_separator;
} FMBookmark;

/* Toolbar button */
typedef struct {
    char name[64];
    char icon[256];
    char tooltip[128];
    void (*callback)();
    int enabled;
} FMToolbarButton;

/* File manager state */
typedef struct {
    int id;
    int x, y, width, height;
    char current_dir[512];
    char home_dir[256];
    char title[256];
    int view_mode;
    int sort_mode;
    int sort_ascending;
    int show_hidden;
    int show_toolbar;
    int show_sidebar;
    int show_statusbar;
    
    /* Files */
    FMFileEntry* files;
    int file_count;
    int selected_count;
    int last_selected;
    
    /* Navigation */
    char** history;
    int history_count;
    int history_index;
    char** forward_history;
    int forward_count;
    
    /* Bookmarks */
    FMBookmark bookmarks[32];
    int bookmark_count;
    
    /* Toolbar */
    FMToolbarButton toolbar_buttons[16];
    int toolbar_button_count;
    
    /* Selection */
    int selection_start_x, selection_start_y;
    int selection_end_x, selection_end_y;
    int is_selecting;
    
    /* Clipboard */
    char** clipboard_files;
    int clipboard_count;
    int clipboard_cut;
    
    /* Rename */
    int renaming;
    char rename_buffer[256];
    int rename_index;
} FileManager;

/* Function prototypes */
FileManager* fm_create(int x, int y, int width, int height);
void fm_destroy(FileManager* fm);
void fm_render(FileManager* fm);
void fm_handle_event(FileManager* fm, void* event);

/* Navigation */
void fm_navigate(FileManager* fm, const char* path);
void fm_go_back(FileManager* fm);
void fm_go_forward(FileManager* fm);
void fm_go_up(FileManager* fm);
void fm_go_home(FileManager* fm);
void fm_go_to_path(FileManager* fm, const char* path);
void fm_refresh(FileManager* fm);

/* File operations */
void fm_open_file(FileManager* fm, const char* path);
void fm_open_folder(FileManager* fm, const char* path);
void fm_create_folder(FileManager* fm, const char* name);
void fm_create_file(FileManager* fm, const char* name);
void fm_delete_selected(FileManager* fm);
void fm_copy_selected(FileManager* fm);
void fm_cut_selected(FileManager* fm);
void fm_paste(FileManager* fm);
void fm_rename(FileManager* fm, int index, const char* new_name);
void fm_select_all(FileManager* fm);
void fm_select_none(FileManager* fm);
void fm_invert_selection(FileManager* fm);

/* View */
void fm_set_view_mode(FileManager* fm, int mode);
void fm_set_sort_mode(FileManager* fm, int mode);
void fm_toggle_hidden(FileManager* fm);
void fm_toggle_toolbar(FileManager* fm);
void fm_toggle_sidebar(FileManager* fm);
void fm_toggle_statusbar(FileManager* fm);

/* Bookmarks */
void fm_add_bookmark(FileManager* fm, const char* name, const char* path);
void fm_remove_bookmark(FileManager* fm, int index);
void fm_load_bookmarks(FileManager* fm);
void fm_save_bookmarks(FileManager* fm);

/* Sorting */
void fm_sort_files(FileManager* fm);
int fm_compare_files(const void* a, const void* b);

/* Selection */
void fm_select_file(FileManager* fm, int index);
void fm_deselect_file(FileManager* fm, int index);
void fm_toggle_file_selection(FileManager* fm, int index);
int fm_get_file_at_position(FileManager* fm, int x, int y);

/* Drag and drop */
void fm_start_drag(FileManager* fm, int index, int x, int y);
void fm_end_drag(FileManager* fm, int x, int y);

/* Search */
void fm_search(FileManager* fm, const char* query);
void fm_clear_search(FileManager* fm);

/* Properties */
void fm_show_properties(FileManager* fm, const char* path);

#endif /* FILEMANAGER_H */
