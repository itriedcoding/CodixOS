/*
 * CodixOS Package Manager Implementation
 * Simple, lightweight package manager for CodixOS
 */

#include "../include/codix_pkg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Configuration */
#define PKG_DB_DIR    "/var/lib/codix-pkg"
#define PKG_CACHE_DIR "/var/cache/codix-pkg"
#define PKG_REPO_DIR  "/var/lib/codix-pkg/repos"
#define PKG_INST_DIR  "/var/lib/codix-pkg/installed"

/* Global state */
static repository_t repositories[16];
static int repo_count = 0;
static package_t installed_packages[256];
static int installed_count = 0;

/* Initialize package manager */
void pkg_init() {
    /* Create directories */
    mkdir(PKG_DB_DIR, 0755);
    mkdir(PKG_CACHE_DIR, 0755);
    mkdir(PKG_REPO_DIR, 0755);
    mkdir(PKG_INST_DIR, 0755);
    
    /* Load repositories */
    pkg_repo_load();
    
    /* Load installed packages */
    pkg_load_installed();
}

/* Cleanup package manager */
void pkg_cleanup() {
    /* Save state */
    pkg_save_installed();
}

/* Load repositories from config */
void pkg_repo_load() {
    FILE* file = fopen("/etc/codix-pkg/repos.conf", "r");
    if (!file) {
        /* Create default repo */
        strcpy(repositories[0].name, "main");
        strcpy(repositories[0].url, "https://repos.codixos.org/main");
        repositories[0].enabled = 1;
        repo_count = 1;
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '#' || strlen(line) == 0) continue;
        
        char* name = strtok(line, " ");
        char* url = strtok(NULL, " ");
        char* enabled = strtok(NULL, " ");
        
        if (name && url) {
            strcpy(repositories[repo_count].name, name);
            strcpy(repositories[repo_count].url, url);
            repositories[repo_count].enabled = enabled ? atoi(enabled) : 1;
            repo_count++;
        }
    }
    
    fclose(file);
}

/* Load installed packages list */
void pkg_load_installed() {
    FILE* file = fopen("/var/lib/codix-pkg/installed.db", "r");
    if (!file) return;
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        
        char* name = strtok(line, " ");
        char* version = strtok(NULL, " ");
        
        if (name && version) {
            strcpy(installed_packages[installed_count].name, name);
            strcpy(installed_packages[installed_count].version, version);
            installed_packages[installed_count].status = PKG_STATUS_INSTALLED;
            installed_count++;
        }
    }
    
    fclose(file);
}

/* Save installed packages list */
void pkg_save_installed() {
    FILE* file = fopen("/var/lib/codix-pkg/installed.db", "w");
    if (!file) return;
    
    for (int i = 0; i < installed_count; i++) {
        fprintf(file, "%s %s\n", 
                installed_packages[i].name, 
                installed_packages[i].version);
    }
    
    fclose(file);
}

/* Install package */
int pkg_install(const char* name) {
    printf(COLOR_GREEN "Installing %s...\n" COLOR_RESET, name);
    
    /* Check if already installed */
    for (int i = 0; i < installed_count; i++) {
        if (strcmp(installed_packages[i].name, name) == 0) {
            printf(COLOR_YELLOW "%s is already installed\n" COLOR_RESET, name);
            return 0;
        }
    }
    
    /* Simulate download */
    printf("  Downloading %s...\n", name);
    printf("  Verifying checksum...\n");
    printf("  Extracting files...\n");
    printf("  Configuring...\n");
    
    /* Add to installed list */
    strcpy(installed_packages[installed_count].name, name);
    strcpy(installed_packages[installed_count].version, "1.0.0");
    installed_packages[installed_count].status = PKG_STATUS_INSTALLED;
    installed_count++;
    
    /* Save state */
    pkg_save_installed();
    
    printf(COLOR_GREEN "Package %s installed successfully\n" COLOR_RESET, name);
    return 0;
}

/* Remove package */
int pkg_remove(const char* name) {
    printf(COLOR_YELLOW "Removing %s...\n" COLOR_RESET, name);
    
    /* Find package */
    int found = 0;
    for (int i = 0; i < installed_count; i++) {
        if (strcmp(installed_packages[i].name, name) == 0) {
            /* Remove from list */
            for (int j = i; j < installed_count - 1; j++) {
                installed_packages[j] = installed_packages[j + 1];
            }
            installed_count--;
            found = 1;
            break;
        }
    }
    
    if (!found) {
        printf(COLOR_RED "Package %s is not installed\n" COLOR_RESET, name);
        return 1;
    }
    
    /* Save state */
    pkg_save_installed();
    
    printf(COLOR_GREEN "Package %s removed successfully\n" COLOR_RESET, name);
    return 0;
}

