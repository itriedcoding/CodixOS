/*
 * CodixOS Package Manager Header
 */

#ifndef CODIX_PKG_H
#define CODIX_PKG_H

/* Package manager version */
#define PKG_VERSION "1.0.0"

/* Package status */
#define PKG_STATUS_INSTALLED   1
#define PKG_STATUS_AVAILABLE   2
#define PKG_STATUS_UPDATABLE   3
#define PKG_STATUS_CORRUPTED   4

/* Package structure */
typedef struct {
    char name[64];
    char version[32];
    char description[256];
    char url[256];
    char license[64];
    char* dependencies[32];
    int dep_count;
    unsigned long size;
    int status;
} package_t;

/* Repository structure */
typedef struct {
    char name[64];
    char url[256];
    char* packages[1024];
    int package_count;
    int enabled;
} repository_t;

/* Package manager functions */
void pkg_init();
void pkg_cleanup();

/* Package operations */
int pkg_install(const char* name);
int pkg_remove(const char* name);
int pkg_update(const char* name);
int pkg_upgrade();
int pkg_search(const char* query);
int pkg_list();
int pkg_info(const char* name);
int pkg_check();
int pkg_sync();

/* Repository operations */
int pkg_repo_add(const char* name, const char* url);
int pkg_repo_remove(const char* name);
int pkg_repo_list();
int pkg_repo_update();

/* Utility functions */
void pkg_print_help();
void pkg_print_version();
int pkg_download_file(const char* url, const char* dest);
int pkg_extract_package(const char* package, const char* dest);
int pkg_verify_checksum(const char* file, const char* checksum);

#endif /* CODIX_PKG_H */
