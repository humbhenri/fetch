/* neofetch like program */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <stdbool.h>

#define CPU_NAME_LEN 200

char* os_name();

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

char *ltrim(char *s);

char *rtrim(char *s);

char *trim(char *s);

void read_line_from_cmd(const char *cmd, char *output, size_t len);

void get_gpu(char *output, size_t output_len);

bool has(const char* program);

int get_packages_number();

int main ()
{
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
  sprintf(uptime, "%ld days %ld hours %ld minutes %ld seconds",
          days, hours, minutes, seconds);

  char ram[200];
  sprintf(ram, "%lu MiB/%lu MiB", 
          (s_info.totalram - s_info.freeram) / 1024 / 1024, 
          s_info.totalram / 1024 / 1024);

  struct cpu c_info;
  cpu_info(&c_info);
  char cpu_text[500];
  sprintf(cpu_text, "%s (%d) @ %lf MHz", trim(c_info.name), c_info.cores, c_info.mhz);

  char packages[100];
  int n_packages = get_packages_number();
  sprintf(packages, "%d", n_packages);

  char resolution[100];
  read_line_from_cmd("xdpyinfo | awk '/dimensions:/ { print $2; exit }'", resolution, sizeof resolution);

  char gpu[200];
  get_gpu(gpu, sizeof gpu);

  struct info infos[] = { 
    {.key = "", .value = userhost}
    ,{.key = "OS", .value = os_name()}
    ,{.key = "Kernel", .value = kernel}
    ,{.key = "Uptime", .value = uptime}
    ,{.key = "Packages", .value = packages}
    ,{.key = "Shell", .value = getenv("SHELL")}
    ,{.key = "Resolution", .value = trim(resolution)}
    ,{.key = "WM", .value = getenv("XDG_SESSION_DESKTOP")}
    /* ,{.key = "WM Theme", .value = "Adwaita"} */
    /* ,{.key = "Theme", .value = "Adwaita"} */
    /* ,{.key = "Icons", .value = "Adwaita"} */
    ,{.key = "Terminal", .value = getenv("TERM")}
    /* ,{.key = "Terminal font", .value = "Comic Sans"} */
    ,{.key = "CPU", .value = trim(cpu_text)}
    ,{.key = "GPU", .value = gpu}
    ,{.key = "Memory", .value = ram}
  };

  size_t info_len = sizeof infos / sizeof(struct info);
  for (size_t i = 0; i < info_len; i++) {
    print_info(&infos[i]);
  }

  return 0;
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

void print_info(struct info *info)
{
  if (strlen(info->key) > 0) {
    printf("\033[0;34m%s:\033[0m %s\n", info->key, info->value);
  } else {
    printf("%s\n", info->value);
  }
}

void cpu_info(struct cpu *cpu)
{
  FILE *fp = fopen("/proc/cpuinfo", "r");
  char line[200];
  int sep = ':';
  while (fgets(line, sizeof line, fp) != NULL) {
    if (strstr(line, "model name") != NULL) {
      char *val = strchr(line, sep);
      if (val != NULL) {
        strncpy(cpu->name, ltrim(val+1), CPU_NAME_LEN);
      }
    }
    if (strstr(line, "cpu MHz") != NULL) {
      char *val = strchr(line, sep);
      if (val != NULL) {
        cpu->mhz = atof(ltrim(val+1));
      }
    }
    if (strstr(line, "cpu cores") != NULL) {
      char *val = strchr(line, sep);
      if (val != NULL) {
        cpu->cores = atoi(ltrim(val+1));
      }
    }
  }
  fclose(fp);
}

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

void get_gpu(char *output, size_t output_len)
{
  FILE *fp = popen("lspci -mm", "r");
  if (fp == NULL) {
    printf("Error opening command");
    exit(1);
  }
  char line[200];
  while (fgets(line, sizeof line, fp) != NULL) {
    if (strstr(line, "3D") != NULL || strstr(line, "Display") != NULL || strstr(line, "VGA")) {
      char *val = strchr(line, ' ');
      if (val != NULL) {
        strncpy(output, trim(val+1), output_len);
        break;
      }
    }
  }
  pclose(fp);
}

int get_packages_number()
{
  int n = -1;
  char packages[100];
  if (has("dpkg-query")) {
    read_line_from_cmd("/usr/bin/dpkg-query -l | wc -l", packages, sizeof packages);
    n = atoi(packages);
  } else if (has("rpm")) {
    read_line_from_cmd("/usr/bin/rpm -qa | wc -l", packages, sizeof packages);
    n = atoi(packages);
  }
  return n;
}

bool has(const char* program)
{
  char find[200];
  sprintf(find, "type -p %s >/dev/null", program);
  int res = system(find);
  return res == 0;
}
