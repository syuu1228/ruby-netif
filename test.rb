require 'netif'

ifname = "eth0"
begin
	puts "eth3 exists?:#{Netif.exists?("eth3")}"
	puts "#{ifname} exists?:#{Netif.exists?(ifname)}"
	eth = Netif.new(ifname)
	p eth
	puts `ifconfig #{ifname}`
	eth.enable_promisc
	puts "[enable_promisc]"
	puts `ifconfig #{ifname}`
	puts "promisc?:#{eth.promisc?}"
	raise "enable_promisc" unless eth.promisc? == 1
	eth.disable_promisc
	puts "[disable_promisc]"
	puts `ifconfig #{ifname}`
	puts "promisc?:#{eth.promisc?}"
	raise "disable_promisc" unless eth.promisc? == 0
	eth.down
	puts "[down]"
	puts `ifconfig #{ifname}`
	puts "up?:#{eth.up?}"
	raise "down" unless eth.up? == 0
	eth.up
	puts "[up]"
	puts `ifconfig #{ifname}`
	puts "up?:#{eth.up?}"
	raise "up" unless eth.up? == 1
	eth.mtu = 1000;
	puts "[mtu=1000]"
	puts `ifconfig #{ifname}`
	puts "mtu:#{eth.mtu}"
	raise "mtu" unless eth.mtu == 1000
	eth.mtu = 1500;
	puts "[mtu=1500]"
	puts `ifconfig #{ifname}`
	puts "mtu:#{eth.mtu}"
	raise "mtu" unless eth.mtu == 1500
	eth.set_addr("172.0.0.1", "255.255.255.0")
	puts "[set_addr]"
	puts `ifconfig #{ifname}`
	puts "addr:#{eth.addr}"
	puts "netmask:#{eth.netmask}"
	puts "broadaddr:#{eth.broadaddr}"
	raise "set_addr addr" unless eth.addr == "172.0.0.1"
	raise "set_addr netmask" unless eth.netmask == "255.255.255.0"
	raise "set_addr broadaddr" unless eth.broadaddr == "172.0.0.255"
	eth.set_addr("192.168.122.105", "255.255.255.0")
	puts "[set_addr]"
	puts `ifconfig #{ifname}`
	puts "addr:#{eth.addr}"
	puts "netmask:#{eth.netmask}"
	puts "broadaddr:#{eth.broadaddr}"
	eth.add_arp("192.168.122.200", "00:11:22:33:44:55")
	puts "[add_arp]"
	puts `arp -a`
	puts "arp(192.168.122.200):#{eth.get_arp("192.168.122.200")}"
	raise "add_arp" unless eth.get_arp("192.168.122.200") == "0:11:22:33:44:55"
	eth.del_arp("192.168.122.200")
	puts "[del_arp]"
	puts `arp -a`
	puts "hwaddr:#{eth.hwaddr}"
	eth.down
	eth.hwaddr = "00:11:22:33:44:55"
	eth.up
	puts "[hwaddr=00:11:22:33:44:55]"
	puts `ifconfig #{ifname}`
	puts "hwaddr:#{eth.hwaddr}"
	raise "hwaddr" unless eth.hwaddr == "0:11:22:33:44:55"
	eth.down
	eth.hwaddr = "52:54:00:77:2b:93"
	eth.up
	puts "[hwaddr=52:54:00:77:2b:93]"
	puts `ifconfig #{ifname}`
	puts "hwaddr:#{eth.hwaddr}"
	raise "hwaddr" unless eth.hwaddr == "52:54:0:77:2b:93"
	eth.close

rescue => e
	p $!
	p e
end
