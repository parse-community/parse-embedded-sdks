# Variable
YUN_IP_ADDRESS=arduino.local #YUN's IP,  you can use arduino.local optionally
YUN_PWD=arduino #YUN's PASSWORD, default is arduino

# scp parse_push and parse_request to Yun
expect -c "
spawn scp linux_package/parse-embedded_1.0.0-1_ar71xx.ipk root@$YUN_IP_ADDRESS:/root/
expect *password:
send $YUN_PWD\n
interact
"

expect -c "
spawn scp linux_package/parse-embedded-yun_1.0.0-1_ar71xx.ipk root@$YUN_IP_ADDRESS:/root/
expect *password:
send $YUN_PWD\n
interact
"

expect -c "
spawn ssh root@$YUN_IP_ADDRESS ''/bin/opkg install /root/parse-embedded_1.0.0-1_ar71xx.ipk''
expect *password:
send $YUN_PWD\n
interact
"

expect -c "
spawn ssh root@$YUN_IP_ADDRESS ''/bin/opkg install /root/parse-embedded-yun_1.0.0-1_ar71xx.ipk''
expect *password:
send $YUN_PWD\n
interact
"
