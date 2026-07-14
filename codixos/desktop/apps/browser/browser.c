/*
 * CodixOS Browser - Firefox Integration
 * Pre-installed web browser for the desktop environment
 */

#include "browser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global browser context */
static browser_ctx_t browser;

/* Bookmarks storage */
#define MAX_BOOKMARKS 64
typedef struct {
    char name[128];
    char url[MAX_URL_LEN];
} bookmark_t;

static bookmark_t bookmarks[MAX_BOOKMARKS];
static int bookmark_count = 0;

/*
 * Initialize browser
 */
void browser_init(void) {
    memset(&browser, 0, sizeof(browser_ctx_t));
    browser.state = BROWSER_STATE_CLOSED;
    browser.java_script = 1;
    browser.popups_blocked = 1;
    browser.do_not_track = 1;
    browser.private_mode = 0;
    strncpy(browser.home_page, "https://www.google.com", MAX_URL_LEN - 1);
    
    printf(COLOR_CYAN "Firefox browser initialized\n" COLOR_RESET);
}

/*
 * Open browser window
 */
int browser_open(void) {
    if (browser.state == BROWSER_STATE_OPEN) {
        printf(COLOR_YELLOW "Browser already open\n" COLOR_RESET);
        return 0;
    }
    
    browser.state = BROWSER_STATE_OPEN;
    browser.tab_count = 1;
    browser.current_tab = 0;
    
    /* Initialize first tab with home page */
    browser_tab_t* tab = &browser.tabs[0];
    strncpy(tab->url, browser.home_page, MAX_URL_LEN - 1);
    strncpy(tab->title, "New Tab", MAX_TITLE_LEN - 1);
    tab->state = TAB_STATE_COMPLETE;
    tab->active = 1;
    
    printf(COLOR_GREEN "Firefox browser opened\n" COLOR_RESET);
    printf("  Homepage: %s\n", browser.home_page);
    
    return 0;
}

/*
 * Close browser window
 */
int browser_close(void) {
    if (browser.state != BROWSER_STATE_OPEN) {
        printf(COLOR_YELLOW "Browser not open\n" COLOR_RESET);
        return 0;
    }
    
    browser.state = BROWSER_STATE_CLOSED;
    browser.tab_count = 0;
    browser.current_tab = 0;
    memset(browser.tabs, 0, sizeof(browser.tabs));
    
    printf(COLOR_GREEN "Browser closed\n" COLOR_RESET);
    return 0;
}

/*
 * Navigate to URL
 */
int browser_navigate(const char* url) {
    if (browser.state != BROWSER_STATE_OPEN) {
        printf(COLOR_RED "Browser not open\n" COLOR_RESET);
        return -1;
    }
    
    if (!url || strlen(url) == 0) {
        printf(COLOR_RED "Invalid URL\n" COLOR_RESET);
        return -1;
    }
    
    browser_tab_t* tab = &browser.tabs[browser.current_tab];
    
    /* Simulate loading */
    tab->state = TAB_STATE_LOADING;
    browser.state = BROWSER_STATE_LOADING;
    
    strncpy(tab->url, url, MAX_URL_LEN - 1);
    
    /* Extract title from URL */
    const char* domain = strstr(url, "://");
    if (domain) {
        domain += 3;
        const char* end = strchr(domain, '/');
        if (end) {
            int len = end - domain;
            if (len > MAX_TITLE_LEN - 1) len = MAX_TITLE_LEN - 1;
            strncpy(tab->title, domain, len);
            tab->title[len] = '\0';
        } else {
            strncpy(tab->title, domain, MAX_TITLE_LEN - 1);
        }
    }
    
    /* Simulate completion */
    tab->state = TAB_STATE_COMPLETE;
    browser.state = BROWSER_STATE_OPEN;
    
    printf(COLOR_CYAN "Navigating to: %s\n" COLOR_RESET, url);
    printf("  Title: %s\n", tab->title);
    
    return 0;
}

/*
 * Go back in history
 */
int browser_back(void) {
    printf(COLOR_CYAN "Going back...\n" COLOR_RESET);
    return 0;
}

/*
 * Go forward in history
 */
int browser_forward(void) {
    printf(COLOR_CYAN "Going forward...\n" COLOR_RESET);
    return 0;
}

/*
 * Reload current page
 */
int browser_reload(void) {
    if (browser.state != BROWSER_STATE_OPEN) {
        printf(COLOR_RED "Browser not open\n" COLOR_RESET);
        return -1;
    }
    
    browser_tab_t* tab = &browser.tabs[browser.current_tab];
    tab->state = TAB_STATE_LOADING;
    browser.state = BROWSER_STATE_LOADING;
    
    /* Simulate reload */
    tab->state = TAB_STATE_COMPLETE;
    browser.state = BROWSER_STATE_OPEN;
    
    printf(COLOR_CYAN "Page reloaded\n" COLOR_RESET);
    return 0;
}

/*
 * Stop loading
 */
