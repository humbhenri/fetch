#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s));
}

void read_line_from_cmd(const char *cmd, char *output, size_t len)
{
  FILE *fp = popen(cmd, "r");
  if (fp == NULL) {
    printf("Error opening command");
    exit(1);
  }
  if (getline(&output, &len, fp) == -1) {
    printf("Error opening command");
    exit(1);
  }
  pclose(fp);
}

bool has(const char* program)
{
  char find[200];
  sprintf(find, "type -p %s >/dev/null", program);
  int res = system(find);
  return res == 0;
}

char* os_name()
{
    #ifdef _WIN32
    return "Windows 32-bit";
    #elif _WIN64
    return "Windows 64-bit";
    #elif __APPLE__ || __MACH__
    return "Mac OSX";
    #elif __linux__
    return "Linux";
    #elif __FreeBSD__
    return "FreeBSD";
    #elif __unix || __unix__
    return "Unix";
    #else
    return "Other";
    #endif
}

/*
 * get the line containing the substring, returns true in case of line found */
bool search_line_containing(const char *substr, const char *filename, char *response, size_t response_len)
{
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error opening file");
    return false;
  }
  size_t line_len = 200;
  char *line;
  bool found = false;
  while (getline(&line, &line_len, fp) != -1) {
    if (strstr(line, substr) != NULL) {
      strncpy(response, line, response_len);
      found = true;
      break;
    }
  }
  fclose(fp);
  return found;
}
