#!/bin/sh

file=$1;

cat $file | grep "6379" | wc -l
cat $file | grep "6380" | wc -l
cat $file | grep "6381" | wc -l
cat $file | grep "6382" | wc -l

