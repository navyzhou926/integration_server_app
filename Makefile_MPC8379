src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))
target = main

$(target):$(obj)
	powerpc-linux-gnu-gcc -o $@ -g $^ -static -lpthread
	cp ./main /home/navyzhou/tftp/integration_server_test.bin
	cp ./main /home/navyzhou/nfs/rootfs_mpc/home/integration_server_test.bin
%.o:%.c
	powerpc-linux-gnu-gcc -o $@ -g -Wall -c $<
clean:
	rm -f $(obj)
	rm -f $(target)
test:
	@echo $(obj)
