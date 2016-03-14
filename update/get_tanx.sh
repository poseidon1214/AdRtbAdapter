#!/bin/bash

time=`date +%s`
token=a177bc5b45c461d506feb8b836c98107
id=43124750
str=`echo -n "$token$time" | md5sum | cut -d' ' -f1`
echo $str

wget -w 2 "http://www.tanx.com/pubfilter/pubcontrolContent/download?dspId=$id&token=$str&signTime=$time" 
wget -w 2 "http://www.tanx.com/pubfilter/pubcontrolDescription/download?dspId=$id&token=$str&signTime=$time"
rm adx_content.txt -f
rm adx_discription.txt -f
mv download\?dspId\=$id\&token\=$str\&signTime\=$time tanx_content.txt 
mv download\?dspId\=$id\&token\=$str\&signTime\=$time.1 tanx_description.txt 
