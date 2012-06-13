#!/bin/sh
cd $(dirname $0)

pidfile="server.pid"
logfile="server.$(date +%Y%M%d%H%m%s).log"
config="$XDG_CONFIG_HOME/group22/sdc.conf"
command="./build/src/server/server -f $config"

[ ! -z $2 ] && config=$2
case $1 in
	"start")
		./build/src/server/server &>>$logfile &
		echo $! > $pidfile;;
	"stop")
		[ -r $pidfile ] && [ ! -z $(cat $pidfile) ] && kill $(cat $pidfile)
		rm $pidfile;;
	"restart")
		$0 "stop"
		$0 "start";;
	"cleanlogs")
		rm server.*.log;;
	*)
	echo "Start/stop interface usage:"
	echo "$0 {start|stop|restart|cleanlogs}"
	./build/src/server/server $*;;
esac
