#!/bin/bash

for i in `seq 1 100`;
do
	./AdRtbInfo -i 127.0.0.1 &
done
