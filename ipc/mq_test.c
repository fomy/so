#include <mqueue.h>

int main(int argc, char** argv)
{

	int mq_fd = open("/tmp/mq", O_CREAT, S_IRUSR|S_IWUSR);

	close(mq_fd);
	return 0;
}
