#ifndef TERM_PROGRAMS_H_
#define TERM_PROGRAMS_H_
#include <stddef.h>

void find_font(const char *term, char *response, size_t len);
const char* term_program_exists(const char* name);

#endif // TERM_PROGRAMS_H_
