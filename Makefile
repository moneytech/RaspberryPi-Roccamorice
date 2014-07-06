all	:	1wire_server weather_client led rainbow

1wire_server	:	1wire_server.c
		#gcc -g 1wire_server.c -o 1wire_server -lpthread
		gcc -g 1wire_server.c -o 1wire_server 

weather_client	:	weather_client.c
		gcc -g weather_client.c -o weather_client

led     :       led.c
		gcc -g led.c -o led

rainbow	:	rainbow.c
		gcc -g rainbow.c -o rainbow
