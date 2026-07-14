/*
 * CodixOS Window Manager Header
 * Lightweight X11/SDL2 window manager
 */

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

/* Window states */
#define WINDOW_STATE_NORMAL    0
#define WINDOW_STATE_MAXIMIZED 1
#define WINDOW_STATE_MINIMIZED 2
#define WINDOW_STATE_FULLSCREEN 3

/* Window decorations */
#define WM_TITLE_HEIGHT        30
#define WM_BORDER_WIDTH        2
#define WM_BUTTON_SIZE         20

/* Colors - Default Theme */
#define WM_COLOR_BG            0x1E1E2E
#define WM_COLOR_TITLEBAR      0x313244
#define WM_COLOR_TITLEBAR_ACTIVE 0x45475A
#define WM_COLOR_TITLEBAR_TEXT 0xCDD6F4
#define WM_COLOR_BORDER        0x585B70
#define WM_COLOR_BORDER_ACTIVE 0x89B4FA
#define WM_COLOR_BUTTON_CLOSE  0xF38BA8
#define WM_COLOR_BUTTON_MIN    0xF9E2AF
#define WM_COLOR_BUTTON_MAX    0xA6E3A1
#define WM_COLOR_BUTTON_HOVER  0x6C7086

/* Window structure */
typedef struct WMWindow {
    int id;
    int x, y, width, height;
    int min_width, min_height;
    int state;
    int focused;
    int decorated;
    int resizable;
    int movable;
    int closable;
    int minimizable;
    int maximizable;
    char title[256];
    char app_name[64];
    int (*draw)(struct WMWindow*);
    int (*handle_event)(struct WMWindow*, void*);
    void* user_data;
    struct WMWindow* next;
    struct WMWindow* prev;
} WMWindow;

/* Monitor structure */
typedef struct {
    int x, y, width, height;
    int refresh_rate;
    char name[32];
} WMMonitor;

/* Desktop state */
typedef struct {
    WMWindow* windows;
    WMWindow* focused_window;
    WMWindow* last_focused;
    WMMonitor monitors[8];
    int monitor_count;
    int current_workspace;
    int workspaces;
    int width, height;
    int running;
    int drag_active;
    int drag_offset_x, drag_offset_y;
    int resize_active;
    int resize_edge;
    unsigned int desktop_color;
    unsigned int panel_color;
    unsigned int panel_text_color;
    int panel_height;
    int show_desktop_icons;
    int compositor_enabled;
} WMDesktop;

/* Function prototypes */
void wm_init(int width, int height);
void wm_cleanup();
void wm_run();
void wm_stop();

/* Window management */
WMWindow* wm_create_window(const char* title, int x, int y, int width, int height);
void wm_destroy_window(WMWindow* win);
void wm_show_window(WMWindow* win);
void wm_hide_window(WMWindow* win);
void wm_focus_window(WMWindow* win);
void wm_move_window(WMWindow* win, int x, int y);
void wm_resize_window(WMWindow* win, int width, int height);
void wm_maximize_window(WMWindow* win);
void wm_minimize_window(WMWindow* win);
void wm_restore_window(WMWindow* win);
WMWindow* wm_find_window(int id);
WMWindow* wm_get_focused_window();

/* Rendering */
void wm_render();
void wm_render_window(WMWindow* win);
void wm_render_titlebar(WMWindow* win);
void wm_render_desktop();
void wm_render_panel();
void wm_render_desktop_icons();

/* Event handling */
void wm_handle_event(void* event);
void wm_handle_key_event(int key, int modifiers);
void wm_handle_mouse_event(int x, int y, int button, int type);
void wm_handle_window_event(WMWindow* win, int event_type);

/* Desktop */
void wm_set_desktop_color(unsigned int color);
void wm_add_desktop_icon(const char* name, const char* icon_path, int x, int y);
void wm_refresh_desktop();

/* Workspace management */
void wm_switch_workspace(int workspace);
void wm_move_to_workspace(WMWindow* win, int workspace);

/* Keyboard shortcuts */
void wm_register_shortcut(int key, int modifiers, void (*callback)());
void wm_unregister_shortcut(int key, int modifiers);

/* Utility */
int wm_get_window_count();
WMWindow* wm_get_window_list();
int wm_is_point_in_window(int x, int y, WMWindow* win);
int wm_get_resize_edge(WMWindow* win, int x, int y);

#endif /* WINDOWMANAGER_H */
