/* * * * * * * * * * * * * * * * *
 * arch_itr.c
 * Author: Nathan Cochran
 * Date: 7/21/2013
 * * * * * * * * * * * * * * * * */

#include "arch_itr.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates an archive iterator for the specified archive
 * Param:   char * archive_name -  Name of archive
 * Return:  struct arch_itr * -  Pointer to the archive iterator
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct arch_itr * create_arch_itr(char * archive_name) {
	struct arch_itr * itr = (struct arch_itr *) malloc(sizeof(struct arch_itr));
	init_arch_itr(itr);

	/* Open archive file */
	if((itr->fd = open(archive_name, O_RDONLY)) == -1) {
		perror("Error opening archive file");
		exit(0);
	}

	/* Seek to first file header */
	if(lseek(itr->fd, (off_t) SARMAG, SEEK_SET) == -1) {
		perror("Error seeking in archive file");
		exit(0);
	}

	return itr;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Initialize the fields of the archive iterator
 * Param:   struct arch_itr * itr -  Iterator to initialize
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void init_arch_itr(struct arch_itr * itr) {
	itr->header = (struct ar_hdr *) malloc(sizeof(struct ar_hdr));
	itr->archfile = (struct ar_file *) malloc(sizeof(struct ar_file));
	if((itr->header == NULL) || (itr->archfile == NULL)) {
		perror("Error allocating memory");
		exit(0);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Free the memory allocated to the archive iterator
 * Param:   struct arch_itr * itr -  Iterator to free
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void free_arch_itr(struct arch_itr * itr) {
	if(close(itr->fd) == -1) {
		perror("Error closing archive file");
		exit(0);
	}
	free(itr->header);
	free(itr->archfile);
	free(itr);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Returns true if the iterator has a next file, advances the offset associated with the fd for the archive
 * Param:   struct arch_itr * itr -  Iterator to check
 * Return:  bool -  True if the archive has a next file
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool hasNextFile(struct arch_itr * itr) {

	/* If there is another file header */
    if (get_header(itr->fd, itr->header)) {

		/* Get the file header */
        get_archfile(itr->header, itr->archfile);

        /* Seek to start of next file header */
        if(lseek(itr->fd, (off_t) get_offset(itr->archfile->size), SEEK_CUR) == -1) {
			perror("Error seeking in archive file");
			exit(0);
		}

		return true;
    }
    else {
        return false;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Returns an ar_file struct containing info on the next archive in the file
 * Param:   struct arch_itr * itr -  Iterator to get next file from
 * Return:  struct ar_file * -  Contains information on the next file in the archive
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct ar_file * nextFile(struct arch_itr * itr) {
	return itr->archfile;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Extracts the "current" file from the archive
 * Param:   struct arch_itr * itr -  Iterator to extract current file from
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void extractFile(struct arch_itr * itr) {
	int fd;
	unsigned long mode;
	struct utimbuf times;

	/* Get mode and timestamp of archived file */
	mode = strtoul(itr->archfile->mode, NULL, 8);
	times.actime = times.modtime = (time_t) strtoul(itr->archfile->date, NULL, 10);

	/* Open/create file to extract into */
	if ((fd = open(itr->archfile->name, O_WRONLY | O_CREAT, mode)) == -1) {
		perror("Error opening/creating file");
		exit(0);
	}

	/* Write contents to the file */
	write_file_contents_to(itr, fd);

	if(close(fd) == -1) {
		perror("Error closing extracted file");
		exit(0);
	}

	/* Restore access and modification timestamps */
	if(utime(itr->archfile->name, &times) == -1) {
		perror("Error setting timestamp of extracted file");
		exit(0);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Writes the current file header to the file specified by the open file descriptor fd
 * Param:   struct arch_itr * itr -  Iterator to write from
 * Param:   int fd -  Open file descriptor of file to write to
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void write_file_header_to(struct arch_itr * itr, int fd) {
	if(write(fd, (void *) itr->header, sizeof(struct ar_hdr)) == -1) {
		perror("Error writing file header");
		exit(0);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Write the current file's contents to the file specified by the open file descriptor fd
 * Param:   struct arch_itr * itr -  Iterator to write from
 * Param:   int fd -  Open file descriptor of file to write to
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void write_file_contents_to(struct arch_itr * itr, int fd) {
	char c;

	/* Seek back to start of file's contents in archive */
	if(lseek(itr->fd, (off_t) -(get_offset(itr->archfile->size)), SEEK_CUR) == -1) {
		perror("Error seeking in archive file");
		exit(0);
	}
	
	/* Write contents to file */
	int i;
	for(i=0; i<strtol(itr->archfile->size, NULL, 10); i++) {
		if(read(itr->fd, &c, sizeof(c)) == -1) {
			perror("Error reading from archive file");
			exit(0);
		}
		if(write(fd, &c, sizeof(c)) == -1) {
			perror("Error writing file contents");
			exit(0);
		}
	}

	/* Seek ahead to even byte, if necessary */
	if (strtol(itr->archfile->size, NULL, 10)%2 == 1) {
		if(lseek(itr->fd, (off_t) 1, SEEK_CUR) == -1) {
			perror("Error seeking in archive file");
			exit(0);
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Takes a string representing the size of a file, returns the number of bytes to seek to the next file header in the archive
 * Param:   char * size -  String representing the size of the current file
 * Return:  long -  The number of bytes to seek past to get to the next file header
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
long get_offset(char * size) {
	long bytes;
	bytes = strtoul(size, NULL, 10);
    if (bytes % 2 == 1)
        bytes += 1;
	return bytes;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Reads the next file header into the specified ar_hdr struct, returning true on success, false if no header was found (end of archive reached)
 * Param:   int fd -  File descriptor for the archive
 * Param:   struct ar_hdr * header -  Header struct to read into (NOTE: the fields of this struct are not null-terminated!)
 * Return:  bool -  True if header was successfully read in, false if end of file reached
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool get_header(int fd, struct ar_hdr * header) {
    int num_read = read(fd, (void *) header, sizeof(struct ar_hdr));

    if (num_read == -1) {
        perror("Error reading header from archive file");
        exit(0);
    }

    if (num_read == sizeof(struct ar_hdr)) {
        return true;
    }
    else {
        return false;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Takes an ar_hdr struct (which cannot be used as is) and converts it to an ar_file
 * Param:   struct ar_hdr * header -  ar_hdr struct containing the header of a file in the archive
 * Param:   struct ar_file * ret -  Contains the same information, but wih null-terminated strings and no whitespace
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_archfile(struct ar_hdr * header, struct ar_file * ret) {
        
    get_hdr_field(header->ar_name, sizeof(header->ar_name), ret->name);
    get_hdr_field(header->ar_date, sizeof(header->ar_date), ret->date);
    get_hdr_field(header->ar_uid, sizeof(header->ar_uid), ret->uid);
    get_hdr_field(header->ar_gid, sizeof(header->ar_gid), ret->gid);
    get_hdr_field(header->ar_mode, sizeof(header->ar_mode), ret->mode);
    get_hdr_field(header->ar_size, sizeof(header->ar_size), ret->size);
    get_hdr_field(header->ar_fmag, sizeof(header->ar_fmag), ret->fmag);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Takes a field of a given length and copies the information into buf (null-terminated, no whitespace)
 * Param:   char * hdr_field -  ar_hdr field to get information from
 * Param:   int len -  Length of the character array
 * Param:   char * buf -  Output parameter - the ar_hdr field's info will be copied here
 * Return:  void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_hdr_field(char * hdr_field, int len, char * buf) {
    int i;
    int j=0;

	/* Read in the header field, ignoring whitespace */
    for (i=0; i<len; i++) {
        if (hdr_field[i] != ' ') {
            buf[j] = hdr_field[i];
            j++;
        }   
    }
    buf[j] = '\0';

	/* Truncate the trailing '/' appended to some file names*/
	if (buf[j-1] == '/') {
		buf[j-1] = '\0';
	}
}


