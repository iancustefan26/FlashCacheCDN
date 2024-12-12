#!/bin/bash

cfg_file="./../client/config/resolv.ini"

echo "IP address of the DNS server:"
read -r ip

if [[ ! $ip =~ ^([0-9]{1,3}\.){3}[0-9]{1,3}$ ]]; then
    echo "IP address not valid"
    exit 1
fi

if [[ -f $cfg_file ]]; then
    sed -i "s/^nameserver = [0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}$/nameserver = $ip/" "$cfg_file"
    echo "Config file updated successfully"
else
    echo "Config file not found"
    exit 1
fi
