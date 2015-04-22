#!/bin/sh

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/bin/cgminer
if [ -x /config/cgminer ]; then
 DAEMON=/config/cgminer
fi
NAME=cgminer
DESC="Cgminer daemon"
CONFIG_NAME="/config/asic-freq.config"
set -e
#set -x
test -x "$DAEMON" || exit 0

do_start() {
	# gpio1_16 = 48 = net check LED
	if [ ! -e /sys/class/gpio/gpio48 ]; then
		echo 48 > /sys/class/gpio/export
	fi
	echo low > /sys/class/gpio/gpio48/direction

	gateway=$(route -n | grep 'UG[ \t]' | awk '{print $2}')
	if ping -w 1 -c 1 $gateway | grep "100%" >/dev/null    
	then                                                   
		prs=1                                                
		echo "$gateway is Not reachable"                             
	else                                               
	    prs=0
		echo "$gateway is reachable" 	
	fi                    
	if [ $prs = "0" ]; then
		echo heartbeat > /sys/class/leds/beaglebone:green:usr3/trigger
		echo 1 > /sys/class/gpio/gpio48/value	
	else
		echo mmc1 > /sys/class/leds/beaglebone:green:usr3/trigger
	fi	
	if [ -z  "`lsmod | grep bitmain_spi`"  ]; then
		echo "No bitmain-asic"
		insmod /lib/modules/`uname -r`/kernel/drivers/bitmain/bitmain_spi.ko
	else
		echo "Have bitmain-asic"
		rmmod bitmain_spi.ko
		sleep 1
		insmod /lib/modules/`uname -r`/kernel/drivers/bitmain/bitmain_spi.ko fpga_ret_prnt=0 rx_st_prnt=0
	fi
	#control console printk level
	real_freq=0782
	chip_value=200
	timeout=40

	PARAMS="--bitmain-dev /dev/bitmain-asic --bitmain-options 115200:32:8:$timeout:$chip_value:$real_freq --bitmain-checkn2diff --bitmain-hwerror --queue 320"
	echo PARAMS = $PARAMS
	start-stop-daemon -b -S -x screen -- -S cgminer -t cgminer -m -d "$DAEMON" $PARAMS --api-listen --default-config /config/cgminer.conf
}

do_stop() {
        killall -9 cgminer || true
}
case "$1" in
  start)
        echo -n "Starting $DESC: "
	do_start
        echo "$NAME."
        ;;
  stop)
        echo -n "Stopping $DESC: "
	do_stop
        echo "$NAME."
        ;;
  restart|force-reload)
        echo -n "Restarting $DESC: "
        do_stop
        do_start
        echo "$NAME."
        ;;
  *)
        N=/etc/init.d/$NAME
        echo "Usage: $N {start|stop|restart|force-reload}" >&2
        exit 1
        ;;
esac

exit 0
