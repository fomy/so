#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char** argv)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(49111);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

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
	msg = -1;
	printf("send -1\n");
	if (send(sockfd, &msg, 4, 0) < 4) goto err;

	return 0;
err:
	perror("Fail to send 4 bytes");
	return -1;
}
