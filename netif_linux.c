#ifdef __linux__

#include "netif.h"
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <netinet/ether.h>
#include <errno.h>
#include <ruby.h>

static int 
socket_open(void)
{
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		rb_raise(rb_eException, "%s", strerror(errno));
	return fd;
}

static char * 
__ether_ntoa(struct ether_addr *addr, char *buf, size_t size)
{
	snprintf(buf, size, "%02x:%02x:%02x:%02x:%02x:%02x",
		addr->ether_addr_octet[0],
		addr->ether_addr_octet[1],
		addr->ether_addr_octet[2],
		addr->ether_addr_octet[3],
		addr->ether_addr_octet[4],
		addr->ether_addr_octet[5]);
	return buf;
}

static void
setifflags(char *ifname, int value)
{
	struct ifreq ifr;
	int flags;
	int fd = socket_open();

	memset(&ifr, 0, sizeof(ifr));
	(void) strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

 	if (ioctl(fd, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	flags = ifr.ifr_flags;

	if (value < 0) {
		value = -value;
		flags &= ~value;
	} else
		flags |= value;
	ifr.ifr_flags = flags;
	if (ioctl(fd, SIOCSIFFLAGS, (caddr_t)&ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
}

static int
getifflags(char *ifname)
{
	struct ifreq ifr;
	int fd = socket_open();

	memset(&ifr, 0, sizeof(ifr));
	(void) strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

 	if (ioctl(fd, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
	return (ifr.ifr_flags);
}

void setifpromisc(char *ifname, int enable)
{
	setifflags(ifname, enable ? IFF_PROMISC : -IFF_PROMISC);
}

int getifpromisc(char *ifname)
{
	int flags = getifflags(ifname);
	return ((flags & IFF_PROMISC) ? 1 : 0);
}

void setifup(char *ifname, int enable)
{
	setifflags(ifname, enable ? IFF_UP : -IFF_UP);
}

int getifup(char *ifname)
{
	int flags = getifflags(ifname);
	return ((flags & IFF_UP) ? 1 : 0);
}

void 
setifmtu(char *ifname, int mtu)
{
	struct ifreq ifr;
	int fd = socket_open();

	strncpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
	ifr.ifr_mtu = mtu;
	if (ioctl(fd, SIOCSIFMTU, (caddr_t)&ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
}

int
getifmtu(char *ifname)
{
	struct ifreq ifr;
	int fd = socket_open();

	strncpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
	if (ioctl(fd, SIOCGIFMTU, (caddr_t)&ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
	return (ifr.ifr_mtu);
}

void 
getifnetmask(char *ifname, char *mask, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;
	int fd = socket_open();

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	sin = (struct sockaddr_in *)&ifr.ifr_netmask;
	strncpy(mask, inet_ntoa(sin->sin_addr), size);
	close(fd);
}

void
setifaddr(char *ifname, char *addr, char *mask)
{
	struct ifreq ifr;
	struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr.ifr_addr);
	int fd = socket_open();

	sin->sin_family = AF_INET;
	if (!inet_aton(addr, &sin->sin_addr)) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}

	if (!inet_aton(mask, &sin->sin_addr)) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}

	if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
}

void 
getifaddr(char *ifname, char *mask, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;
	int fd = socket_open();

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strncpy(mask, inet_ntoa(sin->sin_addr), size);
	close(fd);
}

void
getifbroadaddr(char *ifname, char *mask, size_t size)
{
	struct ifreq ifr;
	struct sockaddr_in *sin;
	int fd = socket_open();

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strncpy(mask, inet_ntoa(sin->sin_addr), size);
	close(fd);
}

int ifexist(char *ifname)
{
	struct ifreq ifr;
	int fd = socket_open();
	int ret;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	ret = ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	return (ret == 0 ? 1 : 0);
}

void addifarp(char *ifname, char *host, char *addr)
{
	struct arpreq req;
	struct sockaddr_in *nhost = (struct sockaddr_in *)(&req.arp_pa);
	struct ether_addr *naddr = (struct ether_addr *)(req.arp_ha.sa_data);
	int fd = socket_open();

	memset(&req, 0, sizeof(req));
	if (!inet_aton(host, &nhost->sin_addr)) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	req.arp_pa.sa_family = AF_INET;
	if (!ether_aton_r(addr, naddr)) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	req.arp_ha.sa_family = ARPHRD_ETHER;
	strncpy(req.arp_dev, ifname, sizeof(req.arp_dev));
	req.arp_flags = ATF_PERM | ATF_COM;
	if (ioctl(fd, SIOCSARP, &req) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
}

void delifarp(char *ifname, char *host)
{
	struct arpreq req;
	struct sockaddr_in *nhost = (struct sockaddr_in *)(&req.arp_pa);
	int fd = socket_open();

	memset(&req, 0, sizeof(req));
	if (!inet_aton(host, &nhost->sin_addr)) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	req.arp_pa.sa_family = AF_INET;
	strncpy(req.arp_dev, ifname, sizeof(req.arp_dev));
	req.arp_flags = ATF_PERM | ATF_COM;
	if (ioctl(fd, SIOCDARP, &req) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
}

void getifarp(char *ifname, char *host, char *addr, size_t size)
{
	struct arpreq req;
	struct sockaddr_in *nhost = (struct sockaddr_in *)(&req.arp_pa);
	struct ether_addr *naddr = (struct ether_addr *)(req.arp_ha.sa_data);
	int fd = socket_open();

	memset(&req, 0, sizeof(req));
	if (!inet_aton(host, &nhost->sin_addr)) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	req.arp_pa.sa_family = AF_INET;
	strncpy(req.arp_dev, ifname, sizeof(req.arp_dev));
	req.arp_flags = ATF_PERM | ATF_COM;
	if (ioctl(fd, SIOCGARP, &req) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	__ether_ntoa(naddr, addr, size);
	close(fd);
}

void
setifhwaddr(char *ifname, char *addr)
{
	struct ifreq ifr;
	struct ether_addr *naddr;
	int fd = socket_open();

	naddr = (struct ether_addr *)(ifr.ifr_hwaddr.sa_data);
	if (!ether_aton_r(addr, naddr)) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = ARPHRD_ETHER;
	if (ioctl(fd, SIOCSIFHWADDR, &ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	close(fd);
}

void
getifhwaddr(char *ifname, char *addr, size_t size)
{
	struct ifreq ifr;
	struct ether_addr *naddr;
	int fd = socket_open();

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
		close(fd);
		rb_raise(rb_eException, "%s", strerror(errno));
	}
	naddr = (struct ether_addr *)(ifr.ifr_hwaddr.sa_data);
	__ether_ntoa(naddr, addr, size);
	close(fd);
}
#endif
