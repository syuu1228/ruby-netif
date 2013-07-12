#include "netif.h"
#include <stdio.h>
#include <errno.h>
#include <net/if.h>
#include <ruby.h>

VALUE rb_cNetif;

int socket_open(void)
{
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		rb_raise(rb_eException, "socket open failed");
	return fd;
}

VALUE netif_initialize(VALUE self, VALUE ifname)
{
	int fd = socket_open();
	rb_iv_set(self, "@ifname", ifname);
	rb_iv_set(self, "@fd", INT2FIX(fd));
	return self;	
}

VALUE netif_close(VALUE self)
{
	close(FIX2INT(rb_iv_get(self, "@fd")));
	return Qnil;
}

VALUE netif_exists(VALUE self, VALUE ifname)
{
	int fd = socket_open();
	int exist = ifexist(fd, StringValuePtr(ifname));
	close(fd);
	return INT2FIX(exist);
}

VALUE netif_setflags(VALUE self, int value)
{
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	if ((setifflags(FIX2INT(fd), StringValuePtr(ifname), value)))
		rb_raise(rb_eException, "%s", strerror(errno));
	return Qnil;
}

VALUE netif_testflags(VALUE self, int value)
{
	int flags;
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	if ((getifflags(FIX2INT(fd), StringValuePtr(ifname), &flags)))
		rb_raise(rb_eException, "%s", strerror(errno));
	return INT2FIX((flags & value));
}

VALUE netif_enable_promisc(VALUE self)
{
	netif_setflags(self, IFF_PROMISC);
	return Qnil;
}

VALUE netif_disable_promisc(VALUE self)
{
	netif_setflags(self, ~IFF_PROMISC);
	return Qnil;
}

VALUE netif_test_promisc(VALUE self)
{
	return netif_testflags(self, IFF_PROMISC);
}

VALUE netif_up(VALUE self)
{
//	netif_setflags(self, (IFF_UP | IFF_RUNNING));
	netif_setflags(self, IFF_UP);
	return Qnil;
}

VALUE netif_down(VALUE self)
{
	netif_setflags(self, ~IFF_UP);
	return Qnil;
}

VALUE netif_test_up(VALUE self)
{
	return netif_testflags(self, IFF_UP);
}

VALUE netif_set_mtu(VALUE self, VALUE mtu)
{
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	if ((setifmtu(FIX2INT(fd), StringValuePtr(ifname), FIX2INT(mtu))))
		rb_raise(rb_eException, "%s", strerror(errno));
	return Qnil;
}

VALUE netif_get_mtu(VALUE self)
{
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	int mtu;
	if ((getifmtu(FIX2INT(fd), StringValuePtr(ifname), &mtu)))
		rb_raise(rb_eException, "%s", strerror(errno));
	return INT2FIX(mtu);
}

VALUE netif_get_netmask(VALUE self)
{
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[16];

	if ((getifnetmask(FIX2INT(fd), StringValuePtr(ifname), buf, 
	    	sizeof(buf))))
		rb_raise(rb_eException, "%s", strerror(errno));
	return rb_str_new2(buf);
}

VALUE netif_get_addr(VALUE self)
{
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[16];

	if ((getifaddr(FIX2INT(fd), StringValuePtr(ifname), buf, 
	    	sizeof(buf))))
		rb_raise(rb_eException, "%s", strerror(errno));
	return rb_str_new2(buf);
}

VALUE netif_get_broadaddr(VALUE self)
{
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[16];

	if ((getifbroadaddr(FIX2INT(fd), StringValuePtr(ifname), buf, 
	    	sizeof(buf))))
		rb_raise(rb_eException, "%s", strerror(errno));
	return rb_str_new2(buf);
}

VALUE netif_set_addr(VALUE self, VALUE addr, VALUE mask)
{
	VALUE fd = rb_iv_get(self, "@fd");
	VALUE ifname = rb_iv_get(self, "@ifname");
	if ((setifaddr(FIX2INT(fd), StringValuePtr(ifname), 
		StringValuePtr(addr), StringValuePtr(mask))))
		rb_raise(rb_eException, "%s", strerror(errno));
	return Qnil;
}

void Init_netif(void)
{
	rb_cNetif = rb_define_class("Netif", rb_cObject);
	rb_define_method(rb_cNetif, "initialize", netif_initialize, 1);
	rb_define_method(rb_cNetif, "close", netif_close, 0);
	rb_define_singleton_method(rb_cNetif, "exists?", netif_exists, 1);
	rb_define_method(rb_cNetif, "enable_promisc", netif_enable_promisc, 0);
	rb_define_method(rb_cNetif, "disable_promisc", netif_disable_promisc, 0);
	rb_define_method(rb_cNetif, "promisc?", netif_test_promisc, 0);
	rb_define_method(rb_cNetif, "up", netif_up, 0);
	rb_define_method(rb_cNetif, "down", netif_down, 0);
	rb_define_method(rb_cNetif, "up?", netif_test_up, 0);
	rb_define_method(rb_cNetif, "mtu=", netif_set_mtu, 1);
	rb_define_method(rb_cNetif, "mtu", netif_get_mtu, 0);
	rb_define_method(rb_cNetif, "netmask", netif_get_netmask, 0);
	rb_define_method(rb_cNetif, "addr", netif_get_addr, 0);
	rb_define_method(rb_cNetif, "broadaddr", netif_get_broadaddr, 0);
	rb_define_method(rb_cNetif, "set_addr", netif_set_addr, 2);
}
