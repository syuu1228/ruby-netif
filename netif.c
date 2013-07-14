#include "netif.h"
#include <stdio.h>
#include <unistd.h>
#include <net/if.h>
#include <ruby.h>

VALUE rb_cNetif;

VALUE netif_initialize(VALUE self, VALUE ifname)
{
	rb_iv_set(self, "@ifname", ifname);
	return self;	
}

VALUE netif_exists(VALUE self, VALUE ifname)
{
	int exist = ifexist(StringValuePtr(ifname));
	return INT2FIX(exist);
}

VALUE netif_enable_promisc(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	setifpromisc(StringValuePtr(ifname), 1);
	return Qnil;
}

VALUE netif_disable_promisc(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	setifpromisc(StringValuePtr(ifname), 0);
	return Qnil;
}

VALUE netif_test_promisc(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	int enable = getifpromisc(StringValuePtr(ifname));
	return INT2FIX(enable);
}

VALUE netif_up(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	setifup(StringValuePtr(ifname), 1);
	return Qnil;
}

VALUE netif_down(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	setifup(StringValuePtr(ifname), 0);
	return Qnil;
}

VALUE netif_test_up(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	int enable = getifup(StringValuePtr(ifname));
	return INT2FIX(enable);
}

VALUE netif_set_mtu(VALUE self, VALUE mtu)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	setifmtu(StringValuePtr(ifname), FIX2INT(mtu));
	return Qnil;
}

VALUE netif_get_mtu(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	int mtu = getifmtu(StringValuePtr(ifname));
	return INT2FIX(mtu);
}

VALUE netif_get_netmask(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[128];

	getifnetmask(StringValuePtr(ifname), buf, sizeof(buf));
	return rb_str_new2(buf);
}

VALUE netif_get_addr(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[128];

	getifaddr(StringValuePtr(ifname), buf, sizeof(buf));
	return rb_str_new2(buf);
}

VALUE netif_get_broadaddr(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[128];

	getifbroadaddr(StringValuePtr(ifname), buf, sizeof(buf));
	return rb_str_new2(buf);
}

VALUE netif_set_addr(VALUE self, VALUE addr, VALUE mask)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	setifaddr(StringValuePtr(ifname), StringValuePtr(addr),
		StringValuePtr(mask));
	return Qnil;
}

VALUE netif_add_arp(VALUE self, VALUE host, VALUE addr)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	addifarp(StringValuePtr(ifname), StringValuePtr(host), 
		StringValuePtr(addr));
	return Qnil;
}

VALUE netif_del_arp(VALUE self, VALUE host)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	delifarp(StringValuePtr(ifname), StringValuePtr(host));
	return Qnil;
}

VALUE netif_get_arp(VALUE self, VALUE host)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[128];

	getifarp(StringValuePtr(ifname), StringValuePtr(host), buf, 
		sizeof(buf));
	return rb_str_new2(buf);
}

VALUE netif_set_hwaddr(VALUE self, VALUE addr)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	setifhwaddr(StringValuePtr(ifname), StringValuePtr(addr));
	return Qnil;
}

VALUE netif_get_hwaddr(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[128];

	getifhwaddr(StringValuePtr(ifname), buf, sizeof(buf));
	return rb_str_new2(buf);
}

VALUE netif_set_defaultgw(VALUE self, VALUE addr)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[128];

	if (!getifdefaultgw(StringValuePtr(ifname), buf, sizeof(buf)))
		delifdefaultgw(StringValuePtr(ifname), buf);
	if (addr != Qnil)
		setifdefaultgw(StringValuePtr(ifname), StringValuePtr(addr));
	return Qnil;
}

VALUE netif_get_defaultgw(VALUE self)
{
	VALUE ifname = rb_iv_get(self, "@ifname");
	char buf[128];

	if (!getifdefaultgw(StringValuePtr(ifname), buf, sizeof(buf)))
		return rb_str_new2(buf);
	else
		return Qnil;
}

void Init_netif(void)
{
	rb_cNetif = rb_define_class("Netif", rb_cObject);
	rb_define_method(rb_cNetif, "initialize", netif_initialize, 1);
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
	rb_define_method(rb_cNetif, "add_arp", netif_add_arp, 2);
	rb_define_method(rb_cNetif, "del_arp", netif_del_arp, 1);
	rb_define_method(rb_cNetif, "get_arp", netif_get_arp, 1);
	rb_define_method(rb_cNetif, "hwaddr=", netif_set_hwaddr, 1);
	rb_define_method(rb_cNetif, "hwaddr", netif_get_hwaddr, 0);
	rb_define_method(rb_cNetif, "defaultgw=", netif_set_defaultgw, 1);
	rb_define_method(rb_cNetif, "defaultgw", netif_get_defaultgw, 0);
}
