#ifndef PSTREE_H_
#define PSTREE_H_
#include <sys/types.h>

struct pstree_node {
    char name[128];
    pid_t pid;
    pid_t ppid;
    struct pstree_node *parent;
    struct pstree_node *next;
};

int catalog_process(pid_t pid);

struct pstree_node *get_pstree_head();

int pstree_delete();

#endif // PSTREE_H_
