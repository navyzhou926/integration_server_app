src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))
target = main

$(target):$(obj)
	/usr/local/arm/2.95.3/bin/arm-linux-gcc -o $@ -g $^ -lpthread
	cp ./main /home/navyzhou/tftp/AT9G20_update_ARM.bin
	#cp ./main /home/navyzhou/upload-folder/TS1500DC_update_ARM.bin
%.o:%.c
	/usr/local/arm/2.95.3/bin/arm-linux-gcc -o $@ -g -Wall -c $<
clean:
	rm -f $(obj)
	rm -f $(target)
test:
	@echo $(obj)
