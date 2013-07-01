#ifdef __FreeBSD__

#include "netif.h"
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if_var.h>		/* for struct ifaddr */
#include <netinet/in_var.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/ethernet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <err.h>
#include <net/route.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/if_ether.h>
#include <sys/sysctl.h>
#include <stdlib.h>

static int
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

static int
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

int setifpromisc(int fd, char *ifname, int enable)
{
	return setifflags(fd, ifname, 
		enable ? IFF_PPROMISC : -IFF_PPROMISC);
}

int getifpromisc(int fd, char *ifname, int *enable)
{
	int flags, ret;

	if ((ret = getifflags(fd, ifname, &flags)))
		return (ret);
	*enable = (flags & IFF_PPROMISC) ? 1 : 0;
	return (0);
}

int setifup(int fd, char *ifname, int enable)
{
	return setifflags(fd, ifname, 
		enable ? IFF_UP: -IFF_UP);
}

int getifup(int fd, char *ifname, int *enable)
{
	int flags, ret;

	if ((ret = getifflags(fd, ifname, &flags)))
		return (ret);
	*enable = (flags & IFF_UP) ? 1 : 0;
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

/*
 * Given a hostname, fills up a (static) struct sockaddr_in with
 * the address of the host and returns a pointer to the
 * structure.
 */
static struct sockaddr_in *
arp_getaddr(char *host)
{
	struct hostent *hp;
	static struct sockaddr_in reply;

	bzero(&reply, sizeof(reply));
	reply.sin_len = sizeof(reply);
	reply.sin_family = AF_INET;
	reply.sin_addr.s_addr = inet_addr(host);
	if (reply.sin_addr.s_addr == INADDR_NONE) {
		if (!(hp = gethostbyname(host))) {
			warnx("%s: %s", host, hstrerror(h_errno));
			return (NULL);
		}
		bcopy((char *)hp->h_addr, (char *)&reply.sin_addr,
			sizeof reply.sin_addr);
	}
	return (&reply);
}

/*
 * Returns true if the type is a valid one for ARP.
 */
static int
arp_valid_type(int type)
{

	switch (type) {
	case IFT_ETHER:
	case IFT_FDDI:
	case IFT_ISO88023:
	case IFT_ISO88024:
	case IFT_ISO88025:
	case IFT_L2VLAN:
	case IFT_BRIDGE:
		return (1);
	default:
		return (0);
	}
}

static struct rt_msghdr *
arp_rtmsg(int cmd, struct sockaddr_in *dst, struct sockaddr_dl *sdl, 
    int flags)
{
	static int seq;
	int rlen;
	int l;
	struct sockaddr_in so_mask, *som = &so_mask;
	static int s = -1;
	static pid_t pid;

	static struct	{
		struct	rt_msghdr m_rtm;
		char	m_space[512];
	}	m_rtmsg;

	struct rt_msghdr *rtm = &m_rtmsg.m_rtm;
	char *cp = m_rtmsg.m_space;

	if (s < 0) {	/* first time: open socket, get pid */
		s = socket(PF_ROUTE, SOCK_RAW, 0);
		if (s < 0)
			err(1, "socket");
		pid = getpid();
	}
	bzero(&so_mask, sizeof(so_mask));
	so_mask.sin_len = 8;
	so_mask.sin_addr.s_addr = 0xffffffff;

	errno = 0;
	/*
	 * XXX RTM_DELETE relies on a previous RTM_GET to fill the buffer
	 * appropriately.
	 */
	if (cmd == RTM_DELETE)
		goto doit;
	bzero((char *)&m_rtmsg, sizeof(m_rtmsg));
	rtm->rtm_flags = flags;
	rtm->rtm_version = RTM_VERSION;

	switch (cmd) {
	default:
		errx(1, "internal wrong cmd");
	case RTM_ADD:
		rtm->rtm_addrs |= RTA_GATEWAY;
		rtm->rtm_rmx.rmx_expire = 0;
		rtm->rtm_inits = RTV_EXPIRE;
		rtm->rtm_flags |= (RTF_HOST | RTF_STATIC | RTF_LLDATA);
		/* FALLTHROUGH */
	case RTM_GET:
		rtm->rtm_addrs |= RTA_DST;
	}
#define NEXTADDR(w, s)					   \
	do {						   \
		if ((s) != NULL && rtm->rtm_addrs & (w)) { \
			bcopy((s), cp, sizeof(*(s)));	   \
			cp += SA_SIZE(s);		   \
		}					   \
	} while (0)

	NEXTADDR(RTA_DST, dst);
	NEXTADDR(RTA_GATEWAY, sdl);
	NEXTADDR(RTA_NETMASK, som);

	rtm->rtm_msglen = cp - (char *)&m_rtmsg;
doit:
	l = rtm->rtm_msglen;
	rtm->rtm_seq = ++seq;
	rtm->rtm_type = cmd;
	if ((rlen = write(s, (char *)&m_rtmsg, l)) < 0) {
		if (errno != ESRCH || cmd != RTM_DELETE) {
			warn("writing to routing socket");
			return (NULL);
		}
	}
	do {
		l = read(s, (char *)&m_rtmsg, sizeof(m_rtmsg));
	} while (l > 0 && (rtm->rtm_seq != seq || rtm->rtm_pid != pid));
	if (l < 0)
		warn("read from routing socket");
	return (rtm);
}

int addifarp(int fd, char *ifname, char *host, char *eaddr)
{
	struct sockaddr_in *addr;
	struct sockaddr_in *dst;	/* what are we looking for */
	struct sockaddr_dl *sdl;
	struct rt_msghdr *rtm;
	struct ether_addr *ea;
	struct sockaddr_dl sdl_m;
	int flags;

	bzero(&sdl_m, sizeof(sdl_m));
	sdl_m.sdl_len = sizeof(sdl_m);
	sdl_m.sdl_family = AF_LINK;

	dst = arp_getaddr(host);
	if (dst == NULL)
		return (1);
	flags = 0;
	ea = (struct ether_addr *)LLADDR(&sdl_m);
	{
		struct ether_addr *ea1 = ether_aton(eaddr);

		if (ea1 == NULL) {
			warnx("invalid Ethernet address '%s'", eaddr);
			return (1);
		} else {
			*ea = *ea1;
			sdl_m.sdl_alen = ETHER_ADDR_LEN;
		}
	}

	/*
	 * In the case a proxy-arp entry is being added for
	 * a remote end point, the RTF_ANNOUNCE flag in the 
	 * RTM_GET command is an indication to the kernel
	 * routing code that the interface associated with
	 * the prefix route covering the local end of the
	 * PPP link should be returned, on which ARP applies.
	 */
	rtm = arp_rtmsg(RTM_GET, dst, &sdl_m, flags);
	if (rtm == NULL) {
		warn("%s", host);
		return (1);
	}
	addr = (struct sockaddr_in *)(rtm + 1);
	sdl = (struct sockaddr_dl *)(SA_SIZE(addr) + (char *)addr);

	if ((sdl->sdl_family != AF_LINK) ||
	    (rtm->rtm_flags & RTF_GATEWAY) ||
	    !arp_valid_type(sdl->sdl_type)) {
		printf("cannot intuit interface index and type for %s\n", host);
		return (1);
	}
	sdl_m.sdl_type = sdl->sdl_type;
	sdl_m.sdl_index = sdl->sdl_index;
	return (arp_rtmsg(RTM_ADD, dst, &sdl_m, flags) == NULL);
}

int delifarp(int fd, char *ifname, char *host)
{
	struct sockaddr_in *addr, *dst;
	struct rt_msghdr *rtm;
	struct sockaddr_dl *sdl;
	struct sockaddr_dl sdl_m;
	int flags = 0;

	dst = arp_getaddr(host);
	if (dst == NULL)
		return (1);

	/*
	 * Perform a regular entry delete first.
	 */
	flags &= ~RTF_ANNOUNCE;

	/*
	 * setup the data structure to notify the kernel
	 * it is the ARP entry the RTM_GET is interested
	 * in
	 */
	bzero(&sdl_m, sizeof(sdl_m));
	sdl_m.sdl_len = sizeof(sdl_m);
	sdl_m.sdl_family = AF_LINK;

	for (;;) {	/* try twice */
		rtm = arp_rtmsg(RTM_GET, dst, &sdl_m, flags);
		if (rtm == NULL) {
			warn("%s", host);
			return (1);
		}
		addr = (struct sockaddr_in *)(rtm + 1);
		sdl = (struct sockaddr_dl *)(SA_SIZE(addr) + (char *)addr);

		/*
		 * With the new L2/L3 restructure, the route 
		 * returned is a prefix route. The important
		 * piece of information from the previous
		 * RTM_GET is the interface index. In the
		 * case of ECMP, the kernel will traverse
		 * the route group for the given entry.
		 */
		if (sdl->sdl_family == AF_LINK &&
		    !(rtm->rtm_flags & RTF_GATEWAY) &&
		    arp_valid_type(sdl->sdl_type) ) {
			addr->sin_addr.s_addr = dst->sin_addr.s_addr;
			break;
		}

		/*
		 * Regualar entry delete failed, now check if there
		 * is a proxy-arp entry to remove.
		 */
		if (flags & RTF_ANNOUNCE) {
			fprintf(stderr, "delete: cannot locate %s\n",host);
			return (1);
		}

		flags |= RTF_ANNOUNCE;
	}
	rtm->rtm_flags |= RTF_LLDATA;
	if (arp_rtmsg(RTM_DELETE, dst, NULL, flags) != NULL) {
		printf("%s (%s) deleted\n", host, inet_ntoa(addr->sin_addr));
		return (0);
	}
	return (1);

}

int getifarp(int fd, char *ifname, char *host, char *saddr, size_t size)
{
	struct sockaddr_in *_addr;
	u_long addr;
	int mib[6];
	size_t needed;
	char *lim, *buf, *next;
	struct rt_msghdr *rtm;
	struct sockaddr_in *sin2;
	struct sockaddr_dl *sdl;
	char _ifname[IF_NAMESIZE];
	int st, found_entry = 0;
	int flags;

	_addr = arp_getaddr(host);
	if (_addr == NULL)
		return (1);
	addr = _addr->sin_addr.s_addr;

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_FLAGS;
#ifdef RTF_LLINFO
	mib[5] = RTF_LLINFO;
#else
	mib[5] = 0;
#endif	
	if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
		err(1, "route-sysctl-estimate");
	if (needed == 0)	/* empty table */
		return 0;
	buf = NULL;
	for (;;) {
		buf = reallocf(buf, needed);
		if (buf == NULL)
			errx(1, "could not reallocate memory");
		st = sysctl(mib, 6, buf, &needed, NULL, 0);
		if (st == 0 || errno != ENOMEM)
			break;
		needed += needed / 8;
	}
	if (st == -1)
		err(1, "actual retrieval of routing table");
	lim = buf + needed;
	for (next = buf; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		sin2 = (struct sockaddr_in *)(rtm + 1);
		sdl = (struct sockaddr_dl *)((char *)sin2 + SA_SIZE(sin2));
		if (if_indextoname(sdl->sdl_index, _ifname) &&
		    strcmp(_ifname, ifname))
			continue;
		if (addr) {
			if (addr != sin2->sin_addr.s_addr)
				continue;
			found_entry = 1;
		}
		if (sdl->sdl_alen) {
			strncpy(saddr, ether_ntoa((struct ether_addr *)LLADDR(sdl)), size);
			break;
		}
	}
	free(buf);

	return (found_entry ? 0 : -1);
}

int 
setifhwaddr(int fd, char *ifname, char *addr)
{
	struct ifreq ifr;
	struct sockaddr_dl sdl;
	char mac[19];
	char name[IFNAMSIZ];
	int s;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memset(mac, 0, sizeof(mac));
	mac[0] = ':';
	strncpy(mac + 1, addr, strlen(addr));
	sdl.sdl_len = sizeof(sdl);
	link_addr(mac, &sdl);

	bcopy(sdl.sdl_data, ifr.ifr_addr.sa_data, 6);
	ifr.ifr_addr.sa_len = 6;

	if (ioctl(fd, SIOCSIFLLADDR, &ifr) < 0)
		return (-1);
	return (0);
}

int 
getifhwaddr(int fd, char *ifname, char *addr, size_t size)
{
	struct ifaddrs *ifa_list, *ifa; 
	struct sockaddr_dl *dl; 
	char name[IFNAMSIZ];
	struct ether_addr *naddr;
	int found = 0;

	if (getifaddrs(&ifa_list) < 0) {
		return (-1);
	}
	for (ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next) { 
		dl = (struct sockaddr_dl*)ifa->ifa_addr; 
		if (dl->sdl_family == AF_LINK && dl->sdl_type == IFT_ETHER) {
			memcpy(name, dl->sdl_data, dl->sdl_nlen);
			name[dl->sdl_nlen] = '\0';
			if (!strcmp(name, ifname)) {
				naddr = (struct ether_addr *)LLADDR(dl);
				strncpy(addr, ether_ntoa(naddr), size);
				found = 1;
				break;
			}
		}
	} 
	freeifaddrs(ifa_list); 
	if (!found)
		return (-1);
	return (0);
}
#endif
