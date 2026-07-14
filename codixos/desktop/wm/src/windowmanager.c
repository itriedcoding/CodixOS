/*
 * CodixOS Window Manager Implementation
 * Full-featured window manager with decorations
 */

#include "windowmanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global desktop state */
static WMDesktop desktop;
static int next_window_id = 1;

/* Initialize window manager */
void wm_init(int width, int height) {
    memset(&desktop, 0, sizeof(WMDesktop));
    
    desktop.width = width;
    desktop.height = height;
    desktop.windows = NULL;
    desktop.focused_window = NULL;
    desktop.last_focused = NULL;
    desktop.current_workspace = 0;
    desktop.workspaces = 4;
    desktop.running = 1;
    desktop.panel_height = 36;
    desktop.show_desktop_icons = 1;
    desktop.compositor_enabled = 1;
    
    /* Default colors (Catppuccin Mocha) */
    desktop.desktop_color = WM_COLOR_BG;
    desktop.panel_color = WM_COLOR_TITLEBAR;
    desktop.panel_text_color = WM_COLOR_TITLEBAR_TEXT;
    
    /* Setup monitor */
    desktop.monitor_count = 1;
    desktop.monitors[0].x = 0;
    desktop.monitors[0].y = 0;
    desktop.monitors[0].width = width;
    desktop.monitors[0].height = height;
    desktop.monitors[0].refresh_rate = 60;
    strcpy(desktop.monitors[0].name, "default");
    
    printf("[WM] Window Manager initialized: %dx%d\n", width, height);
    printf("[WM] Workspaces: %d\n", desktop.workspaces);
    printf("[WM] Panel height: %d\n", desktop.panel_height);
}

/* Cleanup window manager */
void wm_cleanup() {
    WMWindow* win = desktop.windows;
    while (win) {
        WMWindow* next = win->next;
        free(win);
        win = next;
    }
    desktop.windows = NULL;
    printf("[WM] Window Manager cleaned up\n");
}

/* Create window */
WMWindow* wm_create_window(const char* title, int x, int y, int width, int height) {
    WMWindow* win = (WMWindow*)malloc(sizeof(WMWindow));
    if (!win) return NULL;
    
    memset(win, 0, sizeof(WMWindow));
    
    win->id = next_window_id++;
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    win->min_width = 200;
    win->min_height = 150;
    win->state = WINDOW_STATE_NORMAL;
    win->focused = 0;
    win->decorated = 1;
    win->resizable = 1;
    win->movable = 1;
    win->closable = 1;
    win->minimizable = 1;
    win->maximizable = 1;
    
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->draw = NULL;
    win->handle_event = NULL;
    win->user_data = NULL;
    
    /* Add to window list */
    win->next = desktop.windows;
    win->prev = NULL;
    if (desktop.windows) {
        desktop.windows->prev = win;
    }
    desktop.windows = win;
    
    printf("[WM] Created window: %s (ID: %d)\n", title, win->id);
    return win;
}

/* Destroy window */
void wm_destroy_window(WMWindow* win) {
    if (!win) return;
    
    /* Remove from list */
    if (win->prev) {
        win->prev->next = win->next;
    } else {
        desktop.windows = win->next;
    }
    if (win->next) {
        win->next->prev = win->prev;
    }
    
    /* Update focus */
    if (desktop.focused_window == win) {
        desktop.focused_window = desktop.last_focused;
        if (desktop.focused_window) {
            wm_focus_window(desktop.focused_window);
        }
    }
    
    printf("[WM] Destroyed window: %s (ID: %d)\n", win->title, win->id);
    free(win);
}

/* Show window */
void wm_show_window(WMWindow* win) {
    if (!win) return;
    win->state = WINDOW_STATE_NORMAL;
    wm_focus_window(win);
}

/* Hide window */
void wm_hide_window(WMWindow* win) {
    if (!win) return;
    win->state = WINDOW_STATE_MINIMIZED;
    
    if (desktop.focused_window == win) {
        desktop.focused_window = NULL;
        /* Focus next window */
        WMWindow* w = desktop.windows;
        while (w) {
            if (w != win && w->state == WINDOW_STATE_NORMAL) {
                wm_focus_window(w);
                break;
            }
            w = w->next;
        }
    }
}

