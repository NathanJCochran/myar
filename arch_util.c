/* * * * * * * * * * * * * * * * *
 * arch_util.c
 * Author: Nathan Cochran
 * Date: 7/21/2013
 * * * * * * * * * * * * * * * * */
#include "arch_util.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Gets option from the command line and returns the corresponding character
 * Param:   int argc -  Number of arguments in argv
 * Param:   char* argv[] -  array of command line arguments
 * Return:  char -  Character representing the option chosen
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
char get_arch_opt(int argc, char* argv[]) {

	/* Ensure that an option was specified */
    if(argc < 2) {
        printf("Error: No option provided.\n");
        exit(0);
    }

	/* Determine which option */
    if(strcmp(argv[1], "-q") == 0) {return 'q';}
    if(strcmp(argv[1], "-x") == 0) {return 'x';}
    if(strcmp(argv[1], "-t") == 0) {return 't';}
    if(strcmp(argv[1], "-v") == 0) {return 'v';}
    if(strcmp(argv[1], "-d") == 0) {return 'd';}
    if(strcmp(argv[1], "-A") == 0) {return 'A';}
    if(strcmp(argv[1], "-w") == 0) {return 'w';}

	/* If invalid option was supplied */
    printf("Error: Invalid option '%s'\n", argv[1]);
    exit(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Get the name of the archive specified on the command line
 * Param:   int argc -  Number of arguments in argv
 * Param:   char * argv[] -  Array of command line arguments
 * Param:   char * arch_name -  Output parameter - archive name will be stored here
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_arch_name(int argc, char * argv[], char * arch_name) {

	/* Ensure that an archive was specified */
    if(argc < 3) {
        printf("Error: No archive specified.\n");
        exit(0);
    }

	/* Copy the archive name into the output parameter */
	strcpy(arch_name, argv[2]);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Get the names of the member files specified on the command line
 * Param:   int argc -  Number of arguments in argv
 * Param:   char * argv[] -  Array of command line arguments
 * Param:   char * members[] -  Output parameter - names of members will be stored here
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_members(int argc, char * argv[], char * members[]) {
    int i;

	/* Allocate space for each member specified
	   and copy each into output parameter array */
    for (i=0; i<argc-3; i++) {
        members[i] = (char *) malloc(16*sizeof(char));
        strcpy(members[i], argv[i+3]);
    }   
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Frees the array of member names allocated in get_members()
 * Param:   char * members[] -  Array of member names to free
 * Param:   int mem_len -  Length of members array
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void free_members(char * members[], int mem_len) {
    int i;
    for (i=0; i<mem_len; i++) {
        if (members[i] != NULL) {
            free(members[i]);
        }   
    }   
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Determine if a given file name is a member
 * Param:   char * name -  Name to check
 * Param:   char * members[] -  List of members
 * Param:   int mem_len -  Length of members array
 * Return:  bool -  True if name is a member
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool is_member(char * name, char * members[], int mem_len) {
    int i;
    for (i=0; i<mem_len; i++) {
        if((members[i] != NULL) && (strcmp(name, members[i]) == 0)) {
            return true;
        }   
    }
    return false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Removes a given member from the list of members, and frees the associated space
 * Param:   char * name -  Name of member to remove
 * Param:   char * members[] -  List of members to remove from
 * Param:   int mem_len -  Length of members array
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void remove_member(char * name, char * members[], int mem_len) {
    int i;
    for (i=0; i<mem_len; i++) {
        if((members[i] != NULL) && (strcmp(name, members[i]) == 0)) {
            free(members[i]);
            members[i] = NULL;
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Prints the date represented in date_str (seconds since epoch) in MTH DAY HH:MM YEAR format
 * Param:   char * date_str -  String containing seconds since epoch
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void print_date(char * date_str) {
    long timep;
    char time_str[20];
    struct tm * time_tm;

	/* Convert a string representing seconds since
	   epoch to a date of the form: MONTH DAY HH:MM YEAR */
    timep = strtol(date_str, NULL, 10);
    if((time_tm = localtime((time_t *) &timep)) == NULL) {
		perror("Error getting localtime");
		exit(0);
	}
    strftime(time_str, sizeof(time_str)-1, "%b %d %R %Y", time_tm);

	/* Print to output */
    printf("%s ", time_str);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Print the permissions in mode_str (octal number) in "rwxrwxrwx" format
 * Param:   char * mode_str -  String containing octal number
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void print_perms(char * mode_str) {
	unsigned long mode;
    char perms[10];

	/* Convert string representation of octal to numeric octal */
	mode = strtoul(mode_str, NULL, 8);

	/* Set each of the rwx chars */
    strcpy(perms, "---------\0");
    if (mode & S_IRUSR)
        perms[0] = 'r';
    if (mode & S_IWUSR)
        perms[1] = 'w';
    if (mode & S_IXUSR)
        perms[2] = 'x';
    if (mode & S_IRGRP)
        perms[3] = 'r';
    if (mode & S_IWGRP)
        perms[4] = 'w';
    if (mode & S_IXGRP)
        perms[5] = 'x';
    if (mode & S_IROTH)
        perms[6] = 'r';
    if (mode & S_IWOTH)
        perms[7] = 'w';
    if (mode & S_IXOTH)
        perms[8] = 'x';

	/* Print permissions string */
    printf("%s ", perms);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Append the specified member file to the archive
 * Param:   char * arch_name -  Name of the archive
 * Param:   char * member -  Name of the member to append
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void append_to_arch(char * arch_name, char * member) {
	int arch_fd, mem_fd, num_read, len;
	struct stat stat_info;
	FILE * file;
	char member_name[17];
	char c;

	/* Get properly formatted member name */
	strcpy(member_name, basename(member));
	len = strlen(member_name);
	member_name[len] = '/';
	member_name[len+1] = '\0';

	/* Stat the member to be added */
	if(stat(member, &stat_info) == -1) {
		perror("Error calling stat on member file");
		exit(0);
	}

	/* Write the header to the archive file */
	if((file = fopen(arch_name, "a")) == NULL) {
		perror("Error opening archive file");
		exit(0);
	}

	fprintf(file, "%-16s%-12ld%-6ld%-6ld%-8lo%-10lld%-2s",
			member_name,
			(long int) stat_info.st_mtime,
			(long int) stat_info.st_uid,
			(long int) stat_info.st_gid,
			(unsigned long) stat_info.st_mode,
			(long long) stat_info.st_size,
			ARFMAG
	);

	/* Close archive file */
	if(fclose(file) == EOF) {
		perror("Error closing file");
		exit(0);
	}

	/* Open archive and member files */
	if((arch_fd = open(arch_name, O_WRONLY | O_APPEND)) == -1) {
		perror("Error opening archive file");
		exit(0);
	}
	if((mem_fd = open(member, O_RDONLY)) == -1) {
		perror("Error opening member file");
		exit(0);
	}

	/* Read char from member file */
	while((num_read = read(mem_fd, &c, sizeof(c))) != 0 ) {
		if(num_read == -1) {
			perror("Error reading from member file");
			exit(0);
		}

		/* Write char to archive file */
		if(write(arch_fd, &c, sizeof(c)) == -1) {
			perror("Error writing to archive file");
			exit(0);
		}
	}

	/* Write newline character if necessary
	  (so that archive headers begin at even bytes) */
	if(stat_info.st_size%2 == 1) {
		if(write(arch_fd, "\n", 1) == -1) {
			perror("Error writing to archive file");
			exit(0);
		}
	}
	
	/* Close files */
	if(close(mem_fd) == -1) {
		perror("Error closing member file");
		exit(0);
	}
	if(close(arch_fd) == -1) {
		perror("Error closing archive file");
		exit(0);
	}
}