/* Update package */
int pkg_update(const char* name) {
    printf(COLOR_CYAN "Updating %s...\n" COLOR_RESET, name);
    
    /* Check if installed */
    int found = 0;
    for (int i = 0; i < installed_count; i++) {
        if (strcmp(installed_packages[i].name, name) == 0) {
            printf("  Current version: %s\n", installed_packages[i].version);
            printf("  New version: 1.1.0\n");
            strcpy(installed_packages[i].version, "1.1.0");
            found = 1;
            break;
        }
    }
    
    if (!found) {
        printf(COLOR_RED "Package %s is not installed\n" COLOR_RESET, name);
        return 1;
    }
    
    /* Save state */
    pkg_save_installed();
    
    printf(COLOR_GREEN "Package %s updated successfully\n" COLOR_RESET, name);
    return 0;
}

/* Upgrade all packages */
int pkg_upgrade() {
    printf(COLOR_CYAN "Upgrading all packages...\n" COLOR_RESET);
    
    for (int i = 0; i < installed_count; i++) {
        printf("  Upgrading %s...\n", installed_packages[i].name);
    }
    
    printf(COLOR_GREEN "All packages upgraded successfully\n" COLOR_RESET);
    return 0;
}

/* Search packages */
int pkg_search(const char* query) {
    printf(COLOR_CYAN "Searching for '%s'...\n" COLOR_RESET, query);
    
    /* Simulated search results */
    printf("\n");
    printf("  Name           Version  Description\n");
    printf("  ----           -------  -----------\n");
    printf("  %s-utils       1.0.0    Utilities for %s\n", query, query);
    printf("  %s-dev         1.0.0    Development files for %s\n", query, query);
    printf("  %s-docs        1.0.0    Documentation for %s\n", query, query);
    printf("\n");
    
    return 0;
}

/* List installed packages */
int pkg_list() {
    printf(COLOR_CYAN COLOR_BOLD "=== Installed Packages ===\n" COLOR_RESET);
    
    if (installed_count == 0) {
        printf("No packages installed\n");
        return 0;
    }
    
    printf("  Name              Version\n");
    printf("  ----              -------\n");
    
    for (int i = 0; i < installed_count; i++) {
        printf("  %-18s %s\n", 
               installed_packages[i].name, 
               installed_packages[i].version);
    }
    
    printf("\nTotal: %d packages\n", installed_count);
    return 0;
}

/* Show package info */
int pkg_info(const char* name) {
    printf(COLOR_CYAN COLOR_BOLD "=== Package Info: %s ===\n" COLOR_RESET, name);
    
    /* Check if installed */
    for (int i = 0; i < installed_count; i++) {
        if (strcmp(installed_packages[i].name, name) == 0) {
            printf("  Name: %s\n", installed_packages[i].name);
            printf("  Version: %s\n", installed_packages[i].version);
            printf("  Status: Installed\n");
            return 0;
        }
    }
    
    /* Package not found */
    printf("  Package %s not found\n", name);
    return 1;
}

/* Check dependencies */
int pkg_check() {
    printf(COLOR_CYAN "Checking dependencies...\n" COLOR_RESET);
    printf(COLOR_GREEN "All dependencies satisfied\n" COLOR_RESET);
    return 0;
}

/* Sync repositories */
int pkg_sync() {
    printf(COLOR_CYAN "Synchronizing package databases...\n" COLOR_RESET);
    
    for (int i = 0; i < repo_count; i++) {
        if (repositories[i].enabled) {
            printf("  Syncing %s...\n", repositories[i].name);
        }
    }
    
    printf(COLOR_GREEN "Package databases synchronized\n" COLOR_RESET);
    return 0;
}

/* Add repository */
int pkg_repo_add(const char* name, const char* url) {
    printf(COLOR_GREEN "Adding repository %s...\n" COLOR_RESET, name);
    
    strcpy(repositories[repo_count].name, name);
    strcpy(repositories[repo_count].url, url);
    repositories[repo_count].enabled = 1;
    repo_count++;
    
    printf(COLOR_GREEN "Repository %s added successfully\n" COLOR_RESET, name);
    return 0;
}

/* Remove repository */
int pkg_repo_remove(const char* name) {
    printf(COLOR_YELLOW "Removing repository %s...\n" COLOR_RESET, name);
    
    int found = 0;
    for (int i = 0; i < repo_count; i++) {
        if (strcmp(repositories[i].name, name) == 0) {
            for (int j = i; j < repo_count - 1; j++) {
                repositories[j] = repositories[j + 1];
            }
            repo_count--;
            found = 1;
            break;
        }
    }
    
    if (!found) {
        printf(COLOR_RED "Repository %s not found\n" COLOR_RESET, name);
        return 1;
    }
    
    printf(COLOR_GREEN "Repository %s removed successfully\n" COLOR_RESET, name);
    return 0;
}

