/*
 * CodixOS Desktop Shell Header
 * Panel, taskbar, and desktop components
 */

#ifndef DESKTOPSHELL_H
#define DESKTOPSHELL_H

/* Panel position */
#define PANEL_POSITION_TOP    0
#define PANEL_POSITION_BOTTOM 1
#define PANEL_POSITION_LEFT   2
#define PANEL_POSITION_RIGHT  3

/* Panel size */
#define PANEL_HEIGHT          36
#define PANEL_ICON_SIZE       24
#define PANEL_CLOCK_WIDTH     100

/* Menu */
#define MAX_MENU_ITEMS        32
#define MAX_MENU_DEPTH        4

/* App launcher */
#define MAX_LAUNCHER_APPS     64

/* Desktop icon */
#define DESKTOP_ICON_SIZE     64
#define DESKTOP_ICON_TEXT_HEIGHT 20

/* Colors (Catppuccin Mocha) */
#define PANEL_BG_COLOR        0x313244
#define PANEL_TEXT_COLOR      0xCDD6F4
#define PANEL_HOVER_COLOR     0x45475A
#define PANEL_ACTIVE_COLOR    0x89B4FA
#define PANEL_INDICATOR_COLOR 0xA6E3A1
#define MENU_BG_COLOR         0x313244
#define MENU_TEXT_COLOR       0xCDD6F4
#define MENU_HOVER_COLOR      0x45475A
#define MENU_SEPARATOR_COLOR  0x585B70

/* Menu item structure */
typedef struct {
    char name[64];
    char icon[256];
    char command[256];
    int enabled;
    int visible;
    int is_separator;
    int has_submenu;
    struct MenuItem* submenu;
    void (*callback)();
} MenuItem;

/* Panel structure */
typedef struct {
    int position;
    int width;
    int height;
    int x, y;
    unsigned int bg_color;
    unsigned int text_color;
    int auto_hide;
    int always_on_top;
    int visible;
} Panel;

/* Taskbar button */
typedef struct {
    int window_id;
    char title[256];
    char icon[256];
    int active;
    int minimized;
} TaskbarButton;

/* Clock */
typedef struct {
    int x, y;
    int width;
    int format_24h;
    int show_date;
    int show_seconds;
} Clock;

/* System tray */
typedef struct {
    int icon_count;
    char icons[16][256];
    char tooltips[16][256];
} SystemTray;

/* App launcher entry */
typedef struct {
    char name[64];
    char description[128];
    char icon[256];
    char command[256];
    char category[64];
    int frequency;
} AppLauncherEntry;

/* Desktop icon */
typedef struct {
    char name[64];
    char icon[256];
    char path[256];
    int x, y;
    int selected;
    int type; /* 0=file, 1=folder, 2=app, 3=link */
} DesktopIcon;

/* Desktop shell state */
typedef struct {
    Panel panels[4];
    TaskbarButton taskbar_buttons[32];
    int taskbar_button_count;
    Clock clock;
    SystemTray system_tray;
    AppLauncherEntry launcher_apps[MAX_LAUNCHER_APP];
    int launcher_app_count;
    DesktopIcon desktop_icons[64];
    int desktop_icon_count;
    int show_panel;
    int show_desktop_icons;
    int show_app_launcher;
    int current_panel;
} DesktopShell;

/* Function prototypes */
void shell_init();
void shell_cleanup();
void shell_run();

/* Panel */
void panel_init(int position);
void panel_render();
void panel_handle_click(int x, int y);
void panel_add_taskbar_button(int window_id, const char* title, const char* icon);
void panel_remove_taskbar_button(int window_id);
void panel_update_taskbar_button(int window_id, const char* title);

/* Clock */
void clock_init();
void clock_render();
void clock_update();

/* System tray */
void tray_init();
void tray_add_icon(const char* icon, const char* tooltip);
void tray_remove_icon(int index);
void tray_render();

/* App launcher */
void launcher_init();
void launcher_add_app(const char* name, const char* desc, const char* icon, const char* cmd, const char* category);
void launcher_remove_app(int index);
void launcher_render();
void launcher_show();
void launcher_hide();
int launcher_handle_click(int x, int y);
AppLauncherEntry* launcher_find_app(const char* name);

/* Desktop icons */
void desktop_init();
void desktop_add_icon(const char* name, const char* icon, const char* path, int x, int y, int type);
void desktop_remove_icon(int index);
void desktop_render();
void desktop_handle_click(int x, int y);
void desktop_handle_double_click(int x, int y);
void desktop_handle_drag(int icon_index, int x, int y);
void desktop_select_icon(int index);
void desktop_deselect_all();

/* Right-click menu */
void context_menu_show(int x, int y, MenuItem* items, int count);
void context_menu_hide();
void context_menu_render();
int context_menu_handle_click(int x, int y);

#endif /* DESKTOPSHELL_H */
