/*
 * CodixOS Desktop Shell Implementation
 * Complete desktop environment with panel, taskbar, and app launcher
 */

#include "desktopshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Global state */
static DesktopShell shell;

/* Initialize desktop shell */
void shell_init() {
    memset(&shell, 0, sizeof(DesktopShell));
    
    /* Initialize panels */
    panel_init(PANEL_POSITION_TOP);
    panel_init(PANEL_POSITION_BOTTOM);
    
    /* Initialize clock */
    clock_init();
    
    /* Initialize system tray */
    tray_init();
    
    /* Initialize app launcher */
    launcher_init();
    
    /* Initialize desktop */
    desktop_init();
    
    shell.show_panel = 1;
    shell.show_desktop_icons = 1;
    shell.show_app_launcher = 0;
    shell.current_panel = PANEL_POSITION_BOTTOM;
    
    printf("[Shell] Desktop shell initialized\n");
}

/* Cleanup desktop shell */
void shell_cleanup() {
    printf("[Shell] Desktop shell cleaned up\n");
}

/* Run desktop shell */
void shell_run() {
    printf("[Shell] Starting desktop shell...\n");
    
    /* Main loop would be here */
    while (1) {
        /* Render components */
        if (shell.show_panel) {
            panel_render();
        }
        
        if (shell.show_desktop_icons) {
            desktop_render();
        }
        
        if (shell.show_app_launcher) {
            launcher_render();
        }
        
        /* Update clock */
        clock_update();
    }
}

/* ==================== PANEL ==================== */

/* Initialize panel */
void panel_init(int position) {
    Panel* panel = &shell.panels[position];
    panel->position = position;
    panel->height = PANEL_HEIGHT;
    panel->bg_color = PANEL_BG_COLOR;
    panel->text_color = PANEL_TEXT_COLOR;
    panel->auto_hide = 0;
    panel->always_on_top = 1;
    panel->visible = 1;
    
    /* Set position */
    switch (position) {
        case PANEL_POSITION_TOP:
            panel->x = 0;
            panel->y = 0;
            panel->width = 1920;
            break;
        case PANEL_POSITION_BOTTOM:
            panel->x = 0;
            panel->y = 1080 - PANEL_HEIGHT;
            panel->width = 1920;
            break;
        case PANEL_POSITION_LEFT:
            panel->x = 0;
            panel->y = 0;
            panel->width = PANEL_HEIGHT;
            panel->height = 1080;
            break;
        case PANEL_POSITION_RIGHT:
            panel->x = 1920 - PANEL_HEIGHT;
            panel->y = 0;
            panel->width = PANEL_HEIGHT;
            panel->height = 1080;
            break;
    }
}

/* Render panel */
void panel_render() {
    Panel* panel = &shell.panels[shell.current_panel];
    if (!panel->visible) return;
    
    /* Panel rendering would use SDL2/X11 */
    /* Components: */
    /* 1. Menu button (CodixOS logo) */
    /* 2. Taskbar buttons */
    /* 3. System tray */
    /* 4. Clock */
}

/* Handle panel click */
void panel_handle_click(int x, int y) {
    Panel* panel = &shell.panels[shell.current_panel];
    
    /* Check if click is in panel */
    if (x < panel->x || x >= panel->x + panel->width ||
        y < panel->y || y >= panel->y + panel->height) {
        return;
    }
    
    /* Check menu button */
    if (x < 60) {
        /* Toggle app launcher */
        shell.show_app_launcher = !shell.show_app_launcher;
        return;
    }
    
    /* Check taskbar buttons */
    int btn_x = 70;
    for (int i = 0; i < shell.taskbar_button_count; i++) {
        if (x >= btn_x && x < btn_x + 150) {
            /* Click on taskbar button */
            printf("[Shell] Clicked taskbar: %s\n", shell.taskbar_buttons[i].title);
            return;
        }
        btn_x += 160;
    }
}

/* Add taskbar button */
void panel_add_taskbar_button(int window_id, const char* title, const char* icon) {
    if (shell.taskbar_button_count >= 32) return;
    
    TaskbarButton* btn = &shell.taskbar_buttons[shell.taskbar_button_count];
    btn->window_id = window_id;
    strncpy(btn->title, title, sizeof(btn->title) - 1);
    strncpy(btn->icon, icon, sizeof(btn->icon) - 1);
    btn->active = 0;
    btn->minimized = 0;
    
    shell.taskbar_button_count++;
    printf("[Shell] Added taskbar button: %s\n", title);
}

