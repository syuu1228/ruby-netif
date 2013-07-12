#ifdef __FreeBSD__

#include "netif.h"
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if_var.h>		/* for struct ifaddr */
#include <netinet/in_var.h>
#include <arpa/inet.h>

int
setifflags(int fd, char *ifname, int value)
{
	struct ifreq		my_ifr;
	int flags;

	memset(&my_ifr, 0, sizeof(my_ifr));
	(void) strlcpy(my_ifr.ifr_name, ifname, sizeof(my_ifr.ifr_name));

 	if (ioctl(fd, SIOCGIFFLAGS, (caddr_t)&my_ifr) < 0)
		return (-1);
	flags = (my_ifr.ifr_flags & 0xffff) | (my_ifr.ifr_flagshigh << 16);

	if (value < 0) {
		value = -value;
		flags &= ~value;
	} else
		flags |= value;
	my_ifr.ifr_flags = flags & 0xffff;
	my_ifr.ifr_flagshigh = flags >> 16;
	if (ioctl(fd, SIOCSIFFLAGS, (caddr_t)&my_ifr) < 0)
		return (-1);
	return (0);
}

int
getifflags(int fd, char *ifname, int *flags)
{
	struct ifreq		my_ifr;

	memset(&my_ifr, 0, sizeof(my_ifr));
	(void) strlcpy(my_ifr.ifr_name, ifname, sizeof(my_ifr.ifr_name));

 	if (ioctl(fd, SIOCGIFFLAGS, (caddr_t)&my_ifr) < 0)
		return (-1);
	*flags = (my_ifr.ifr_flags & 0xffff) | (my_ifr.ifr_flagshigh << 16);
	return (0);
}

int
setifmtu(int fd, char *ifname, int mtu)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
	ifr.ifr_mtu = mtu;
	if (ioctl(fd, SIOCSIFMTU, (caddr_t)&ifr) < 0)
		return (-1);
	return (0);
}

int
getifmtu(int fd, char *ifname, int *mtu)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
	if (ioctl(fd, SIOCGIFMTU, (caddr_t)&ifr) < 0)
		return (-1);
	*mtu = ifr.ifr_mtu;
	return (0);
}

int
getifnetmask(int fd, char *ifname, char *mask, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0)
		return (-1);
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strncpy(mask, inet_ntoa(sin->sin_addr), size);
	return (0);
}

int 
getifaddr(int fd, char *ifname, char *addr, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
		return (-1);
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strncpy(addr, inet_ntoa(sin->sin_addr), size);
	return (0);
}

int 
getifbroadaddr(int fd, char *ifname, char *addr, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0)
		return (-1);
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strncpy(addr, inet_ntoa(sin->sin_addr), size);
	return (0);
}

int
setifaddr(int fd, char *ifname, char *addr, char *mask)
{
	struct ifreq ifr;
	static struct in_aliasreq in_addreq;
	struct sockaddr_in *saddr = &in_addreq.ifra_addr;
	struct sockaddr_in *smask = &in_addreq.ifra_mask;

	memset(&in_addreq, 0, sizeof(in_addreq));
	strncpy(in_addreq.ifra_name, ifname, IFNAMSIZ);
	smask->sin_len = saddr->sin_len = sizeof(*saddr);
	smask->sin_family = saddr->sin_family = AF_INET;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
		memcpy(&in_addreq.ifra_addr, &ifr.ifr_addr, 
			sizeof(ifr.ifr_addr));
		if (ioctl(fd, SIOCDIFADDR, &in_addreq) < 0)
			return (-1);
	}
	if (!inet_aton(addr, &saddr->sin_addr))
		return (-1);
	if (!inet_aton(mask, &smask->sin_addr))
		return (-1);

	if (ioctl(fd, SIOCAIFADDR, &in_addreq) < 0)
		return (-1);
	return (0);
}

int ifexist(int fd, char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	return (ioctl(fd, SIOCGIFADDR, &ifr) == 0);
}

int addifarp(int fd, char *ifname, char *host, char *addr)
{
	return (-1);
}

int delifarp(int fd, char *ifname, char *host)
{
	return (-1);
}

int getifarp(int fd, char *ifname, char *host, char *addr, size_t size)
{
	return (-1);
}
#endif
