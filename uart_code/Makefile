all:
	/usr/local/arm/2.95.3/bin/arm-linux-gcc -static main.c uart.c -o uart.bin
	cp uart.bin /home/navyzhou/tftp/ -r
clean:
	rm uart.bin
