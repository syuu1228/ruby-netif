#ifndef _NETIF_H
#define _NETIF_H

#include <sys/types.h>

int setifflags(int fd, char *ifname, int value);
int getifflags(int fd, char *ifname, int *flags);
int setifmtu(int fd, char *ifname, int mtu);
int getifmtu(int fd, char *ifname, int *mtu);
int getifnetmask(int fd, char *ifname, char *mask, size_t size);
int getifaddr(int fd, char *ifname, char *mask, size_t size);
int getifbroadaddr(int fd, char *ifname, char *mask, size_t size);
int setifaddr(int fd, char *ifname, char *addr, char *mask);
int ifexist(int fd, char *ifname);
#endif
