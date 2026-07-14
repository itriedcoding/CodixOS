/*
 * CodixOS Settings Panel Header
 * System settings application
 */

#ifndef SETTINGS_H
#define SETTINGS_H

/* Settings categories */
#define SETTINGS_CATEGORY_APPEARANCE  0
#define SETTINGS_CATEGORY_DISPLAY     1
#define SETTINGS_CATEGORY_SOUND       2
#define SETTINGS_CATEGORY_NETWORK     3
#define SETTINGS_CATEGORY_BLUETOOTH   4
#define SETTINGS_CATEGORY_POWER       5
#define SETTINGS_CATEGORY_USERS       6
#define SETTINGS_CATEGORY_SYSTEM      7

/* Settings window */
#define SETTINGS_DEFAULT_WIDTH    800
#define SETTINGS_DEFAULT_HEIGHT   600
#define SETTINGS_SIDEBAR_WIDTH    200
#define SETTINGS_CONTENT_MARGIN   20

/* Colors (Catppuccin Mocha) */
#define SETTINGS_BG_COLOR         0x1E1E2E
#define SETTINGS_FG_COLOR         0xCDD6F4
#define SETTINGS_SIDEBAR_BG       0x181825
#define SETTINGS_SIDEBAR_ITEM     0x313244
#define SETTINGS_SIDEBAR_HOVER    0x45475A
#define SETTINGS_SIDEBAR_ACTIVE   0x89B4FA
#define SETTINGS_CONTENT_BG       0x1E1E2E
#define SETTINGS_WIDGET_BG        0x313244
#define SETTINGS_WIDGET_HOVER     0x45475A
#define SETTINGS_ACCENT_COLOR     0x89B4FA
#define SETTINGS_TEXT_COLOR       0xCDD6F4

/* Setting item types */
#define SETTING_TYPE_NONE         0
#define SETTING_TYPE_TOGGLE       1
#define SETTING_TYPE_SLIDER       2
#define SETTING_TYPE_COMBOBOX     3
#define SETTING_TYPE_TEXTBOX      4
#define SETTING_TYPE_COLOR        5
#define SETTING_TYPE_FILE         6
#define SETTING_TYPE_BUTTON       7

/* Setting item */
typedef struct {
    char name[128];
    char description[256];
    int type;
    union {
        int toggle_value;
        int slider_value;
        int slider_min;
        int slider_max;
        int combobox_value;
        char text_value[256];
        unsigned int color_value;
        char file_path[512];
    };
    char** combobox_options;
    int combobox_option_count;
    void (*callback)(struct SettingItem*);
} SettingItem;

/* Settings page */
typedef struct {
    char name[64];
    char icon[256];
    SettingItem items[32];
    int item_count;
} SettingsPage;

/* Settings state */
typedef struct {
    int id;
    int x, y, width, height;
    int selected_category;
    int scroll_y;
    
    /* Pages */
    SettingsPage pages[16];
    int page_count;
    
    /* UI state */
    int hovered_item;
    int editing_item;
    char edit_buffer[256];
} Settings;

/* Function prototypes */
Settings* settings_create(int x, int y, int width, int height);
void settings_destroy(Settings* settings);
void settings_render(Settings* settings);
void settings_handle_event(Settings* settings, void* event);

/* Pages */
void settings_add_page(Settings* settings, const char* name, const char* icon);
SettingsPage* settings_get_page(Settings* settings, int index);
void settings_set_page(Settings* settings, int index);

/* Items */
void settings_add_toggle(Settings* settings, const char* name, const char* desc, int default_value);
void settings_add_slider(Settings* settings, const char* name, const char* desc, int min, int max, int default_value);
void settings_add_combobox(Settings* settings, const char* name, const char* desc, char** options, int count, int default_value);
void settings_add_textbox(Settings* settings, const char* name, const char* desc, const char* default_value);
void settings_add_color(Settings* settings, const char* name, const char* desc, unsigned int default_color);
void settings_add_file(Settings* settings, const char* name, const char* desc, const char* default_path);

/* Getters */
int settings_get_toggle(Settings* settings, const char* page_name, const char* item_name);
int settings_get_slider(Settings* settings, const char* page_name, const char* item_name);
int settings_get_combobox(Settings* settings, const char* page_name, const char* item_name);
const char* settings_get_textbox(Settings* settings, const char* page_name, const char* item_name);
unsigned int settings_get_color(Settings* settings, const char* page_name, const char* item_name);

/* Setters */
void settings_set_toggle(Settings* settings, const char* page_name, const char* item_name, int value);
void settings_set_slider(Settings* settings, const char* page_name, const char* item_name, int value);
void settings_set_combobox(Settings* settings, const char* page_name, const char* item_name, int value);
void settings_set_textbox(Settings* settings, const char* page_name, const char* item_name, const char* value);
void settings_set_color(Settings* settings, const char* page_name, const char* item_name, unsigned int color);

/* Apply/Save */
void settings_apply(Settings* settings);
void settings_save(Settings* settings);
void settings_load(Settings* settings);
void settings_reset(Settings* settings);

/* Appearance */
void settings_init_appearance(Settings* settings);
void settings_apply_theme(Settings* settings);
void settings_apply_wallpaper(Settings* settings);
void settings_apply_font(Settings* settings);

/* Display */
void settings_init_display(Settings* settings);
void settings_apply_resolution(Settings* settings);
void settings_apply_refresh_rate(Settings* settings);

/* Sound */
void settings_init_sound(Settings* settings);
void settings_apply_volume(Settings* settings);
void settings_apply_output(Settings* settings);

/* Network */
void settings_init_network(Settings* settings);
void settings_apply_wifi(Settings* settings);
void settings_apply_proxy(Settings* settings);

#endif /* SETTINGS_H */
