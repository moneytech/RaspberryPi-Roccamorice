God.watch do |w|
  w.dir = '/home/pi/src'
  w.uid = 'pi'
  w.name = "1wire_server"
  w.start = "/home/pi/src/1wire_server"
  w.keepalive
end

