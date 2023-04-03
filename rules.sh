# Flush rules and delete custom chains
iptables -F
iptables -X

# Define chain to allow particular source addresses
iptables -N chain-incoming-ssh
iptables -A chain-incoming-ssh -s 192.168.1.148 -j ACCEPT
iptables -A chain-incoming-ssh -s 192.168.1.149 -j ACCEPT
iptables -A chain-incoming-ssh -j DROP

# Define chain to allow particular services
iptables -N chain-outgoing-services
iptables -A chain-outgoing-services -p tcp --dport 53  -j ACCEPT
iptables -A chain-outgoing-services -p udp --dport 53  -j ACCEPT
iptables -A chain-outgoing-services -p tcp --dport 123 -j ACCEPT
iptables -A chain-outgoing-services -p udp --dport 123 -j ACCEPT
iptables -A chain-outgoing-services -p tcp --dport 80  -j ACCEPT
iptables -A chain-outgoing-services -p tcp --dport 443 -j ACCEPT
iptables -A chain-outgoing-services -p tcp --dport 22  -j ACCEPT
iptables -A chain-outgoing-services -p icmp            -j ACCEPT
iptables -A chain-outgoing-services -j DROP

# Define chain to allow established connections
iptables -N chain-states
iptables -A chain-states -p tcp  -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
iptables -A chain-states -p udp  -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
iptables -A chain-states -p icmp -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
iptables -A chain-states -j RETURN

# Drop invalid packets
iptables -A INPUT -m conntrack --ctstate INVALID -j DROP

# Accept everything on loopback
iptables -A INPUT  -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT

# Accept incoming/outgoing packets for established connections
iptables -A INPUT  -j chain-states
iptables -A OUTPUT -j chain-states

# Accept incoming ICMP
iptables -A INPUT -p icmp -j ACCEPT

# Accept incoming SSH
iptables -A INPUT -p tcp --dport 22 -j chain-incoming-ssh

# Accept outgoing 
iptables -A OUTPUT -j chain-outgoing-services

## Drop everything else
iptables -P INPUT   DROP
iptables -P FORWARD DROP
iptables -P OUTPUT  DROP
