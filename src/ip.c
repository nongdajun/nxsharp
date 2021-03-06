/* File: ip.c
 * ----------
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include "pcaphelper.h"
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

void GetIpFromDevice(uint8_t ip[4], const char DeviceName[])
{
#if defined(WIN32)
	pcap_if_t *alldevs;
	pcap_if_t *dev;
	pcap_addr_t *paddr;
	SOCKADDR_IN *sin;
	char errbuf[PCAP_ERRBUF_SIZE];
	if (pcap_findalldevs(&alldevs, errbuf) == -1) {
	fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
	//exit(1);
	}
	for (dev = alldevs; dev != NULL; dev = dev->next) {
		if(strcmp(dev->name,DeviceName) == 0)
			paddr = dev->addresses;
	}
	for (; paddr; paddr = paddr->next)
	{
		sin = (SOCKADDR_IN *)paddr->addr;
		if (sin->sin_family == AF_INET)
		{
			memcpy(ip, &sin->sin_addr.s_addr,4);
		}
	}

	pcap_freealldevs(alldevs);

#else
	int fd;
	struct ifreq ifr;

	assert(strlen(DeviceName) <= IFNAMSIZ);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(fd>0);

	strncpy(ifr.ifr_name, DeviceName, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
	{
		struct sockaddr_in *p = (void*) &(ifr.ifr_addr);
		memcpy(ip, &(p->sin_addr), 4);
	}
	else
	{
		// 查询不到IP时默认填零处理
		memset(ip, 0x00, 4);
	}

	close(fd);
#endif
}

