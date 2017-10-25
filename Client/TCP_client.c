#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>

#define MAXLINE 1024

int max(int a, int b);
void strcli(FILE *fp, int sockfd);

int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

void strcli(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;

	stdineof = 0;
	FD_ZERO(&rset);
	for (; ; )
	{
		if(stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset))	
		{
			if ((n == read(sockfd, buf, MAXLINE))== 0)
			{
				if(stdineof == 1)
					return;

				else
					printf("strcli: server terminated prematurely");
			}
			write(fileno(stdout), buf, n);
		}

		if(FD_ISSET(fileno(fp), &rset))
		{
			if((n = read(fileno(fp), buf, MAXLINE))== 0)
			{
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			write(sockfd, buf, n);
		}
	}
}



int main(int argc, char const *argv[])
{
	int sockfd;
	char IPv4[20];
	struct sockaddr_in servaddr;
	unsigned short port = 8080;
	// printf("请输入IP地址：");
	// scanf("%s", IPv4);
	// printf("请输入端口号：");
	// scanf("%hu", &port);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	// servaddr.sin_addr.s_addr = inet_addr(IPv4);
	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	strcli(stdin, sockfd);

	
	return 0;
}