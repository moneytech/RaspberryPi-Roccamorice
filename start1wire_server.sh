#!/bin/bash

### BEGIN INIT INFO
# Provides:          start1wire_server
# Required-Start:    $local_fs $syslog
# Required-Stop:     $local_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start OWFS at boot time
# Description:       Start OWFS at boot time
### END INIT INFO

# Starts OWFS_server on port 18300
/home/pi/src/1wire_server &
