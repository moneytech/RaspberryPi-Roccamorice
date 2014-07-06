#!/bin/bash

### BEGIN INIT INFO
# Provides:          god
# Required-Start:    $local_fs $syslog
# Required-Stop:     $local_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start god at boot time
# Description:       Start god at boot time
### END INIT INFO

# Starts god
#/home/pi/.rvm/gems/ruby-2.0.0-p247/bin/god -c /home/pi/src/god.programs


#! /bin/sh
# /etc/init.d/blah
#

# Some things that run always

/home/pi/.rvm/gems/ruby-2.0.0-p247/bin/god -c /home/pi/src/god.programs &

# Carry out specific functions when asked to by the system
case "$1" in
  start)
    #echo "Starting script blah "
    #echo "Could do more here"
	/home/pi/.rvm/gems/ruby-2.0.0-p247/bin/god -c /home/pi/src/god.programs
    ;;
  stop)
	echo "stopping all god monitoring but god will continue to run"
	/home/pi/.rvm/gems/ruby-2.0.0-p247/bin/god stop
    ;;
  *)
    echo "Usage: /etc/init.d/blah {start|stop}"
    exit 1
    ;;
esac

exit 0

