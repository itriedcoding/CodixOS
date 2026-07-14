/*
 * CodixOS Web Browser Header
 * Simple web browser application
 */

#ifndef BROWSER_H
#define BROWSER_H

/* Browser configuration */
#define BROWSER_DEFAULT_WIDTH    1024
#define BROWSER_DEFAULT_HEIGHT   768
#define BROWSER_MIN_WIDTH        640
#define BROWSER_MIN_HEIGHT       480
#define BROWSER_TOOLBAR_HEIGHT   40
#define BROWSER_TAB_HEIGHT       35
#define BROWSER_STATUSBAR_HEIGHT 25

/* Colors (Catppuccin Mocha) */
#define BROWSER_BG_COLOR         0x1E1E2E
#define BROWSER_FG_COLOR         0xCDD6F4
#define BROWSER_TOOLBAR_BG       0x313244
#define BROWSER_TAB_BG           0x181825
#define BROWSER_TAB_ACTIVE       0x313244
#define BROWSER_TAB_HOVER        0x45475a
#define BROWSER_URL_BG           0x45475a
#define BROWSER_URL_TEXT         0xcdd6f4
#define BROWSER_CONTENT_BG       0xffffff
#define BROWSER_LINK_COLOR       0x89b4fa
#define BROWSER_VISITED_COLOR    0xf5c2e7
#define BROWSER_STATUS_BG        0x313244

/* Browser tab */
typedef struct {
    int id;
    char title[256];
    char url[1024];
    char* content;
    int content_length;
    int scroll_x, scroll_y;
    int loading;
    int can_go_back;
    int can_go_forward;
    char** history;
    int history_count;
    int history_index;
    int active;
} BrowserTab;

/* Browser bookmark */
typedef struct {
    char name[128];
    char url[1024];
    char icon[256];
    int is_folder;
    int parent_id;
} BrowserBookmark;

/* Browser state */
typedef struct {
    int id;
    int x, y, width, height;
    
    /* Tabs */
    BrowserTab tabs[32];
    int tab_count;
    int active_tab;
    
    /* UI */
    int show_toolbar;
    int show_bookmarks;
    int show_statusbar;
    
    /* Bookmarks */
    BrowserBookmark bookmarks[128];
    int bookmark_count;
    
    /* Settings */
    int home_button;
    int do_not_track;
    int javascript_enabled;
    int cookies_enabled;
    char home_page[1024];
    char search_engine[256];
    
    /* State */
    int is_fullscreen;
    int zoom_level;
    char status_text[256];
} Browser;

/* Function prototypes */
Browser* browser_create(int x, int y, int width, int height);
void browser_destroy(Browser* browser);
void browser_render(Browser* browser);
void browser_handle_event(Browser* browser, void* event);

/* Navigation */
void browser_navigate(Browser* browser, const char* url);
void browser_go_back(Browser* browser);
void browser_go_forward(Browser* browser);
void browser_reload(Browser* browser);
void browser_stop(Browser* browser);
void browser_go_home(Browser* browser);

/* Tabs */
void browser_new_tab(Browser* browser, const char* url);
void browser_close_tab(Browser* browser, int index);
void browser_set_active_tab(Browser* browser, int index);
void browser_move_tab(Browser* browser, int from, int to);

/* Bookmarks */
void browser_add_bookmark(Browser* browser, const char* name, const char* url);
void browser_remove_bookmark(Browser* browser, int index);
void browser_load_bookmarks(Browser* browser);
void browser_save_bookmarks(Browser* browser);

/* Settings */
void browser_set_home_page(Browser* browser, const char* url);
void browser_set_search_engine(Browser* browser, const char* engine);
void browser_set_zoom(Browser* browser, int zoom);

/* History */
void browser_add_history(Browser* browser, const char* url);
void browser_clear_history(Browser* browser);

/* Content */
void browser_load_content(Browser* browser, const char* url);
void browser_render_content(Browser* browser);

/* Download */
void browser_download(Browser* browser, const char* url);

/* Find */
void browser_find(Browser* browser, const char* query);
void browser_find_next(Browser* browser);
void browser_find_previous(Browser* browser);

/* Print */
void browser_print(Browser* browser);

/* View source */
void browser_view_source(Browser* browser);

#endif /* BROWSER_H */
