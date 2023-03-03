#ifndef HELPERS_H_
#define HELPERS_H_
#include <stdbool.h>
#include <stddef.h>

char *ltrim(char *s);

char *rtrim(char *s);

char *trim(char *s);

void read_line_from_cmd(const char *cmd, char *output, size_t len);

bool has(const char* program);

char* os_name();

bool search_line_containing(const char *substr, const char *filename, char *response, size_t response_len);

#endif // HELPERS_H_
