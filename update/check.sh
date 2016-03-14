#!/bin/bash

while read ip
do
	echo "Device:"$ip
	../bin/AdRtbInfo -i $ip -p 8124 -t 4 | grep "Ad Rtb System Info"
	../bin/AdRtbInfo -i $ip -p 8125 -t 4 | grep "Ad Rtb System Info"
done < ibid_ip.txt
