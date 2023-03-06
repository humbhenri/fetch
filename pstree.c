#include "pstree.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "helpers.h"

static struct pstree_node *ll_head;

struct pstree_node *get_pstree_head()
{
    return ll_head;
}

int ll_create(char *procname, pid_t pid, pid_t ppid)
{
    struct pstree_node *node = malloc(sizeof(struct pstree_node));
    strcpy(node->name, procname);
    node->pid = pid;
    node->ppid = ppid;
    node->next = ll_head;
    node->parent = NULL;
    ll_head = node;
    return 0;
}

int catalog_process(pid_t pid)
{
    if (pid == 1) {
        return 0;
    }
    char filename[256];
    sprintf(filename, "/proc/%d/status", pid);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 1;
    }
    char linebuf[256];
    char *key, *value;
    char ppid[32];
    char procname[256];
    while (fgets(linebuf, sizeof linebuf, fp) != NULL) {
        key = strtok(linebuf, ":");
        value = strtok(NULL, ":");
        if (key != NULL && value != NULL) {
            key = trim(key);
            value = trim(value);
            if (strcmp(key, "PPid") == 0) {
                strcpy(ppid, value);
            }
            if (strcmp(key, "Name") == 0) {
                strcpy(procname, value);
            }
        }
    }
    ll_create(procname, pid, atoi(ppid));
    return catalog_process(atoi(ppid));
}

int pstree_delete()
{
    struct pstree_node *next = ll_head;
    while (next != NULL) {
        struct pstree_node *tmp = next;
        next = next->next;
        free(tmp);
    }
    return 0;
}
