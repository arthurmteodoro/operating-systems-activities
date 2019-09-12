#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

struct linux_dirent {
   long           d_ino;
   off_t          d_off;
   unsigned short d_reclen;
   char           d_name[];
};

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
	int fd, nread;
	char buf[BUF_SIZE];
	struct linux_dirent *d;
	int bpos;

	fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_NONBLOCK | O_DIRECTORY | O_CLOEXEC);

	int finish = 0;
	while(!finish) {
		nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);

		if (nread == 0) finish = 1;

		for (bpos = 0; bpos < nread;) {
			d = (struct linux_dirent *) (buf + bpos);
			printf("Inod: %ld \t\t%s\n", d->d_ino,d->d_name);
			bpos += d->d_reclen;
		}
	}

	return 0;
}
