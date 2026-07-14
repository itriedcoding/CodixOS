/*
 * CodixOS System Utilities Header
 */

#ifndef CODIX_UTILS_H
#define CODIX_UTILS_H

/* Utility function prototypes */
int codix_ls(int argc, char** argv);
int codix_cat(int argc, char** argv);
int codix_cp(int argc, char** argv);
int codix_mv(int argc, char** argv);
int codix_rm(int argc, char** argv);
int codix_mkdir(int argc, char** argv);
int codix_echo(int argc, char** argv);
int codix_grep(int argc, char** argv);
int codix_chmod(int argc, char** argv);
int codix_ps(int argc, char** argv);
int codix_kill(int argc, char** argv);
int codix_df(int argc, char** argv);
int codix_free(int argc, char** argv);

/* Helper functions */
void print_usage(const char* program);
void print_error(const char* message);
int file_exists(const char* path);
int directory_exists(const char* path);

#endif /* CODIX_UTILS_H */
