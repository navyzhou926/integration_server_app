src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))
target = main

$(target):$(obj)
	/usr/local/arm/2.95.3/bin/arm-linux-gcc -o $@ -g $^ -lpthread
	cp ./main /home/navyzhou/tftp/arm.bin
	#cp ./main /home/navyzhou/nfs/rootfs_mpc/home/integration_server_test.bin
	#cp ./main /home/navyzhou/nfs/integration/integration_server_test.bin
	cp main /home/navyzhou/ubuntuone_navy/integration_server/jffs2/rootfs/home/program/arm.bin
%.o:%.c
	/usr/local/arm/2.95.3/bin/arm-linux-gcc -o $@ -g -Wall -c $<
clean:
	rm -f $(obj)
	rm -f $(target)
test:
	@echo $(obj)
