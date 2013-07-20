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
#include <ar.h>

char get_ar_opt(int argc, char * argv[]);
void q_append(int argc, char * argv[]);
char * get_archive_name(int argc, char * argv[]);
void t_contents(int argc, char * argv[]);
bool get_header(int fd, struct ar_hdr * header);
void v_contents(int argc, char * argv[]);

int main(int argc, char * argv[]) {
	
	switch(get_ar_opt(argc, argv)){
		case 'q':
			q_append(argc, argv);
			break;
		case 'x':
			break;
		case 't':
			t_contents(argc, argv);
			break;
		case 'v':
			v_contents(argc, argv);
			break;
		case 'd':
			break;
		case 'A':
			break;
		case 'w':
			break;
	}

	return 0;
}


void v_contents(int argc, char * argv[]) {
	int fd, bytes, odd;
	long timep;
	struct ar_hdr header;
	struct tm * time_tm;
	char archive_name[16], time_str[20];
	strcpy(archive_name, get_archive_name(argc, argv));

	if((fd = open(archive_name, O_RDONLY)) == -1) {
		/* ERROR OPENING FILE */
		exit(0);
	}

	/* Seek to first file header */
	lseek(fd, (off_t) SARMAG, SEEK_SET);

	/* Get header and print file name */
	while (get_header(fd, &header)) {
		timep = atoi(header.ar_date);
		time_tm = gmtime((time_t *) &timep);
		strftime(time_str, sizeof(time_str)-1, "%b %d %R %Y", time_tm);
		printf("%s ", time_str);
		printf("%.16s\n", header.ar_name);
		
		/* Seek to next file header */
		bytes = atoi(header.ar_size);
		if (bytes % 2 == 1)
			odd = 1;
		else 
			odd = 0;
		lseek(fd, (off_t) bytes+odd, SEEK_CUR);
	}


}


void t_contents(int argc, char * argv[]) {
	int fd, bytes, odd;
	char archive_name[16];
	strcpy(archive_name, get_archive_name(argc, argv));

	struct ar_hdr header;

	if((fd = open(archive_name, O_RDONLY)) == -1) {
		/* ERROR OPENING FILE */
		exit(0);
	}

	/* Seek to first file header */
	lseek(fd, (off_t) SARMAG, SEEK_SET);

	/* Get header and print file name */
	while (get_header(fd, &header)) {
		printf("%.16s\n", header.ar_name);
		
		/* Seek to next file header */
		bytes = atoi(header.ar_size);
		if (bytes % 2 == 1)
			odd = 1;
		else
			odd = 0;
		lseek(fd, (off_t) bytes+odd, SEEK_CUR);
	}
}

bool get_header(int fd, struct ar_hdr * header) {
	int num_read = read(fd, (void *) header, sizeof(struct ar_hdr));

	if (num_read == -1) {
		/* ERROR */
		exit(0);
	}

	if (num_read == sizeof(struct ar_hdr)) {
		return true;
	}
	else {
		return false;
	}
}


void q_append(int argc, char * argv[]) {
	int arch_fd, mem_fd, num_written, num_read;
	char c;
	FILE * file;
	struct stat stat_info;
	char archive_name[16];

	strcpy(archive_name, get_archive_name(argc, argv));

	if((arch_fd = open(archive_name, O_WRONLY | O_CREAT | O_EXCL, 0666)) != -1 ) {
		num_written = write(arch_fd, ARMAG, SARMAG);
		if (num_written != SARMAG) {
			/* ERROR */
			exit(0);
		}
	}

	int i;
	for (i=3; i<argc; i++) {

		stat(argv[i], &stat_info);

		/* Print the header */
		file = fopen(archive_name, "a");
		fprintf(file, "%-16s%-12ld%-6ld%-6ld%-8lo%-10lld%-2s",
				basename(argv[i]),
				(long int) stat_info.st_mtime,
				(long int) stat_info.st_uid,
				(long int) stat_info.st_gid,
				(unsigned long) stat_info.st_mode,
				(long long) stat_info.st_size,
				ARFMAG
		);
		fclose(file);

		/* Print the contents of the file */
		arch_fd = open(archive_name, O_WRONLY | O_APPEND);
		mem_fd = open(argv[i], O_RDONLY);

		while((num_read = read(mem_fd, &c, sizeof(c))) != 0 ) {
			if(num_read == -1) {
				/* ERROR */
				exit(0);
			}
			num_written = write(arch_fd, &c, sizeof(c));

			if(num_written == -1) {
				/* ERROR */
				exit(0);
			}
		}

		/* Write newline character if necessary (so files begin at even bytes) */
		if(stat_info.st_size%2 == 1) {
			num_written = write(arch_fd, "\n", 1);
		}

		close(mem_fd);
		close(arch_fd);
	}

}

char * get_archive_name(int argc, char * argv[]) {
	if(argc < 3) {
		printf("Error: No archive specified.\n");
		exit(0);
	}
	
	return argv[2];
}


char get_ar_opt(int argc, char* argv[]) {

	if(argc < 2) {
		printf("Error: No option provided.\n");
		exit(0);
	}

	if(strcmp(argv[1], "-q") == 0) {return 'q';}

	if(strcmp(argv[1], "-x") == 0) {return 'x';}

	if(strcmp(argv[1], "-t") == 0) {return 't';}

	if(strcmp(argv[1], "-v") == 0) {return 'v';}

	if(strcmp(argv[1], "-d") == 0) {return 'd';}

	if(strcmp(argv[1], "-A") == 0) {return 'A';}

	if(strcmp(argv[1], "-w") == 0) {return 'w';}

	printf("Error: Invalid option %s\n", argv[1]);
	exit(0);
}