/* Remove taskbar button */
void panel_remove_taskbar_button(int window_id) {
    for (int i = 0; i < shell.taskbar_button_count; i++) {
        if (shell.taskbar_buttons[i].window_id == window_id) {
            /* Shift remaining buttons */
            for (int j = i; j < shell.taskbar_button_count - 1; j++) {
                shell.taskbar_buttons[j] = shell.taskbar_buttons[j + 1];
            }
            shell.taskbar_button_count--;
            printf("[Shell] Removed taskbar button: %d\n", window_id);
            return;
        }
    }
}

/* Update taskbar button */
void panel_update_taskbar_button(int window_id, const char* title) {
    for (int i = 0; i < shell.taskbar_button_count; i++) {
        if (shell.taskbar_buttons[i].window_id == window_id) {
            strncpy(shell.taskbar_buttons[i].title, title, 
                    sizeof(shell.taskbar_buttons[i].title) - 1);
            return;
        }
    }
}

/* ==================== CLOCK ==================== */

/* Initialize clock */
void clock_init() {
    shell.clock.width = PANEL_CLOCK_WIDTH;
    shell.clock.format_24h = 1;
    shell.clock.show_date = 1;
    shell.clock.show_seconds = 0;
}

/* Render clock */
void clock_render() {
    /* Clock rendering would happen here */
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    
    char time_str[64];
    if (shell.clock.format_24h) {
        if (shell.clock.show_seconds) {
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tm);
        } else {
            strftime(time_str, sizeof(time_str), "%H:%M", tm);
        }
    } else {
        if (shell.clock.show_seconds) {
            strftime(time_str, sizeof(time_str), "%I:%M:%S %p", tm);
        } else {
            strftime(time_str, sizeof(time_str), "%I:%M %p", tm);
        }
    }
    
    if (shell.clock.show_date) {
        char date_str[64];
        strftime(date_str, sizeof(date_str), "%a %b %d", tm);
        /* Render date and time */
    }
}

/* Update clock */
void clock_update() {
    static time_t last_update = 0;
    time_t now = time(NULL);
    
    if (now != last_update) {
        last_update = now;
        clock_render();
    }
}

/* ==================== SYSTEM TRAY ==================== */

/* Initialize tray */
void tray_init() {
    shell.system_tray.icon_count = 0;
}

/* Add tray icon */
void tray_add_icon(const char* icon, const char* tooltip) {
    if (shell.system_tray.icon_count >= 16) return;
    
    int idx = shell.system_tray.icon_count;
    strncpy(shell.system_tray.icons[idx], icon, sizeof(shell.system_tray.icons[idx]) - 1);
    strncpy(shell.system_tray.tooltips[idx], tooltip, sizeof(shell.system_tray.tooltips[idx]) - 1);
    shell.system_tray.icon_count++;
}

/* Remove tray icon */
void tray_remove_icon(int index) {
    if (index < 0 || index >= shell.system_tray.icon_count) return;
    
    for (int i = index; i < shell.system_tray.icon_count - 1; i++) {
        strcpy(shell.system_tray.icons[i], shell.system_tray.icons[i + 1]);
        strcpy(shell.system_tray.tooltips[i], shell.system_tray.tooltips[i + 1]);
    }
    shell.system_tray.icon_count--;
}

/* Render tray */
void tray_render() {
    /* System tray rendering would happen here */
}

/* ==================== APP LAUNCHER ==================== */

/* Initialize launcher */
void launcher_init() {
    shell.launcher_app_count = 0;
    
    /* Add default apps */
    launcher_add_app("Terminal", "Command line interface", "terminal.png", "codix-term", "System");
    launcher_add_app("File Manager", "Browse files and folders", "filemanager.png", "codix-files", "System");
    launcher_add_app("Text Editor", "Edit text files", "texteditor.png", "codix-editor", "Utilities");
    launcher_add_app("Settings", "System settings", "settings.png", "codix-settings", "System");
    launcher_add_app("System Monitor", "Monitor system resources", "monitor.png", "codix-monitor", "System");
    launcher_add_app("Calculator", "Perform calculations", "calculator.png", "codix-calc", "Utilities");
    launcher_add_app("Web Browser", "Browse the internet", "browser.png", "codix-browser", "Internet");
    launcher_add_app("Package Manager", "Manage software packages", "package.png", "codix-pkg", "System");
}

