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

#define DEBUG 3

#if defined(DEBUG) && DEBUG > 0
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
 #define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#endif // HELPERS_H_
