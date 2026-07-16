# Detectar cualquier paquete que NO salga con 64 (IPv4 e IPv6)
WAN_IP=$(ip -4 addr show dev pppoe-wan | grep inet | awk '{print $2}' | cut -d/ -f1)
WAN_IP6=$(ip -6 addr show dev pppoe-wan | grep global | awk '{print $2}' | cut -d/ -f1)

# Lanza estos dos (puedes abrirlos en ventanas separadas)
tcpdump -v -i pppoe-wan -nn "ip src $WAN_IP and ip[8] != 64"
tcpdump -v -i pppoe-wan -nn "ip6 src $WAN_IP6 and ip6[7] != 64"



tcpdump -v -i pppoe-wan -nn "ip src $WAN_IP and ip[4:2] != 0"



tcpdump -i pppoe-wan -nn -v "tcp[tcpflags] & (tcp-syn) != 0 and src host $WAN_IP" | grep "mss"



traceroute google.com
