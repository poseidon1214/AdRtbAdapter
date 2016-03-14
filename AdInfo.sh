#!/bin/bash

port1=8124
port2=8125

for ip in 184 185 186 163 164 165 166 167 168 80
do
	./bin/AdRtbInfo -i 192.168.167.$ip -p $port1
	./bin/AdRtbInfo -i 192.168.167.$ip -p $port2
done