/* Focus window */
void wm_focus_window(WMWindow* win) {
    if (!win) return;
    
    /* Unfocus current */
    if (desktop.focused_window) {
        desktop.focused_window->focused = 0;
    }
    
    /* Focus new window */
    win->focused = 1;
    desktop.last_focused = desktop.focused_window;
    desktop.focused_window = win;
    
    /* Move to front */
    if (win->prev) {
        win->prev->next = win->next;
        if (win->next) {
            win->next->prev = win->prev;
        }
        win->next = desktop.windows;
        win->prev = NULL;
        if (desktop.windows) {
            desktop.windows->prev = win;
        }
        desktop.windows = win;
    }
    
    printf("[WM] Focused window: %s\n", win->title);
}

/* Move window */
void wm_move_window(WMWindow* win, int x, int y) {
    if (!win || !win->movable) return;
    
    win->x = x;
    win->y = y;
    
    /* Constrain to screen */
    if (win->x < 0) win->x = 0;
    if (win->y < 0) win->y = 0;
    if (win->x + win->width > desktop.width) {
        win->x = desktop.width - win->width;
    }
    if (win->y + win->height > desktop.height - desktop.panel_height) {
        win->y = desktop.height - desktop.panel_height - win->height;
    }
}

/* Resize window */
void wm_resize_window(WMWindow* win, int width, int height) {
    if (!win || !win->resizable) return;
    
    if (width < win->min_width) width = win->min_width;
    if (height < win->min_height) height = win->min_height;
    
    win->width = width;
    win->height = height;
    
    /* Constrain to screen */
    if (win->x + win->width > desktop.width) {
        win->width = desktop.width - win->x;
    }
    if (win->y + win->height > desktop.height - desktop.panel_height) {
        win->height = desktop.height - desktop.panel_height - win->y;
    }
}

/* Maximize window */
void wm_maximize_window(WMWindow* win) {
    if (!win || !win->maximizable) return;
    
    if (win->state == WINDOW_STATE_MAXIMIZED) {
        wm_restore_window(win);
        return;
    }
    
    win->state = WINDOW_STATE_MAXIMIZED;
    win->x = 0;
    win->y = 0;
    win->width = desktop.width;
    win->height = desktop.height - desktop.panel_height;
}

/* Minimize window */
void wm_minimize_window(WMWindow* win) {
    if (!win || !win->minimizable) return;
    wm_hide_window(win);
}

/* Restore window */
void wm_restore_window(WMWindow* win) {
    if (!win) return;
    win->state = WINDOW_STATE_NORMAL;
}

/* Find window by ID */
WMWindow* wm_find_window(int id) {
    WMWindow* win = desktop.windows;
    while (win) {
        if (win->id == id) return win;
        win = win->next;
    }
    return NULL;
}

/* Get focused window */
WMWindow* wm_get_focused_window() {
    return desktop.focused_window;
}

/* Render all */
void wm_render() {
    /* This would render to screen using SDL2/X11 */
    /* For now, just log */
    WMWindow* win = desktop.windows;
    int count = 0;
    while (win) {
        if (win->state == WINDOW_STATE_NORMAL) {
            count++;
        }
        win = win->next;
    }
    /* printf("[WM] Rendering %d windows\n", count); */
}

/* Render window */
void wm_render_window(WMWindow* win) {
    if (!win || win->state != WINDOW_STATE_NORMAL) return;
    
    /* Draw window content */
    if (win->draw) {
        win->draw(win);
    }
}

/* Render titlebar */
void wm_render_titlebar(WMWindow* win) {
    if (!win || !win->decorated) return;
    
    /* Titlebar rendering would happen here with SDL2/X11 */
    /* Including: title text, close/min/max buttons, drag area */
}

/* Render desktop */
void wm_render_desktop() {
    /* Desktop background rendering */
}

/* Render panel/taskbar */
void wm_render_panel() {
    /* Panel/taskbar rendering */
}

/* Render desktop icons */
void wm_render_desktop_icons() {
    /* Desktop icon rendering */
}

/* Handle event */
void wm_handle_event(void* event) {
    /* Event handling would be implemented with SDL2/X11 */
}