/* Add app to launcher */
void launcher_add_app(const char* name, const char* desc, const char* icon, 
                      const char* cmd, const char* category) {
    if (shell.launcher_app_count >= MAX_LAUNCHER_APPS) return;
    
    AppLauncherEntry* app = &shell.launcher_apps[shell.launcher_app_count];
    strncpy(app->name, name, sizeof(app->name) - 1);
    strncpy(app->description, desc, sizeof(app->description) - 1);
    strncpy(app->icon, icon, sizeof(app->icon) - 1);
    strncpy(app->command, cmd, sizeof(app->command) - 1);
    strncpy(app->category, category, sizeof(app->category) - 1);
    app->frequency = 0;
    
    shell.launcher_app_count++;
}

/* Remove app from launcher */
void launcher_remove_app(int index) {
    if (index < 0 || index >= shell.launcher_app_count) return;
    
    for (int i = index; i < shell.launcher_app_count - 1; i++) {
        shell.launcher_apps[i] = shell.launcher_apps[i + 1];
    }
    shell.launcher_app_count--;
}

/* Render launcher */
void launcher_render() {
    if (!shell.show_app_launcher) return;
    
    /* Launcher rendering would use SDL2/X11 */
    /* Components: */
    /* 1. Search bar */
    /* 2. Category tabs */
    /* 3. App grid */
    /* 4. App info */
    
    printf("[Shell] Rendering app launcher with %d apps\n", shell.launcher_app_count);
}

/* Show launcher */
void launcher_show() {
    shell.show_app_launcher = 1;
}

/* Hide launcher */
void launcher_hide() {
    shell.show_app_launcher = 0;
}

/* Handle launcher click */
int launcher_handle_click(int x, int y) {
    if (!shell.show_app_launcher) return -1;
    
    /* Calculate which app was clicked */
    /* Grid layout: 4 columns */
    int col = (x - 50) / 150;
    int row = (y - 100) / 150;
    int index = row * 4 + col;
    
    if (index >= 0 && index < shell.launcher_app_count) {
        printf("[Shell] Launching: %s (%s)\n", 
               shell.launcher_apps[index].name,
               shell.launcher_apps[index].command);
        launcher_hide();
        return index;
    }
    
    return -1;
}

/* Find app by name */
AppLauncherEntry* launcher_find_app(const char* name) {
    for (int i = 0; i < shell.launcher_app_count; i++) {
        if (strcmp(shell.launcher_apps[i].name, name) == 0) {
            return &shell.launcher_apps[i];
        }
    }
    return NULL;
}

/* ==================== DESKTOP ICONS ==================== */

/* Initialize desktop */
void desktop_init() {
    shell.desktop_icon_count = 0;
    
    /* Add default desktop icons */
    desktop_add_icon("Home", "folder-home.png", "/home/codix", 50, 50, 1);
    desktop_add_icon("Documents", "folder-documents.png", "/home/codix/Documents", 50, 150, 1);
    desktop_add_icon("Downloads", "folder-downloads.png", "/home/codix/Downloads", 50, 250, 1);
    desktop_add_icon("Terminal", "terminal.png", "codix-term", 50, 350, 2);
    desktop_add_icon("File Manager", "filemanager.png", "codix-files", 50, 450, 2);
}

/* Add desktop icon */
void desktop_add_icon(const char* name, const char* icon, const char* path, 
                      int x, int y, int type) {
    if (shell.desktop_icon_count >= 64) return;
    
    DesktopIcon* di = &shell.desktop_icons[shell.desktop_icon_count];
    strncpy(di->name, name, sizeof(di->name) - 1);
    strncpy(di->icon, icon, sizeof(di->icon) - 1);
    strncpy(di->path, path, sizeof(di->path) - 1);
    di->x = x;
    di->y = y;
    di->selected = 0;
    di->type = type;
    
    shell.desktop_icon_count++;
}

