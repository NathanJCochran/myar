/* * * * * * * * * * * * * * * * *
 * arch_util.h
 * Author: Nathan Cochran
 * Date: 7/21/2013
 * * * * * * * * * * * * * * * * */
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
#include <stdbool.h>

#ifndef arch_util_h
#define arch_util_h

void print_usage(char * prog_name);
char get_arch_opt(int argc, char *argv[]);
void get_arch_name(int argc, char *argv[], char * arch_name);
void get_members(int argc, char *argv[], char *members[]);
void free_members(char *members[], int mem_len);
bool is_member(char *name, char *members[], int mem_len);
void remove_member(char *name, char *members[], int mem_len);
void print_date(char *date_str);
void print_perms(char *mode_str);
void append_to_arch(char *arch_name, char *member);

#endif
