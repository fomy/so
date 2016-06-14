#include <stdlib.h>
#include <stdio.h>
#include <socket.h>

int main(int argc, char** argv)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		printf("Fail to create socket\n");
		return -1;
	}

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5000);
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);

	int ret = bind(sockfd, &server_addr, sizeof(server_addr));

	if (ret) {
		perror("Fail to bind");
		goto err;
	}

	ret = listen(sockfd, 0);

	if (ret) {
		perror("Fail to listen");
		goto err;
	}

	while (1) {
		struct sockaddr_in client_addr;
		int client_fd = accept(sockfd, &client_addr, sizeof(client_addr));
		
		if (client_fd == -1) {
			perror("fail to accept");
			goto err;
		}
	}

err:
	close(sockfd);
	return -1;
}
