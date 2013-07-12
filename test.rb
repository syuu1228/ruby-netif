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
	eth.disable_promisc
	puts "[disable_promisc]"
	puts `ifconfig #{ifname}`
	puts "promisc?:#{eth.promisc?}"
	eth.down
	puts "[down]"
	puts `ifconfig #{ifname}`
	puts "up?:#{eth.up?}"
	eth.up
	puts "[up]"
	puts `ifconfig #{ifname}`
	puts "up?:#{eth.up?}"
	eth.mtu = 1000;
	puts "[mtu=1000]"
	puts `ifconfig #{ifname}`
	puts "mtu:#{eth.mtu}"
	eth.mtu = 1500;
	puts "[mtu=1500]"
	puts `ifconfig #{ifname}`
	puts "mtu:#{eth.mtu}"
	eth.set_addr("172.0.0.1", "255.255.255.0")
	puts "[set_addr]"
	puts `ifconfig #{ifname}`
	puts "addr:#{eth.addr}"
	puts "netmask:#{eth.netmask}"
	puts "broadaddr:#{eth.broadaddr}"
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
	eth.del_arp("192.168.122.200")
	puts "[del_arp]"
	puts `arp -a`
	eth.close

rescue => e
	p $!
	p e
end
