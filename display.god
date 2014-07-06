God.watch do |w|
  w.dir = '/home/pi/4d_display'
  w.uid = 'pi'
  w.name = "display"
  w.start = "/home/pi/4d_display/display"
  w.keepalive
end

