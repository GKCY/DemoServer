#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#define MAXLINE 1024

void str_echo(int sockfd);
void sig_chld(int signo);

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

void str_echo(int sockfd)
{
	char		buff[MAXLINE];
	int			n;

	for ( ; ; ){
		if ((n = read(sockfd, buff, MAXLINE)) > 0){
			buff[n] = '\0';
			write(sockfd, buff, n);
		}
		else if (n < 0 && errno == EINTR)
			continue;
		else if (n < 0){
			printf("str_echo:read error\n");
			return;
		}
		else if (n == 0){
			break;
		}
	}
}

int main(int argc, char const *argv[])
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t client;
	struct sockaddr_in servaddr, cliaddr;
	unsigned short server_port = 8080;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		perror("Error:Creating socket failed\n");
		exit(1);
	}


	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0)
	{
		perror("Error:bind to serverport");
		close(listenfd);
		exit(0);
	}


	listen(listenfd, 10);
	signal(SIGCHLD, sig_chld);

	for (;;)
	{
		client = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &client);
		if ((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			//printf("pid:%d  You have a connect.\n", getpid());
			exit(0);
		}
		close(connfd);	
	}




	return 0;
}
