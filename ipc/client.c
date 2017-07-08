#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <aio.h>

void aio_write_cbk(sigval_t sigval);

int main(int argc, char** argv)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(49111);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	int ret = connect(sockfd, (struct sockaddr*)&server_addr, 
            sizeof(server_addr));

	if (ret != 0) {
		perror("Fail to connect");
		return -1;
	}

	int msg = 1;
	printf("send 1\n");
	if (send(sockfd, &msg, 4, 0) < 4) goto err;

	sleep(1);

	msg = 2;
	printf("send 2\n");
	if (send(sockfd, &msg, 4, 0) < 4) goto err;

	sleep(1);
	msg = 3;
	printf("send 3\n");
	if (send(sockfd, &msg, 4, 0) < 4) goto err;

    // starting to aio
    struct aiocb my_aiocb;
    my_aiocb.aio_fildes = sockfd;
    msg = -1;
    my_aiocb.aio_buf = &msg;
    my_aiocb.aio_nbytes = 4;

    // set callback
    my_aiocb.aio_sigevent.sigev_notify = SIGEV_THREAD; // SIGEV_SIGNAL
    my_aiocb.aio_sigevent.sigev_notify_function = aio_write_cbk;
    my_aiocb.aio_sigevent.sigev_notify_attributes = NULL;
    my_aiocb.aio_sigevent.sigev_value.sival_ptr = &my_aiocb;

	printf("send -1\n");
    ret = aio_write(&my_aiocb);
    if (ret < 0) {
        printf("Fail to enqueue!\n");
        goto err;
    }

    while(aio_error(&my_aiocb) != 0) {};
    
    printf("aio completes!\n");
    sleep(1);

    close(sockfd);
	return 0;
err:
	perror("Fail to send 4 bytes");
    close(sockfd);
	return -1;
}

void aio_write_cbk(sigval_t sigval) {
    struct aiocb *req = sigval.sival_ptr;
    if (aio_error(req) == 0) {
        printf("in callback!\n");
        aio_return(req);
    } else {
        printf("something wrong!\n");
    }
}
