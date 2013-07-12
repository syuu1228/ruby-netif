#ifdef __linux__

#include "netif.h"
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <netinet/ether.h>

int
setifflags(int fd, char *ifname, int value)
{
	struct ifreq		my_ifr;
	int flags;

	memset(&my_ifr, 0, sizeof(my_ifr));
	(void) strncpy(my_ifr.ifr_name, ifname, sizeof(my_ifr.ifr_name));

 	if (ioctl(fd, SIOCGIFFLAGS, (caddr_t)&my_ifr) < 0)
		return (-1);
	flags = my_ifr.ifr_flags;

	if (value < 0) {
		value = -value;
		flags &= ~value;
	} else
		flags |= value;
	my_ifr.ifr_flags = flags;
	if (ioctl(fd, SIOCSIFFLAGS, (caddr_t)&my_ifr) < 0)
		return (-1);
	return (0);
}

int
getifflags(int fd, char *ifname, int *flags)
{
	struct ifreq		my_ifr;

	memset(&my_ifr, 0, sizeof(my_ifr));
	(void) strncpy(my_ifr.ifr_name, ifname, sizeof(my_ifr.ifr_name));

 	if (ioctl(fd, SIOCGIFFLAGS, (caddr_t)&my_ifr) < 0)
		return (-1);
	*flags = my_ifr.ifr_flags;
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
	sin = (struct sockaddr_in *)&ifr.ifr_netmask;
	strncpy(mask, inet_ntoa(sin->sin_addr), size);
	return (0);
}

int
setifaddr(int fd, char *ifname, char *addr, char *mask)
{
	struct ifreq ifr;
	struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr.ifr_addr);

	sin->sin_family = AF_INET;
	if (!inet_aton(addr, &sin->sin_addr))
		return (-1);

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
		return (-1);

	if (!inet_aton(mask, &sin->sin_addr))
		return (-1);

	if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
		return (-1);

	return (0);
}

int 
getifaddr(int fd, char *ifname, char *mask, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
		return (-1);
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strncpy(mask, inet_ntoa(sin->sin_addr), size);
	return (0);
}

int 
getifbroadaddr(int fd, char *ifname, char *mask, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
		return (-1);
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strncpy(mask, inet_ntoa(sin->sin_addr), size);
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
	struct arpreq req;
	struct sockaddr_in *nhost = (struct sockaddr_in *)(&req.arp_pa);
	struct ether_addr *naddr = (struct ether_addr *)(req.arp_ha.sa_data);

	memset(&req, 0, sizeof(req));
	if (!inet_aton(host, &nhost->sin_addr))
		return (-1);
	req.arp_pa.sa_family = AF_INET;
	if (!ether_aton_r(addr, naddr))
		return (-1);
	req.arp_ha.sa_family = ARPHRD_ETHER;
	strncpy(req.arp_dev, ifname, sizeof(req.arp_dev));
	req.arp_flags = ATF_PERM | ATF_COM;
	if (ioctl(fd, SIOCSARP, &req) < 0)
		return (-1);
	return (0);
}

int delifarp(int fd, char *ifname, char *host)
{
	struct arpreq req;
	struct sockaddr_in *nhost = (struct sockaddr_in *)(&req.arp_pa);

	memset(&req, 0, sizeof(req));
	if (!inet_aton(host, &nhost->sin_addr))
		return (-1);
	req.arp_pa.sa_family = AF_INET;
	strncpy(req.arp_dev, ifname, sizeof(req.arp_dev));
	req.arp_flags = ATF_PERM | ATF_COM;
	if (ioctl(fd, SIOCDARP, &req) < 0)
		return (-1);
	return (0);
}

int getifarp(int fd, char *ifname, char *host, char *addr, size_t size)
{
	struct arpreq req;
	struct sockaddr_in *nhost = (struct sockaddr_in *)(&req.arp_pa);
	struct ether_addr *naddr = (struct ether_addr *)(req.arp_ha.sa_data);

	memset(&req, 0, sizeof(req));
	if (!inet_aton(host, &nhost->sin_addr))
		return (-1);
	req.arp_pa.sa_family = AF_INET;
	strncpy(req.arp_dev, ifname, sizeof(req.arp_dev));
	req.arp_flags = ATF_PERM | ATF_COM;
	if (ioctl(fd, SIOCGARP, &req) < 0)
		return (-1);
	strncpy(addr, ether_ntoa(naddr), size);
	return (0);
}
#endif
