/*
 * CodixOS Shell Parser
 */

#ifndef PARSER_H
#define PARSER_H

/* Parser functions */
void parser_init();
int parser_parse(char* line, char** args, int* argc);
int parser_expand_variables(char* input, char* output, int max_len);
int parser_handle_quotes(char* input, char* output, int max_len);
int parser_handle_wildcards(char* pattern, char* output, int max_len);

#endif /* PARSER_H */
