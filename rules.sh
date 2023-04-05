if [ $1 = "lock" ];
then
    # Flush rules and delete custom chains
    iptables -F
    iptables -X

    # Define white-list-chain
    iptables -N white-list
    iptables -I white-list 1 -d 111.65.250.2 -j ACCEPT
    
    # Define chain to lock
    iptables -N chain-lock
    iptables -A chain-lock -j white-list
    iptables -A chain-lock -j DROP

    # Add chain-lock to OUTPUT chain
    
    iptables -A OUTPUT -j chain-lock
elif [ $1 = "unlock" ];
then
    # Flush rules and delete custom chains
    iptables -F
    iptables -X
else
    echo "Invalid input"
fi
