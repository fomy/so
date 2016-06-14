#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void listening(int child, int sockfd);

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
	server_addr.sin_port = htons(49111);
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);

	int ret = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (ret) {
		perror("Fail to bind");
		goto err;
	}

	ret = listen(sockfd, 0);

	if (ret) {
		perror("Fail to listen");
		goto err;
	}

	int client_cnt = 0;
	while (1) {
		struct sockaddr_in client_addr;
		socklen_t len;
		int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &len);
		
		if (client_fd == -1) {
			perror("fail to accept");
			goto err;
		}

		client_cnt++;

		pid_t pid = fork();

		if (pid == 0) {
			/* child */
			printf("This is child listening to %d\n", client_cnt);
			listening(client_cnt, client_fd);
			break;
		} else {
			if (client_cnt > 10) break;
		}
	}

	close(sockfd);
	return 0;
err:
	close(sockfd);
	return -1;
}

void listening(int child, int sockfd)
{
	int msg = 0;

	while(1) {
		ssize_t size = recv(sockfd, &msg, 4, 0);
		if (size < 4 || msg == -1) {
			printf("child %d ends\n", child);
			break;
		}

		printf("child %d reads %d\n", child, msg);
	}
	close(sockfd);
}
