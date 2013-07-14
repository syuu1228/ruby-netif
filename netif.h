#ifndef _NETIF_H
#define _NETIF_H

#include <sys/types.h>

void setifpromisc(char *ifname, int enable);
int getifpromisc(char *ifname);
void setifup(char *ifname, int enable);
int getifup(char *ifname);
void setifmtu(char *ifname, int mtu);
int getifmtu(char *ifname);
void getifnetmask(char *ifname, char *mask, size_t size);
void getifaddr(char *ifname, char *mask, size_t size);
void getifbroadaddr(char *ifname, char *mask, size_t size);
void setifaddr(char *ifname, char *addr, char *mask);
int ifexist(char *ifname);
void addifarp(char *ifname, char *host, char *addr);
void delifarp(char *ifname, char *host);
void getifarp(char *ifname, char *host, char *addr, size_t size);
void setifhwaddr(char *ifname, char *addr);
void getifhwaddr(char *ifname, char *addr, size_t size);
void setifdefaultgw(char *ifname, char *addr);
void delifdefaultgw(char *ifname, char *addr);
int getifdefaultgw(char *ifname, char *addr, size_t size);
#endif
