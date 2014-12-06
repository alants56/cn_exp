#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>

#define PORT 8888
#define LEN 2

/*
	server: using linux socket for tcp  
*/

int main()
{
	int ss,sc;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int err;

	ss = socket(AF_INET, SOCK_STREAM, 0);
	if(ss < 0){
		printf("socket error!");
		exit(1);
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	err = bind(ss, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(err < 0){
		printf("bind error!");
		exit(1);
	}
	
	err = listen(ss, LEN);
	if(err < 0){
		printf("listen error!");
		exit(1);
	}
	
	while(1){
		int addrlen = sizeof(server_addr);
		ssize_t size = 0;
		char buffer[100];

		sc = accept(ss, (struct sockaddr*)&client_addr, &addrlen);
		if(sc < 0){
			continue;
		}

		size = read(sc,buffer,100);
		if(size == 0){
			continue;
		}
		printf("server:\n");	
		printf("receive:%s\n",buffer);
		printf("send   :");
		scanf("%s",buffer);
		write(sc,buffer,strlen(buffer)+1);
		close(sc);
		exit(0);
	}
}
