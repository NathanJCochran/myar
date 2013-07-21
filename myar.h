#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <ar.h>
#include "arch_itr.h"
#include "arch_util.h"

#ifndef myar_h
#define myar_h

void a_append(char *arch_name);
void d_delete(char *arch_name, char *members[], int mem_len);
void x_extract(char *arch_name, char *members[], int mem_len);
void v_contents(char *arch_name);
void t_contents(char *arch_name);
void q_append(char *arch_name, char *members[], int mem_len);

#endif
