/*
 * CodixOS Shell Commands Header
 */

#ifndef COMMANDS_H
#define COMMANDS_H

/* Command function prototypes */
int cmd_help(char** args, int argc);
int cmd_exit(char** args, int argc);
int cmd_cd(char** args, int argc);
int cmd_pwd(char** args, int argc);
int cmd_echo(char** args, int argc);
int cmd_clear(char** args, int argc);
int cmd_history(char** args, int argc);
int cmd_export(char** args, int argc);
int cmd_env(char** args, int argc);
int cmd_alias(char** args, int argc);
int cmd_source(char** args, int argc);
int cmd_uname(char** args, int argc);
int cmd_uptime(char** args, int argc);
int cmd_whoami(char** args, int argc);
int cmd_hostname(char** args, int argc);
int cmd_date(char** args, int argc);
int cmd_cal(char** args, int argc);
int cmd_ls(char** args, int argc);
int cmd_cat(char** args, int argc);
int cmd_cp(char** args, int argc);
int cmd_mv(char** args, int argc);
int cmd_rm(char** args, int argc);
int cmd_mkdir(char** args, int argc);
int cmd_touch(char** args, int argc);
int cmd_find(char** args, int argc);
int cmd_grep(char** args, int argc);
int cmd_ps(char** args, int argc);
int cmd_kill(char** args, int argc);
int cmd_df(char** args, int argc);
int cmd_free(char** args, int argc);
int cmd_top(char** args, int argc);
int cmd_pkg(char** args, int argc);
int cmd_man(char** args, int argc);
int cmd_about(char** args, int argc);
int cmd_neofetch(char** args, int argc);
int cmd_cmatrix(char** args, int argc);

/* External command execution */
int execute_external(char** args, int argc);

#endif /* COMMANDS_H */
