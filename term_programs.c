#include "term_programs.h"
#include "helpers.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char *TERM_PROGRAMS[] = {
    "gnome-terminal",
    "emacs",
    "kitty",
    "alacritty",
    "node", // vscode
    NULL
};

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
    strncpy(response, "TODO", len);
}

void (*font_finders[])(char *, size_t) = {
    find_font_gnome_terminal,
    find_font_emacs,
    find_font_kitty,
    find_font_alacritty,
    find_font_vscode,
    NULL
};

void find_font(const char *term, char *response, size_t len)
{
    for (int i=0; TERM_PROGRAMS[i] != NULL; i++) {
        if (strcmp(term, TERM_PROGRAMS[i]) == 0) {
            DEBUG_PRINT("term = %s\n", TERM_PROGRAMS[i]);
            (*font_finders[i])(response, len);
            break;
        }
    }
}