#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

int main (int argc, char **argv)
{
	int pipe_fd[2];
	pid_t pid;
	char wbuf[128], rbuf[128];

	bzero(wbuf, sizeof(wbuf));
	bzero(rbuf, sizeof(rbuf));

	if (pipe(pipe_fd) < 0) {
		perror("Fail to create pipe");
		return -1;
	}

	printf("this is parent %d\n", getpid());
	pid = fork();

	if (pid < 0) {
		perror("Fail to fork");
		return -1;
	} else if (pid == 0) {
		/* child */
		printf("this is child %d\n", getpid());

		int wnum = write(pipe_fd[1], "c1", strlen("c1")+1);

		if (wnum < 0) {
			perror("Child: fail to write");
			return -1;
		} else {
			printf("Parent: write %d bytes\n", wnum);
		}

		sleep(1);
		int rnum = read(pipe_fd[0], rbuf, strlen("p1")+1);

		if (rnum < 0) {
			perror("Child: fail to read");
			return -1;
		} else {
			printf("Child: read %d bytes\n", rnum);
			printf("Child: %s\n", rbuf);
		}

	} else {
		/* parent */
		printf("this is parent %d\n", getpid());
		int wnum = write(pipe_fd[1], "p1", strlen("p1")+1);

		if (wnum < 0) {
			perror("Parent: fail to write");
			return -1;
		} else {
			printf("Parent: write %d bytes\n", wnum);
		}

		sleep(1);
		int rnum = read(pipe_fd[0], rbuf, strlen("c1")+1);

		if (rnum < 0) {
			perror("Parent: fail to read");
			return -1;
		} else {
			printf("Parent: read %d bytes\n", rnum);
			printf("Parent: %s\n", rbuf);
		}

	}

	return 0;
}
