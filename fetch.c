/* neofetch like program */

#include "helpers.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

#define CPU_NAME_LEN 200

struct info {
  char *key, *value;
};

void print_info(struct info *info);

struct cpu {
  char name[CPU_NAME_LEN];
  double mhz;
  int cores;
};

void cpu_info(struct cpu *cpu);

void get_gpu(char *output, size_t output_len);

int get_packages_number();

void get_font_name(char *term_program, char *response, size_t len);

void get_terminal_program(char *term, size_t term_len);

int main() {
  char *user = getenv("USER");
  char host[100];
  gethostname(host, 100);
  char userhost[200];
  sprintf(userhost, "%s@%s", user, host);

  struct utsname uname_data;
  int error = uname(&uname_data);
  if (error != 0) {
    printf("Error uname\n");
    return EXIT_FAILURE;
  }
  char kernel[100];
  sprintf(kernel, "%s", uname_data.release);

  struct sysinfo s_info;
  error = sysinfo(&s_info);
  if (error != 0) {
    printf("Error uname\n");
    return EXIT_FAILURE;
  }
  char uptime[100];
  long hours = 0;
  long minutes = 0;
  long days = 0;
  long seconds = s_info.uptime;
  if (seconds > 60) {
    minutes = seconds / 60;
    seconds = seconds % 60;
  }
  if (minutes > 60) {
    hours = minutes / 60;
    minutes = minutes % 60;
  }
  if (hours > 24) {
    days = hours / 24;
    hours = hours % 24;
  }
  sprintf(uptime, "%ld days %ld hours %ld minutes %ld seconds", days, hours,
          minutes, seconds);

  char ram[200];
  sprintf(ram, "%lu MiB/%lu MiB",
          (s_info.totalram - s_info.freeram) / 1024 / 1024,
          s_info.totalram / 1024 / 1024);

  struct cpu c_info;
  cpu_info(&c_info);
  char cpu_text[500];
  sprintf(cpu_text, "%s (%d) @ %lf MHz", trim(c_info.name), c_info.cores,
          c_info.mhz);

  char packages[100];
  int n_packages = get_packages_number();
  sprintf(packages, "%d", n_packages);

  char resolution[100];
  read_line_from_cmd("xdpyinfo | awk '/dimensions:/ { print $2; exit }'",
                     resolution, sizeof resolution);

  char gpu[200];
  get_gpu(gpu, sizeof gpu);

  char term_program[100];
  get_terminal_program(term_program, sizeof term_program);

  char term_font[100];
  get_font_name(term_program, term_font, sizeof term_font);

  struct info infos[] = {{.key = "", .value = userhost},
                         {.key = "OS", .value = os_name()},
                         {.key = "Kernel", .value = kernel},
                         {.key = "Uptime", .value = uptime},
                         {.key = "Packages", .value = packages},
                         {.key = "Shell", .value = getenv("SHELL")},
                         {.key = "Resolution", .value = trim(resolution)},
                         {.key = "WM", .value = getenv("XDG_SESSION_DESKTOP")}
                         /* ,{.key = "WM Theme", .value = "Adwaita"} */
                         /* ,{.key = "Theme", .value = "Adwaita"} */
                         /* ,{.key = "Icons", .value = "Adwaita"} */
                         ,
                         {.key = "Terminal", .value = trim(term_program)},
                         {.key = "Terminal font", .value = trim(term_font)},
                         {.key = "CPU", .value = cpu_text},
                         {.key = "GPU", .value = gpu},
                         {.key = "Memory", .value = ram}};

  size_t info_len = sizeof infos / sizeof(struct info);
  for (size_t i = 0; i < info_len; i++) {
    print_info(&infos[i]);
  }

  return 0;
}

void print_info(struct info *info) {
  if (strlen(info->key) > 0) {
    printf("\033[0;34m%s:\033[0m %s\n", info->key, info->value);
  } else {
    printf("%s\n", info->value);
  }
}

void cpu_info(struct cpu *cpu) {
  FILE *fp = fopen("/proc/cpuinfo", "r");
  char line[200];
  int sep = ':';
  while (fgets(line, sizeof line, fp) != NULL) {
    if (strstr(line, "model name") != NULL) {
      char *val = strchr(line, sep);
      if (val != NULL) {
        strncpy(cpu->name, ltrim(val + 1), CPU_NAME_LEN);
      }
    }
    if (strstr(line, "cpu MHz") != NULL) {
      char *val = strchr(line, sep);
      if (val != NULL) {
        cpu->mhz = atof(ltrim(val + 1));
      }
    }
    if (strstr(line, "cpu cores") != NULL) {
      char *val = strchr(line, sep);
      if (val != NULL) {
        cpu->cores = atoi(ltrim(val + 1));
      }
    }
  }
  fclose(fp);
}

void get_gpu(char *output, size_t output_len) {
  FILE *fp = popen("lspci -mm", "r");
  if (fp == NULL) {
    printf("Error opening command");
    exit(1);
  }
  char line[200];
  while (fgets(line, sizeof line, fp) != NULL) {
    if (strstr(line, "3D") != NULL || strstr(line, "Display") != NULL ||
        strstr(line, "VGA") != NULL) {
      char *val = strchr(line, ' ');
      if (val != NULL) {
        strncpy(output, trim(val + 1), output_len);
        break;
      }
    }
  }
  pclose(fp);
}

int get_packages_number() {
  int n = -1;
  char packages[100];
  if (has("dpkg-query")) {
    read_line_from_cmd("/usr/bin/dpkg-query -l | wc -l", packages,
                       sizeof packages);
    n = atoi(packages);
  } else if (has("rpm")) {
    read_line_from_cmd("/usr/bin/rpm -qa | wc -l", packages, sizeof packages);
    n = atoi(packages);
  }
  return n;
}

void get_font_name(char *term_program, char *response, size_t len) {
  term_program = trim(term_program);
  if (strcasecmp(term_program, "emacs") == 0) {
    read_line_from_cmd("emacsclient --eval \"(font-get (face-attribute 'default :font) :family)\"", response, len);
    return;
  }
  if (strcasecmp(term_program, "alacritty") == 0) {
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
  strncpy(response, "-", len);
}

void get_terminal_program(char *term, size_t term_len) {
  pid_t ppid = getppid(); // this is usually bash, zsh, fish etc
  char term_pid_cmd[50];
  char term_pid[50];
  sprintf(term_pid_cmd, "ps -p %d -o ppid=", ppid);
  read_line_from_cmd(term_pid_cmd, term_pid, sizeof term_pid);
  char process_name_cmd[100];
  sprintf(process_name_cmd, "cat /proc/%s/comm", trim(term_pid));
  read_line_from_cmd(process_name_cmd, term, term_len);
}
