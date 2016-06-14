#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

void alarm_handler(int sig);

int main (int argc, char **argv)
{
	signal(SIGALRM, alarm_handler);
	int ret = alarm(5);
	if (ret != 0) {
		perror("fail to alarm");
		return -1;
	}

	while(1) {
		sleep(1);
		printf("wake up!\n");
	}

	return 0;
}

void alarm_handler(int sig)
{
	printf("hey!\n");
}
