/*
 * CodixOS File System
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "kernel.h"

/* File types */
#define FILE_TYPE_FILE      0
#define FILE_TYPE_DIRECTORY 1
#define FILE_TYPE_LINK      2

/* File modes */
#define FILE_MODE_READ   0x01
#define FILE_MODE_WRITE  0x02
#define FILE_MODE_EXEC   0x04

/* File node structure */
typedef struct fs_node {
    char name[256];
    uint32_t inode;
    uint32_t length;
    uint32_t permissions;
    uint32_t flags;
    uint32_t uid;
    uint32_t gid;
    uint32_t impl;
    struct fs_node* ptr;
} fs_node_t;

/* Directory entry */
typedef struct dirent {
    char name[256];
    uint32_t inode;
} dirent_t;

/* Functions */
void init_filesystem();
fs_node_t* create_file(const char* name, uint32_t permissions);
fs_node_t* create_directory(const char* name, uint32_t permissions);
void delete_node(fs_node_t* node);
fs_node_t* find_node(const char* path);
uint32_t read_file(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
uint32_t write_file(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
dirent_t* read_directory(fs_node_t* node, uint32_t index);

#endif /* FILESYSTEM_H */
