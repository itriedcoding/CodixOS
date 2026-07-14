/*
 * CodixOS File System Implementation
 */

#include "filesystem.h"
#include "memory.h"
#include "kernel.h"

static fs_node_t* root_node = NULL;
static fs_node_t* current_node = NULL;

/* Simple RAM disk storage */
#define RAMDISK_SIZE (1024 * 1024) /* 1MB */
static uint8_t ramdisk[RAMDISK_SIZE];
static uint32_t ramdisk_used = 0;

void init_filesystem() {
    /* Create root directory */
    root_node = create_directory("/", 0755);
    current_node = root_node;
    
    /* Create standard directories */
    create_directory("/bin", 0755);
    create_directory("/etc", 0755);
    create_directory("/home", 0755);
    create_directory("/tmp", 0755);
    create_directory("/usr", 0755);
    create_directory("/var", 0755);
}

fs_node_t* create_file(const char* name, uint32_t permissions) {
    fs_node_t* node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    if (!node) return NULL;
    
    strcpy(node->name, name);
    node->inode = ramdisk_used;
    node->length = 0;
    node->permissions = permissions;
    node->flags = FILE_TYPE_FILE;
    node->uid = 0;
    node->gid = 0;
    node->impl = 0;
    node->ptr = NULL;
    
    return node;
}

fs_node_t* create_directory(const char* name, uint32_t permissions) {
    fs_node_t* node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    if (!node) return NULL;
    
    strcpy(node->name, name);
    node->inode = ramdisk_used;
    node->length = 0;
    node->permissions = permissions;
    node->flags = FILE_TYPE_DIRECTORY;
    node->uid = 0;
    node->gid = 0;
    node->impl = 0;
    node->ptr = NULL;
    
    return node;
}

void delete_node(fs_node_t* node) {
    if (node) {
        kfree(node);
    }
}

fs_node_t* find_node(const char* path) {
    /* Simple path resolution */
    if (strcmp(path, "/") == 0) {
        return root_node;
    }
    
    /* For now, return root for any path */
    return root_node;
}

uint32_t read_file(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || node->flags != FILE_TYPE_FILE) {
        return 0;
    }
    
    uint32_t to_read = size;
    if (offset + size > node->length) {
        to_read = node->length - offset;
    }
    
    memcpy(buffer, &ramdisk[node->inode + offset], to_read);
    return to_read;
}

uint32_t write_file(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || node->flags != FILE_TYPE_FILE) {
        return 0;
    }
    
    if (node->inode + offset + size > RAMDISK_SIZE) {
        return 0;
    }
    
    memcpy(&ramdisk[node->inode + offset], buffer, size);
    node->length = offset + size;
    return size;
}

dirent_t* read_directory(fs_node_t* node, uint32_t index) {
    /* Placeholder implementation */
    return NULL;
}