int browser_stop(void) {
    if (browser.state == BROWSER_STATE_LOADING) {
        browser.state = BROWSER_STATE_OPEN;
        browser.tabs[browser.current_tab].state = TAB_STATE_COMPLETE;
        printf(COLOR_CYAN "Loading stopped\n" COLOR_RESET);
    }
    return 0;
}

/*
 * Add new tab
 */
int browser_new_tab(void) {
    if (browser.tab_count >= MAX_TABS) {
        printf(COLOR_RED "Maximum tabs reached\n" COLOR_RESET);
        return -1;
    }
    
    int new_tab = browser.tab_count++;
    browser_tab_t* tab = &browser.tabs[new_tab];
    
    strncpy(tab->url, browser.home_page, MAX_URL_LEN - 1);
    strncpy(tab->title, "New Tab", MAX_TITLE_LEN - 1);
    tab->state = TAB_STATE_COMPLETE;
    tab->active = 1;
    
    /* Deactivate other tabs */
    for (int i = 0; i < browser.tab_count; i++) {
        if (i != new_tab) {
            browser.tabs[i].active = 0;
        }
    }
    
    browser.current_tab = new_tab;
    
    printf(COLOR_GREEN "New tab opened (tab %d)\n" COLOR_RESET, new_tab);
    return 0;
}

/*
 * Close tab
 */
int browser_close_tab(int tab_index) {
    if (tab_index < 0 || tab_index >= browser.tab_count) {
        printf(COLOR_RED "Invalid tab index\n" COLOR_RESET);
        return -1;
    }
    
    if (browser.tab_count == 1) {
        printf(COLOR_YELLOW "Closing last tab will close browser\n" COLOR_RESET);
        browser_close();
        return 0;
    }
    
    /* Shift tabs */
    for (int i = tab_index; i < browser.tab_count - 1; i++) {
        browser.tabs[i] = browser.tabs[i + 1];
    }
    
    browser.tab_count--;
    
    if (browser.current_tab >= browser.tab_count) {
        browser.current_tab = browser.tab_count - 1;
    }
    
    printf(COLOR_GREEN "Tab closed\n" COLOR_RESET);
    return 0;
}

/*
 * Switch to tab
 */
int browser_switch_tab(int tab_index) {
    if (tab_index < 0 || tab_index >= browser.tab_count) {
        printf(COLOR_RED "Invalid tab index\n" COLOR_RESET);
        return -1;
    }
    
    /* Deactivate all tabs */
    for (int i = 0; i < browser.tab_count; i++) {
        browser.tabs[i].active = 0;
    }
    
    browser.tabs[tab_index].active = 1;
    browser.current_tab = tab_index;
    
    printf(COLOR_CYAN "Switched to tab %d: %s\n" COLOR_RESET, tab_index, browser.tabs[tab_index].title);
    return 0;
}

/*
 * Set home page
 */
int browser_set_homepage(const char* url) {
    if (!url) {
        printf(COLOR_RED "Invalid URL\n" COLOR_RESET);
        return -1;
    }
    
    strncpy(browser.home_page, url, MAX_URL_LEN - 1);
    printf(COLOR_GREEN "Homepage set to: %s\n" COLOR_RESET, url);
    return 0;
}

/*
 * Toggle JavaScript
 */
int browser_toggle_javascript(void) {
    browser.java_script = !browser.java_script;
    printf(COLOR_CYAN "JavaScript: %s\n" COLOR_RESET, 
           browser.java_script ? COLOR_GREEN "Enabled" COLOR_RESET : COLOR_YELLOW "Disabled" COLOR_RESET);
    return 0;
}

/*
 * Toggle pop-up blocking
 */
int browser_toggle_popups(void) {
    browser.popups_blocked = !browser.popups_blocked;
    printf(COLOR_CYAN "Pop-up blocking: %s\n" COLOR_RESET,
           browser.popups_blocked ? COLOR_GREEN "Enabled" COLOR_RESET : COLOR_YELLOW "Disabled" COLOR_RESET);
    return 0;
}

/*
 * Toggle Do Not Track
 */
int browser_toggle_dnt(void) {
    browser.do_not_track = !browser.do_not_track;
    printf(COLOR_CYAN "Do Not Track: %s\n" COLOR_RESET,
           browser.do_not_track ? COLOR_GREEN "Enabled" COLOR_RESET : COLOR_YELLOW "Disabled" COLOR_RESET);
    return 0;
}

/*
 * Toggle private browsing mode
 */
int browser_toggle_private(void) {
    browser.private_mode = !browser.private_mode;
    printf(COLOR_CYAN "Private Browsing: %s\n" COLOR_RESET,
           browser.private_mode ? COLOR_GREEN "Enabled" COLOR_YELLOW "Disabled" COLOR_RESET);
    return 0;
}

/*
 * Show browser status
 */
