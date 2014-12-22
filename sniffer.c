#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

int main(int argc,char **argv)
{
	int sock,n,i;
	char buffer[ETH_FRAME_LEN];
	struct ifreq ifr;
	int err;
	char *ethname = "eth0";
	struct ethhdr *p_eth;	
	struct iphdr *p_ip;
	struct tcphdr *p_tcp;
	/*struct udphdr *p_udp;*/
	/*
		设置socket使其接收IP包
	*/
	sock = socket(AF_INET, SOCK_PACKET, htons(0x0003));
	if(sock < 0){
		perror("socket!\n");
		exit(1);
	}
	/*
		设置网卡为混杂模式
	*/
	strcpy(ifr.ifr_name,ethname);
	err = ioctl(sock, SIOCGIFFLAGS, &ifr);
	if(err < 0){
		close(sock);
		perror("ioctl!\n");
		exit(1);
	}
	/*
		设置相应的标志
	*/
	ifr.ifr_flags |= IFF_PROMISC;
	err = ioctl(sock, SIOCGIFFLAGS, &ifr);
	if(err < 0){
		close(sock);
		perror("set flags error!\n");
		exit(1);
	}
	
	while(1){
		n = recvfrom(sock, buffer, ETH_FRAME_LEN, 0, NULL, NULL);
		
		/*
			以太网帧结构：目的地址(6) + 源地址(6) + 类型(2) + 帧内数据(46-1500) + CRC(4)
				       
			IP 帧结构：头部（20） + 数据
			TCP帧结构：头部（20） + 数据
			UDP帧结构：头部（8） + 数据
		*/
		if(n < 42){
			continue;
		}
		printf("-----------------------------------\n");	
		printf("%d bytes receive\n", n);
		/*
			MAC 地址
		*/
		p_eth = (struct ethhddr*)buffer;
		printf("Destination MAC:");
		for(i = 0; i < ETH_ALEN-1; ++i){
			printf("%02x:",p_eth->h_dest[i]);
		}
		printf("%02x\n",p_eth->h_dest[ETH_ALEN-1]);
		
		printf("Source      MAC:");
		for(i = 0; i < ETH_ALEN-1; ++i){
			printf("%02x:",p_eth->h_source[i]);

		}
		printf("%02x\n",p_eth->h_source[ETH_ALEN-1]);
		
		/*
			MAC帧类型：IP 0x0800, ARP 0x0806, RARP 0x8035
				  PPPOE发现阶段 0x8863
				  PPPOE会话阶段 0x8864
		*/
		if(ntohs(p_eth->h_proto) == 0x0800){
			p_ip = (struct iphdr*)(buffer + ETH_HLEN);
			printf("Source      IP:%s\n",inet_ntoa(p_ip->saddr));
			printf("Destination IP:%s\n",inet_ntoa(p_ip->saddr));
			/*
				IP帧类型： TCP 6, UDP 17
			*/
			if(p_ip->protocol == 6){
				int len = 0;
				char *daddr = NULL;
				printf("TCP:\n");
				p_tcp = (struct tcphdr*)(p_ip + p_ip->ihl * 4);
				/*
					数据地址：p_tcp + 20;
					数据长度： n - 18 - IP头部 - TCP头部
				*/
				daddr = (char*)(p_tcp + 20);
				len = n - 18 - p_ip->ihl * 4 - 20;
				printf("Date length:%d\n",len);
			}
		}else if(ntohs(p_eth->h_proto) == 0x0806){
			printf("ARP\n");
		}else if(ntohs(p_eth->h_proto) == 0x8035){
			printf("RARP\n");
		}else if(ntohs(p_eth->h_proto) == 0x8864){
			printf("PPPOE\n");
		}else{
			printf("others\n");
		}
	}
	
}

