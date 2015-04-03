echo "************************************"
echo "____Lab411-phutvbk@gmail.com________"
echo "Install batman-adv and configure network interface"
echo "__________________________________________________"
#remove batman & del wlan0
sudo rmmod batman-adv
sudo batctl if del wlan0
sleep 2

echo "Insmod batman-adv"
sudo insmod ./batman-adv.ko
#stop Network manager
sudo stop network-manager
#disable firewall
sudo iptables -F
echo "Configure wlanx interface"
sudo ifconfig wlan0 down
sudo iwconfig wlan0 mode ad-hoc	
sudo iwconfig wlan0 essid NC
sudo iwconfig wlan0 key 1234567890
sudo ifconfig wlan0 10.42.43.22/8
sleep 1
sudo iwconfig wlan0 ap 02:CE:FA:DD:CA:00 
sudo iwconfig wlan0 channel 1
sudo ifconfig wlan0 up
sudo ifconfig wlan0 mtu 1532
sleep 1

echo "Configure bat0 interface"
sudo ifconfig wlan0 promisc
sudo modprobe libcrc32can
sudo batctl if add wlan0

sudo ifconfig bat0 up
sudo ifconfig wlan0 up

sudo ifconfig wlan0 0.0.0.0
sudo ifconfig bat0 192.168.123.2/24
echo "Configure is successful"
