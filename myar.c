/* * * * * * * * * * * * * * * * *
 * myar.c
 * Author: Nathan Cochran
 * Date: 7/21/2013
 * * * * * * * * * * * * * * * * */
#include "myar.h"

int main(int argc, char * argv[]) {
	int mem_len = argc-3;
	char * members[mem_len];
	char arch_name[32];

	get_arch_name(argc, argv, arch_name);
	get_members(argc, argv, members);
	
	switch(get_arch_opt(argc, argv)){
		case 'q':
			q_append(arch_name, members, mem_len);
			break;
		case 'x':
			x_extract(arch_name, members, mem_len);
			break;
		case 't':
			t_contents(arch_name);
			break;
		case 'v':
			v_contents(arch_name);
			break;
		case 'd':
			d_delete(arch_name, members, mem_len);
			break;
		case 'A':
			a_append(arch_name);
			break;
		case 'w':
			/* Not currently implemented */
			break;
	}

	free_members(members, mem_len);
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Appends all regular files in the current directory to the archive
 * Param:   char * arch_name -  Name of the archive
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void a_append(char * arch_name) {
	int fd, num_written;
	char cwd[4096];
	DIR * dir;
	struct dirent * entry;

	/* Create new archive file if none exists */
	if((fd = open(arch_name, O_WRONLY | O_CREAT | O_EXCL, 0666)) != -1 ) {
		printf("myar: creating %s\n", arch_name);
		if ((num_written = write(fd, ARMAG, SARMAG)) != SARMAG) {
			if (num_written == -1)
				perror("Error writing archive header to file");
			else
				printf("Error writing full archive header to file: %s", arch_name);
			exit(0);
		}
		close(fd);
	}
	/* If an error occurs that is not because the file already exists */
	else if(errno != EEXIST) {
		perror("Error creating new file");
		exit(0);
	}

	/* Get current working directory */
	if(getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("Error getting current working directory");
		exit(0);
	}

	/* Open current working directory */
	if((dir = opendir(cwd)) == NULL) {
		perror("Error opening current working directory");
		exit(0);
	}

	/* Append all regular files, except the archive itself  */
	while((entry = readdir(dir)) != NULL) {
		if ((strcmp(arch_name, entry->d_name) != 0) && (entry->d_type == DT_REG)) {
			append_to_arch(arch_name, entry->d_name);
		}
	}

	/* Close the directory */
	if(closedir(dir) == -1){
		perror("Error closing directory stream");
	}
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Deletes the first instance of the specified member files from the archive
 * Param:   char * arch_name -  Name of the archive
 * Param:   char * members[] -  Names of the member files to delete
 * Param:   int mem_len -  Length of the members array
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void d_delete(char * arch_name, char * members[], int mem_len) {
	int fd, num_written;
	struct ar_file * archfile;
	struct arch_itr * itr = create_arch_itr(arch_name);

	/* Unlink the archive file */
	if(unlink(arch_name) == -1) {
		perror("Error unlinking file");
		exit(0);	
	}

	/* Create new archive file with same name */
	if((fd = open(arch_name, O_WRONLY | O_CREAT, 0666)) == -1) {
		perror("Error creating new archive file");
		exit(0);
	}

	/* Write the archive magic string */
	if ((num_written = write(fd, ARMAG, SARMAG)) != SARMAG) {
		if (num_written == -1)
			perror("Error writing archive header to file");
		else
			printf("Error writing full archive header to file: %s", arch_name);
		exit(0);
	}

	/* Iterate through archived files */
	while (hasNextFile(itr)) {
		archfile = nextFile(itr);

		/* If it's listed for deletion, skip it */
		/* (and remove from members so it isn't skipped again) */
		if(is_member(archfile->name, members, mem_len)) {
			remove_member(archfile->name, members, mem_len);
		}

		/* Otherwise, write it to the new file */
		else {
			write_file_header_to(itr, fd);
			write_file_contents_to(itr, fd);

			/* Write newline character if necessary (so files begin at even bytes) */
			if(strtol(archfile->size, NULL, 10)%2 ==1) {
				write(fd, "\n", 1);
			}
		}
	}

	/* Free resources */
	free_arch_itr(itr);
	close(fd);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Extracts the specified member files from the archive, or, if no members are specified, extracts all
 * Param:   char * arch_name -  Name of the archive
 * Param:   char * members[] -  Names of the member files to extract
 * Param:   int mem_len -  Length of the members array
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void x_extract(char * arch_name, char * members[], int mem_len) {
	struct ar_file * archfile;
	struct arch_itr * itr = create_arch_itr(arch_name);

	/* If no members given, extract all */
	if (mem_len == 0) {
		while (hasNextFile(itr)) {
			extractFile(itr);
		}
	}

	/* Otherwise, extract only the members given */
	else {
		while (hasNextFile(itr)) {
			archfile = nextFile(itr);
			if(is_member(archfile->name, members, mem_len)) {
				extractFile(itr);
			}
		}
	}

	/* Free resources */
	free_arch_itr(itr);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Prints a verbose table of contents for the specified archive
 * Param:   char * arch_name -  Name of the archive
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void v_contents(char * arch_name) {
	struct ar_file * archfile;
	struct arch_itr * itr = create_arch_itr(arch_name);

	/* Iterate through archived files */
	while (hasNextFile(itr)) {
		archfile = nextFile(itr);

		/* Print verbose table of contents information */
		print_perms(archfile->mode);
		printf("%s/%s ", archfile->uid, archfile->gid);
		printf("%6s ", archfile->size);
		print_date(archfile->date);
		printf("%.16s\n", archfile->name);
	}

	/* Free resources */
	free_arch_itr(itr);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Prints a table of contents for the specified archive
 * Param:   char * arch_name -  Name of the archive
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void t_contents(char * arch_name) {
	struct ar_file * archfile;
	struct arch_itr * itr = create_arch_itr(arch_name);

	/* Print table of contents */
	while (hasNextFile(itr)) {
		archfile = nextFile(itr);
		printf("%s\n", archfile->name);
	}

	/* Free resources */
	free_arch_itr(itr);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Appends the specified member files to the archive
 * Param:   char * arch_name -  Name of the archive
 * Param:   char * members[] -  Names of the files to append
 * Param:   int mem_len -  Length of the members array
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void q_append(char * arch_name, char * members[], int mem_len) {
	int fd, num_written, i;

	/* Create new archive file if none exists */
	if((fd = open(arch_name, O_WRONLY | O_CREAT | O_EXCL, 0666)) != -1 ) {
		printf("myar: creating %s\n", arch_name);
		if ((num_written = write(fd, ARMAG, SARMAG)) != SARMAG) {
			if (num_written == -1)
				perror("Error writing archive header to file");
			else
				printf("Error writing full archive header to file: %s", arch_name);
			exit(0);
		}
		close(fd);
	}
	/* If an error occurs that is not because the file already exists */
	else if(errno != EEXIST) {
		perror("Error creating new file");
		exit(0);
	}

	/* Append the specified files to the archive */
	for (i=0; i<mem_len; i++) {
		append_to_arch(arch_name, members[i]);
	}
}

