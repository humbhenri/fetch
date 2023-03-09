#include "term_programs.h"
#include "helpers.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*font_finder_type)(char *, size_t);

void find_font_gnome_terminal(char *response, size_t len)
{
    strncpy(response, "TODO", len);
}

void find_font_emacs(char *response, size_t len)
{
    read_line_from_cmd("emacsclient --eval \"(font-get (face-attribute 'default :font) :family)\"", response, len);
}

void find_font_alacritty(char *response, size_t len)
{
    char *line;
    size_t line_len = 0;
    char config_file[300];
    sprintf(config_file, "%s/.config/alacritty/alacritty.yml", getenv("HOME"));
    FILE *f = fopen(config_file, "r");
    if (!f) {
      puts("File not found");
      exit(1);
    }
    while (getline(&line, &line_len, f) != -1) {
      if (strstr(line, "#") != NULL) {
        continue;
      }
      if (strstr(line, "family") != NULL) {
        char *font_name = strchr(line, ':');
        if (font_name != NULL) {
          strncpy(response, font_name+1, len);
        }
        break;
      }
    }
    fclose(f);
    return;
}

void find_font_vscode(char *response, size_t len)
{
    strncpy(response, "TODO", len);
}

void find_font_kitty(char *response, size_t len)
{
    char config_file[100];
    sprintf(config_file, "%s/.config/kitty/kitty.conf", getenv("HOME"));
    char line[100];
    bool found = search_line_containing("font_family", config_file, line, sizeof line);
    if (found) {
        char *font_name = strchr(line, ' ');
        if (font_name != NULL) {
            strncpy(response, font_name+1, len);
        }
    }
}

struct term_program {
    char *name;
    font_finder_type font_finder;
};

struct term_program term_programs[] = {
    {.name = "gnome-terminal"	, .font_finder = find_font_gnome_terminal},
    {.name = "emacs"		, .font_finder = find_font_emacs},
    {.name = "kitty"		, .font_finder = find_font_kitty},
    {.name = "alacritty"	, .font_finder = find_font_alacritty},
    {.name = "node"		, .font_finder = find_font_vscode},
};

void find_font(const char *term, char *response, size_t len)
{
    size_t term_programs_len = sizeof(term_programs) / sizeof(struct term_program);
    for (int i=0; i < term_programs_len; i++) {
        if (strcmp(term, term_programs[i].name) == 0) {
            DEBUG_PRINT("term = %s\n", term_programs[i].name);
            (*term_programs[i].font_finder)(response, len);
            break;
        }
    }
}

const char* term_program_exists(const char* name)
{
    size_t term_programs_len = sizeof(term_programs) / sizeof(struct term_program);
    for (int i=0; i < term_programs_len; i++) {
	if (strstr(name, term_programs[i].name) != NULL) {
	    DEBUG_PRINT("searching for %s found %s\n", name, term_programs[i].name);
	    return term_programs[i].name;
	}
    }
    return NULL;
}
