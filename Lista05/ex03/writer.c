#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

int main(int argc, char* argv[]) {
	int sleep_time = strtol(argv[1], NULL, 10);
	
	int fd = open("file.txt", O_CREAT | O_WRONLY);
	
	fprintf(stderr, "Writer try to lock file\n");
	
	flock(fd, LOCK_EX);
	fprintf(stderr, "Writer lock file to exclusive lock\n");
	sleep(sleep_time);
	flock(fd, LOCK_UN);
	
	fprintf(stderr, "Writer unlock file\n");
	
	close(fd);
}

