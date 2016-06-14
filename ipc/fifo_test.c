#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

// for mkfifo()
#include <sys/types.h>
#include <sys/stat.h>
// for open()
#include <fcntl.h>

/*
 * mkfifo() will create a FIFO special file.
 * open() will be blocked until a pair of open() are called,
 * 	one for read and another for write.
 * With O_NONBLOK, open() for write will fail if there is no open for read.
 * But open() for read will succed even if there is no open for write.
 *
 * The data written to FIFO remains in the FIFO file 
 * 	even if the process has returned.
 */

int main(int argc, char **argv)
{
	char *path = "/tmp/fifo.file";
	pid_t pid;

	int ret = mkfifo(path, S_IRUSR|S_IWUSR);
	if (ret < 0) {
		perror("Fail to create fifo");
		if (errno != EEXIST) {
			return -1;
		}
	}

	pid = fork();

	if (pid < 0) {
		perror("Fail to fork");
		return -1;
	} else if (pid == 0) {
		printf("This is child %d\n", getpid());

		sleep(5);
		int fd = open(path, O_WRONLY|O_NONBLOCK);

		if (fd < 0) {
			perror("child: Fail to open write");
			return -1;
		}
		printf("child: open succeeds\n");

		/*int cnt = write(fd, "Fu Min", strlen("Fu Min")+1);*/
		/*printf("write %d bytes\n", cnt);*/

		close(fd);
	} else {
		printf("This is parent %d\n", getpid());

		int fd = open(path, O_RDONLY|O_NONBLOCK);

		if (fd < 0) {
			perror("parent: Fail to open for read");
			return -1;
		}
		printf("parent: open succeeds\n");

		char rbuf[16];
		int cnt = read(fd, rbuf, strlen("Fu Min")+1);
		printf("read %d bytes: %s\n", cnt, rbuf);

		close(fd);
	}

	return 0;
}
