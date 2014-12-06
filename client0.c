#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8888

/*
	client: tcp.
	serverIP:127.0.0.1 
*/

int main()
{
	int s;
	struct sockaddr_in server_addr;
	char IP[] = "127.0.0.1"; 
	char buffer[100];

	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0){
		printf("socket error\n");
		exit(1);
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;	
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, IP, &server_addr.sin_addr);

	connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	printf("client:\n");
	printf("send   :");
	scanf("%s",buffer);
	write(s, buffer, strlen(buffer)+1);
	read(s, buffer, 100);
	printf("receive:%s\n",buffer);

	close(s);
	exit(0);
}
