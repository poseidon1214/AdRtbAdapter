#!/bin/bash

while true
do
  echo "============================Adapter Start"
  /opt/AdRtbAdapter/bin/AdRtbAdapter -c /opt/AdRtbAdapter/conf/AdAdp.conf -t 8 -p $1 -d $2 
  date 
  echo "============================Adapter Stop"
done
