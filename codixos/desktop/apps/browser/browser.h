/*
 * CodixOS Browser - Firefox Integration
 * Pre-installed web browser for the desktop environment
 */

#ifndef BROWSER_H
#define BROWSER_H

#include <stdint.h>

/* Browser states */
#define BROWSER_STATE_CLOSED      0
#define BROWSER_STATE_OPEN        1
#define BROWSER_STATE_LOADING     2
#define BROWSER_STATE_ERROR       3

/* Tab states */
#define TAB_STATE_ACTIVE          0
#define TAB_STATE_LOADING         1
#define TAB_STATE_COMPLETE        2

#define MAX_TABS                  32
#define MAX_URL_LEN               2048
#define MAX_TITLE_LEN             256

/* Tab structure */
typedef struct {
    char     url[MAX_URL_LEN];
    char     title[MAX_TITLE_LEN];
    uint8_t  state;
    uint8_t  active;
} browser_tab_t;

/* Browser context */
typedef struct {
    uint8_t      state;
    uint8_t      current_tab;
    uint8_t      tab_count;
    browser_tab_t tabs[MAX_TABS];
    char         home_page[MAX_URL_LEN];
    uint8_t      java_script;
    uint8_t      popups_blocked;
    uint8_t      do_not_track;
    uint8_t      private_mode;
} browser_ctx_t;

/*
 * Initialize browser
 */
void browser_init(void);

/*
 * Open browser window
 */
int browser_open(void);

/*
 * Close browser window
 */
int browser_close(void);

/*
 * Navigate to URL
 */
int browser_navigate(const char* url);

/*
 * Go back in history
 */
int browser_back(void);

/*
 * Go forward in history
 */
int browser_forward(void);

/*
 * Reload current page
 */
int browser_reload(void);

/*
 * Stop loading
 */
int browser_stop(void);

/*
 * Add new tab
 */
int browser_new_tab(void);

/*
 * Close tab
 */
int browser_close_tab(int tab_index);

/*
 * Switch to tab
 */
int browser_switch_tab(int tab_index);

/*
 * Set home page
 */
int browser_set_homepage(const char* url);

/*
 * Toggle JavaScript
 */
int browser_toggle_javascript(void);

/*
 * Toggle pop-up blocking
 */
int browser_toggle_popups(void);

/*
 * Toggle Do Not Track
 */
int browser_toggle_dnt(void);

/*
 * Toggle private browsing mode
 */
int browser_toggle_private(void);

/*
 * Show browser status
 */
void browser_status(void);

/*
 * Show browser settings
 */
void browser_settings(void);

/*
 * Show browser history
 */
void browser_history(void);

/*
 * Search the web
 */
int browser_search(const char* query);

/*
 * Download file
 */
int browser_download(const char* url, const char* save_path);

/*
 * Bookmark page
 */
int browser_bookmark(const char* url, const char* name);

/*
 * Show bookmarks
 */
void browser_bookmarks(void);

/*
 * Clear browsing data
 */
int browser_clear_data(uint8_t clear_history, uint8_t clear_cookies, uint8_t clear_cache);

/*
 * Get browser version
 */
const char* browser_version(void);

#endif /* BROWSER_H */