void browser_status(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Firefox Browser Status ===\n" COLOR_RESET);
    printf("\n");
    
    const char* state_str;
    switch (browser.state) {
        case BROWSER_STATE_CLOSED:  state_str = COLOR_RED "Closed" COLOR_RESET; break;
        case BROWSER_STATE_OPEN:    state_str = COLOR_GREEN "Open" COLOR_RESET; break;
        case BROWSER_STATE_LOADING: state_str = COLOR_YELLOW "Loading" COLOR_RESET; break;
        case BROWSER_STATE_ERROR:   state_str = COLOR_RED "Error" COLOR_RESET; break;
        default:                    state_str = "Unknown"; break;
    }
    
    printf("  State:            %s\n", state_str);
    printf("  Tabs:             %d/%d\n", browser.tab_count, MAX_TABS);
    printf("  Current Tab:      %d\n", browser.current_tab);
    printf("  Homepage:         %s\n", browser.home_page);
    printf("\n");
    
    if (browser.state == BROWSER_STATE_OPEN) {
        printf(COLOR_CYAN "  Open Tabs:\n" COLOR_RESET);
        for (int i = 0; i < browser.tab_count; i++) {
            browser_tab_t* tab = &browser.tabs[i];
            printf("    [%d] %s%s%s - %s\n",
                   i,
                   i == browser.current_tab ? COLOR_BOLD : "",
                   tab->title,
                   i == browser.current_tab ? COLOR_RESET : "",
                   tab->url);
        }
    }
    
    printf("\n");
    printf("  Settings:\n");
    printf("    JavaScript:    %s\n", browser.java_script ? "On" : "Off");
    printf("    Pop-ups:       %s\n", browser.popups_blocked ? "Blocked" : "Allowed");
    printf("    Do Not Track:  %s\n", browser.do_not_track ? "On" : "Off");
    printf("    Private Mode:  %s\n", browser.private_mode ? "On" : "Off");
    printf("\n");
}

/*
 * Show browser settings
 */
void browser_settings(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Browser Settings ===\n" COLOR_RESET);
    printf("\n");
    printf("  1. Homepage:      %s\n", browser.home_page);
    printf("  2. JavaScript:    %s\n", browser.java_script ? "Enabled" : "Disabled");
    printf("  3. Pop-ups:       %s\n", browser.popups_blocked ? "Blocked" : "Allowed");
    printf("  4. Do Not Track:  %s\n", browser.do_not_track ? "Enabled" : "Disabled");
    printf("  5. Private Mode:  %s\n", browser.private_mode ? "Enabled" : "Disabled");
    printf("\n");
}

/*
 * Show browser history
 */
void browser_history(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Browser History ===\n" COLOR_RESET);
    printf("\n");
    printf("  (History would be displayed here)\n");
    printf("\n");
}

/*
 * Search the web
 */
int browser_search(const char* query) {
    if (!query) {
        printf(COLOR_RED "Invalid search query\n" COLOR_RESET);
        return -1;
    }
    
    char url[MAX_URL_LEN];
    snprintf(url, MAX_URL_LEN, "https://www.google.com/search?q=%s", query);
    
    printf(COLOR_CYAN "Searching for: %s\n" COLOR_RESET, query);
    return browser_navigate(url);
}

/*
 * Download file
 */
int browser_download(const char* url, const char* save_path) {
    if (!url) {
        printf(COLOR_RED "Invalid URL\n" COLOR_RESET);
        return -1;
    }
    
    printf(COLOR_CYAN "Downloading: %s\n" COLOR_RESET, url);
    printf("  Save to: %s\n", save_path ? save_path : "~/Downloads/");
    printf(COLOR_GREEN "Download started\n" COLOR_RESET);
    
    return 0;
}

/*
 * Bookmark page
 */
int browser_bookmark(const char* url, const char* name) {
    if (bookmark_count >= MAX_BOOKMARKS) {
        printf(COLOR_RED "Maximum bookmarks reached\n" COLOR_RESET);
        return -1;
    }
    
    bookmark_t* bm = &bookmarks[bookmark_count++];
    strncpy(bm->url, url, MAX_URL_LEN - 1);
    strncpy(bm->name, name ? name : url, 127);
    
    printf(COLOR_GREEN "Bookmarked: %s\n" COLOR_RESET, bm->name);
    return 0;
}

/*
 * Show bookmarks
 */
void browser_bookmarks(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Bookmarks ===\n" COLOR_RESET);
    printf("\n");
    
    if (bookmark_count == 0) {
        printf("  No bookmarks\n");
    } else {
        for (int i = 0; i < bookmark_count; i++) {
            printf("  [%d] %s - %s\n", i, bookmarks[i].name, bookmarks[i].url);
        }
    }
    
    printf("\n");
}

/*
 * Clear browsing data
 */
int browser_clear_data(uint8_t clear_history, uint8_t clear_cookies, uint8_t clear_cache) {
    printf(COLOR_CYAN "Clearing browsing data...\n" COLOR_RESET);
    
    if (clear_history) {
        printf("  Clearing history...\n");
    }
    if (clear_cookies) {
        printf("  Clearing cookies...\n");
    }
    if (clear_cache) {
        printf("  Clearing cache...\n");
    }
    
    printf(COLOR_GREEN "Browsing data cleared\n" COLOR_RESET);
    return 0;
}

/*
 * Get browser version
 */
const char* browser_version(void) {
    return "CodixOS Firefox 120.0 (codixos)";
}
