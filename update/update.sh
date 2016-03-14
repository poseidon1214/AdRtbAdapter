#!/usr/bin/expect  -f

set filename "ibid_ip.txt"
set file [ open $filename r]

while { [ gets $file IP ] >= 0 } {
spawn ssh root@$IP
set timeout 5
expect "*(yes/no)?" {send "yes\r"}
set timeout 120
expect "*password*"
send "ge()_r()()t\r"
expect "*]*"
send "ls;exit\r"

spawn scp tanx_content.txt tanx_description.txt root@$IP:/opt/iBidding/config/
expect "*password*"
send "ge()_r()()t\r"

spawn scp nginx.conf root@$IP:/root/
expect "*password*"
send "ge()_r()()t\r"

spawn ssh root@$IP
expect "*password*"
send "ge()_r()()t\r"
expect "*]*"
send "cd /usr/local/nginx; mv conf/nginx.conf conf/nginx.conf-temp -f; mv /root/nginx.conf conf/;./sbin/nginx -s reload\r"
sleep 3
send "killall AdAdapterStart.sh; killall AdRtbAdapter; cd /opt; rm AdRtbAdapter* -rf;exit\r"

spawn bash -c "scp AdRtbAdapter*tar.gz root@$IP:/opt"
expect "*password*"
send "ge()_r()()t\r"

spawn ssh root@$IP
expect "*password*"
send "ge()_r()()t\r"
expect "*]*"
send "cd /opt;tar zxvf AdRtbAdapter*tar.gz; cd /opt/AdRtbAdapter\r"
expect "*]*"
#send "screen -R new1;cd /opt/AdRtbAdapter;./AdAdapterStart.sh 20007 8124 & \r"
send "nohup /opt/AdRtbAdapter/AdAdapterStart.sh 20007 8124 > /opt/20007.log 2>&1 &\r"
#send "\x01" 
#send "d"
expect "*]*"
#send "screen -R new2;cd /opt/AdRtbAdapter;./AdAdapterStart.sh 20008 8125 & \r"
send "nohup /opt/AdRtbAdapter/AdAdapterStart.sh 20008 8125 > /opt/20008.log 2>&1 &\r"
#sleep 2
#send "\x01" 
#send "d"
expect "*]*"
send "cd /usr/local/nginx; mv conf/nginx.conf-temp conf/nginx.conf -f;./sbin/nginx -s reload; exit\r"
expect eof
}
close $file