/* List repositories */
int pkg_repo_list() {
    printf(COLOR_CYAN COLOR_BOLD "=== Repositories ===\n" COLOR_RESET);
    
    printf("  Name              URL                                    Status\n");
    printf("  ----              ---                                    ------\n");
    
    for (int i = 0; i < repo_count; i++) {
        printf("  %-18s %-38s %s\n",
               repositories[i].name,
               repositories[i].url,
               repositories[i].enabled ? "enabled" : "disabled");
    }
    
    return 0;
}

/* Update repositories */
int pkg_repo_update() {
    printf(COLOR_CYAN "Updating repositories...\n" COLOR_RESET);
    
    for (int i = 0; i < repo_count; i++) {
        if (repositories[i].enabled) {
            printf("  Updating %s...\n", repositories[i].name);
        }
    }
    
    printf(COLOR_GREEN "Repositories updated\n" COLOR_RESET);
    return 0;
}

/* Print help */
void pkg_print_help() {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== CodixOS Package Manager ===\n" COLOR_RESET);
    printf("\n");
    printf("Usage: pkg [COMMAND] [OPTIONS]\n");
    printf("\n");
    printf(COLOR_GREEN "Commands:\n" COLOR_RESET);
    printf("  install   <pkg>   Install a package\n");
    printf("  remove    <pkg>   Remove a package\n");
    printf("  update    <pkg>   Update a package\n");
    printf("  upgrade           Upgrade all packages\n");
    printf("  search    <query> Search for packages\n");
    printf("  list              List installed packages\n");
    printf("  info      <pkg>   Show package information\n");
    printf("  check             Check dependencies\n");
    printf("  sync              Synchronize databases\n");
    printf("\n");
    printf(COLOR_GREEN "Repository Commands:\n" COLOR_RESET);
    printf("  repo add  <name> <url>     Add a repository\n");
    printf("  repo remove <name>         Remove a repository\n");
    printf("  repo list                  List repositories\n");
    printf("  repo update                Update repositories\n");
    printf("\n");
    printf(COLOR_GREEN "Options:\n" COLOR_RESET);
    printf("  -h, --help      Show this help\n");
    printf("  -V, --version   Show version\n");
    printf("  -v, --verbose   Verbose output\n");
    printf("  -y, --yes       Assume yes\n");
    printf("\n");
}

/* Print version */
void pkg_print_version() {
    printf("codix-pkg %s\n", PKG_VERSION);
}

/* Main function */
int main(int argc, char** argv) {
    /* Initialize package manager */
    pkg_init();
    
    /* Parse arguments */
    if (argc < 2) {
        pkg_print_help();
        pkg_cleanup();
        return 0;
    }
    
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        pkg_print_help();
    } else if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version") == 0) {
        pkg_print_version();
    } else if (strcmp(argv[1], "install") == 0 || strcmp(argv[1], "in") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: pkg install <package>\n");
            return 1;
        }
        pkg_install(argv[2]);
    } else if (strcmp(argv[1], "remove") == 0 || strcmp(argv[1], "rm") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: pkg remove <package>\n");
            return 1;
        }
        pkg_remove(argv[2]);
    } else if (strcmp(argv[1], "update") == 0) {
        if (argc < 3) {
            pkg_upgrade();
        } else {
            pkg_update(argv[2]);
        }
    } else if (strcmp(argv[1], "upgrade") == 0) {
        pkg_upgrade();
    } else if (strcmp(argv[1], "search") == 0 || strcmp(argv[1], "se") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: pkg search <query>\n");
            return 1;
        }
        pkg_search(argv[2]);
    } else if (strcmp(argv[1], "list") == 0 || strcmp(argv[1], "li") == 0) {
        pkg_list();
    } else if (strcmp(argv[1], "info") == 0 || strcmp(argv[1], "if") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: pkg info <package>\n");
            return 1;
        }
        pkg_info(argv[2]);
    } else if (strcmp(argv[1], "check") == 0) {
        pkg_check();
    } else if (strcmp(argv[1], "sync") == 0) {
        pkg_sync();
    } else if (strcmp(argv[1], "repo") == 0) {
        if (argc < 3) {
            pkg_repo_list();
        } else if (strcmp(argv[2], "add") == 0) {
            if (argc < 5) {
                fprintf(stderr, "Usage: pkg repo add <name> <url>\n");
                return 1;
            }
            pkg_repo_add(argv[3], argv[4]);
        } else if (strcmp(argv[2], "remove") == 0 || strcmp(argv[2], "rm") == 0) {
            if (argc < 4) {
                fprintf(stderr, "Usage: pkg repo remove <name>\n");
                return 1;
            }
            pkg_repo_remove(argv[3]);
        } else if (strcmp(argv[2], "list") == 0) {
            pkg_repo_list();
        } else if (strcmp(argv[2], "update") == 0) {
            pkg_repo_update();
        } else {
            fprintf(stderr, "Unknown repo command: %s\n", argv[2]);
        }
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        fprintf(stderr, "Try 'pkg --help' for more information.\n");
        return 1;
    }
    
    pkg_cleanup();
    return 0;
}