/* Remove desktop icon */
void desktop_remove_icon(int index) {
    if (index < 0 || index >= shell.desktop_icon_count) return;
    
    for (int i = index; i < shell.desktop_icon_count - 1; i++) {
        shell.desktop_icons[i] = shell.desktop_icons[i + 1];
    }
    shell.desktop_icon_count--;
}

/* Render desktop icons */
void desktop_render() {
    if (!shell.show_desktop_icons) return;
    
    /* Desktop icon rendering would use SDL2/X11 */
    /* Each icon: */
    /* 1. Icon image */
    /* 2. Name text */
    /* 3. Selection highlight */
}

/* Handle desktop click */
void desktop_handle_click(int x, int y) {
    /* Check if click is on an icon */
    for (int i = 0; i < shell.desktop_icon_count; i++) {
        DesktopIcon* di = &shell.desktop_icons[i];
        if (x >= di->x && x < di->x + DESKTOP_ICON_SIZE &&
            y >= di->y && y < di->y + DESKTOP_ICON_SIZE + DESKTOP_ICON_TEXT_HEIGHT) {
            desktop_select_icon(i);
            return;
        }
    }
    
    /* Click on empty desktop */
    desktop_deselect_all();
}

/* Handle desktop double click */
void desktop_handle_double_click(int x, int y) {
    for (int i = 0; i < shell.desktop_icon_count; i++) {
        DesktopIcon* di = &shell.desktop_icons[i];
        if (x >= di->x && x < di->x + DESKTOP_ICON_SIZE &&
            y >= di->y && y < di->y + DESKTOP_ICON_SIZE + DESKTOP_ICON_TEXT_HEIGHT) {
            printf("[Shell] Opening: %s (%s)\n", di->name, di->path);
            return;
        }
    }
}

/* Handle icon drag */
void desktop_handle_drag(int icon_index, int x, int y) {
    if (icon_index < 0 || icon_index >= shell.desktop_icon_count) return;
    
    shell.desktop_icons[icon_index].x = x;
    shell.desktop_icons[icon_index].y = y;
}

/* Select icon */
void desktop_select_icon(int index) {
    desktop_deselect_all();
    if (index >= 0 && index < shell.desktop_icon_count) {
        shell.desktop_icons[index].selected = 1;
    }
}

/* Deselect all icons */
void desktop_deselect_all() {
    for (int i = 0; i < shell.desktop_icon_count; i++) {
        shell.desktop_icons[i].selected = 0;
    }
}

/* ==================== CONTEXT MENU ==================== */

static MenuItem* context_menu_items = NULL;
static int context_menu_count = 0;
static int context_menu_visible = 0;
static int context_menu_x = 0;
static int context_menu_y = 0;

/* Show context menu */
void context_menu_show(int x, int y, MenuItem* items, int count) {
    context_menu_items = items;
    context_menu_count = count;
    context_menu_x = x;
    context_menu_y = y;
    context_menu_visible = 1;
}

/* Hide context menu */
void context_menu_hide() {
    context_menu_visible = 0;
    context_menu_items = NULL;
    context_menu_count = 0;
}

/* Render context menu */
void context_menu_render() {
    if (!context_menu_visible || !context_menu_items) return;
    
    /* Context menu rendering would use SDL2/X11 */
    /* Menu items with icons, text, and keyboard shortcuts */
}

/* Handle context menu click */
int context_menu_handle_click(int x, int y) {
    if (!context_menu_visible) return -1;
    
    /* Check if click is in menu */
    if (x < context_menu_x || x > context_menu_x + 200 ||
        y < context_menu_y || y > context_menu_y + context_menu_count * 30) {
        context_menu_hide();
        return -1;
    }
    
    /* Find which item was clicked */
    int index = (y - context_menu_y) / 30;
    if (index >= 0 && index < context_menu_count) {
        if (context_menu_items[index].enabled && !context_menu_items[index].is_separator) {
            printf("[Shell] Menu item clicked: %s\n", context_menu_items[index].name);
            if (context_menu_items[index].callback) {
                context_menu_items[index].callback();
            }
            context_menu_hide();
            return index;
        }
    }
    
    return -1;
}
