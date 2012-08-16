src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))
target = main

$(target):$(obj)
	/usr/local/arm/2.95.3/bin/arm-linux-gcc -o $@ -g $^ -lpthread
	cp ./main /home/navyzhou/tftp/integration_server_test.bin
	#cp ./main /home/navyzhou/nfs/rootfs_mpc/home/integration_server_test.bin
	cp ./main /home/navyzhou/nfs/integration/integration_server_test.bin
%.o:%.c
	/usr/local/arm/2.95.3/bin/arm-linux-gcc -o $@ -g -Wall -c $<
clean:
	rm -f $(obj)
	rm -f $(target)
test:
	@echo $(obj)
