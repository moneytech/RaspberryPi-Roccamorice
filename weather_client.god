God.watch do |w|
  w.dir = '/home/pi/src'
  w.uid = 'pi'
  w.name = "weather_client"
  w.start = "/home/pi/src/weather_client"
  w.keepalive
end