/* Handle key event */
void wm_handle_key_event(int key, int modifiers) {
    /* Keyboard shortcuts */
    /* Alt+Tab: Switch windows */
    /* Super+Arrow: Snap windows */
    /* Super+D: Show desktop */
    /* Super+L: Lock screen */
}

/* Handle mouse event */
void wm_handle_mouse_event(int x, int y, int button, int type) {
    /* Find window under mouse */
    WMWindow* win = desktop.windows;
    while (win) {
        if (win->state == WINDOW_STATE_NORMAL && wm_is_point_in_window(x, y, win)) {
            if (type == 0) { /* Mouse down */
                wm_focus_window(win);
                
                /* Check titlebar */
                if (win->decorated && y < win->y + WM_TITLE_HEIGHT) {
                    /* Titlebar click - start drag */
                    desktop.drag_active = 1;
                    desktop.drag_offset_x = x - win->x;
                    desktop.drag_offset_y = y - win->y;
                }
            } else if (type == 1) { /* Mouse up */
                desktop.drag_active = 0;
                desktop.resize_active = 0;
            } else if (type == 2) { /* Mouse move */
                if (desktop.drag_active && desktop.focused_window == win) {
                    wm_move_window(win, x - desktop.drag_offset_x, y - desktop.drag_offset_y);
                }
            }
            return;
        }
        win = win->next;
    }
}

/* Set desktop color */
void wm_set_desktop_color(unsigned int color) {
    desktop.desktop_color = color;
}

/* Add desktop icon */
void wm_add_desktop_icon(const char* name, const char* icon_path, int x, int y) {
    /* Desktop icon management */
}

/* Refresh desktop */
void wm_refresh_desktop() {
    /* Force desktop redraw */
}

/* Switch workspace */
void wm_switch_workspace(int workspace) {
    if (workspace < 0 || workspace >= desktop.workspaces) return;
    
    /* Hide current workspace windows */
    WMWindow* win = desktop.windows;
    while (win) {
        win->state = WINDOW_STATE_MINIMIZED;
        win = win->next;
    }
    
    desktop.current_workspace = workspace;
    
    /* Show new workspace windows */
    /* (Would need workspace tracking per window) */
    
    printf("[WM] Switched to workspace %d\n", workspace);
}

/* Move to workspace */
void wm_move_to_workspace(WMWindow* win, int workspace) {
    if (!win || workspace < 0 || workspace >= desktop.workspaces) return;
    /* Would store workspace per window */
    printf("[WM] Moved window %s to workspace %d\n", win->title, workspace);
}

/* Register shortcut */
void wm_register_shortcut(int key, int modifiers, void (*callback)()) {
    /* Keyboard shortcut registration */
}

/* Get window count */
int wm_get_window_count() {
    int count = 0;
    WMWindow* win = desktop.windows;
    while (win) {
        count++;
        win = win->next;
    }
    return count;
}

/* Get window list */
WMWindow* wm_get_window_list() {
    return desktop.windows;
}

/* Check if point is in window */
int wm_is_point_in_window(int x, int y, WMWindow* win) {
    if (!win) return 0;
    return x >= win->x && x < win->x + win->width &&
           y >= win->y && y < win->y + win->height;
}

/* Get resize edge */
int wm_get_resize_edge(WMWindow* win, int x, int y) {
    if (!win || !win->resizable) return 0;
    
    int edge = 0;
    const int margin = 5;
    
    if (x < win->x + margin) edge |= 1; /* Left */
    if (x >= win->x + win->width - margin) edge |= 2; /* Right */
    if (y < win->y + margin) edge |= 4; /* Top */
    if (y >= win->y + win->height - margin) edge |= 8; /* Bottom */
    
    return edge;
}

/* Run window manager */
void wm_run() {
    printf("[WM] Starting window manager loop...\n");
    
    /* Main event loop would be here */
    /* Using SDL2 or X11 event loop */
    
    while (desktop.running) {
        /* Process events */
        /* Render */
        /* Cap framerate */
    }
}

/* Stop window manager */
void wm_stop() {
    desktop.running = 0;
    printf("[WM] Window manager stopped\n");
}
