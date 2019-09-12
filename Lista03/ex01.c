#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Error: How to run:\n");
		fprintf(stderr, "ex01 file\n");
		return -1;
	}
	
	struct stat file_stat;
	int status_stat = stat(argv[1], &file_stat);
	if (status_stat == -1) {
		fprintf(stderr, "Error: file dont exists\n");
		return -1;
	}
	
	printf("File %s infos: \n", argv[1]);
	printf("User ID of owner: %d\n", file_stat.st_uid);
	printf("Group ID of owner: %d\n", file_stat.st_gid);
	printf("Number of hard links: %ld\n", file_stat.st_nlink);
	printf("File size: %ld bytes\n", file_stat.st_size);
	
	return 0;
}
