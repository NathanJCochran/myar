#include <stdbool.h>
#include <utime.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ar.h>

/* Header iterator struct */
struct arch_itr {
    int fd; 
    struct ar_hdr * header;
    struct ar_file * archfile;
};

/* Printable ar_hdr struct */
/* Includes the terminating null byte */
struct ar_file {
    char name[17];
    char date[13];
    char uid[7], gid[7];
    char mode[9];
    char size[11];
    char fmag[3];
};

struct arch_itr *create_arch_itr(char *archive_name);

void init_arch_itr(struct arch_itr *itr);

void free_arch_itr(struct arch_itr * itr);

bool hasNextFile(struct arch_itr *itr);

struct ar_file * nextFile(struct arch_itr *itr);

void extractFile(struct arch_itr *itr);

void write_file_header_to(struct arch_itr * itr, int fd);

void write_file_contents_to(struct arch_itr * itr, int fd);

long get_offset(char * size);

void get_archfile(struct ar_hdr *header, struct ar_file *ret);

bool get_header(int fd, struct ar_hdr *header);

void get_hdr_field(char *hdr_field, int len, char *buf);
