#!/bin/sh

define_chain ()
{
    # Flush rules and delete custom chains
    iptables -F
    iptables -X
    # Define custom chain and add to OUTPUT chain
    iptables -N white-list
    iptables -N chain-lock
    iptables -A chain-lock -j white-list
    iptables -A OUTPUT -j chain-lock
}

if [ $1 = "lock" ];
then
    echo "LOCK MODE"
    define_chain

    # Define white-list-chain
    iptables -I white-list 1 -d 172.217.164.10 -j ACCEPT
    # iptables -I white-list 1 -d 111.65.250.2 -j ACCEPT

    iptables -A white-list -p udp --dport 53 -j ACCEPT
    iptables -A white-list -p udp --sport 53 -j ACCEPT
    
    # Define chain to lock
    iptables -A chain-lock -j DROP

elif [ $1 = "unlock" ];
then
    echo "UNLOCK MODE"
    define_chain
else
    echo "Invalid input"
fi

