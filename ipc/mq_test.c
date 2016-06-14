#include <errno.h>
#include <mqueue.h>

// gcc mq_test.c -o mq_test -lrt

int main(int argc, char** argv)
{

	mqd_t mq_fd = mq_open("/tmp/mq", O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);

	if (mq_fd < 0) {
		perror("Fail to create mq");
		assert(mq_fd == -1);
		if (errno == EEXIST) {
			mq_fd = mq_open("/tmp/mq", O_WRONLY);
			if (mq_fd < 0) {
				perror("Fail to open mq");
				return -1;
			}
		} else {
			return -1;
		}
	}

	/*char *msg = "this is msg from Min";*/
	/*mq_send(mq_fd, msg, strlen(msg)+1, 0);*/


	struct mq_attr mqstat;
	bzero(&mqstat, sizeof(mqstat));
	int ret = mq_getattr(mq_fd, &mqstat);

	printf("%ld, %ld, %ld, %ld\n", mqstat.mq_flags, mqstat.mq_maxmsg,
			mqstat.mq_msgsize, mqstat.mq_curmsgs);

	mq_close(mq_fd);
	return 0;
}
