powerpc-linux-gnu-gcc global.c ck2316_alarm.c serial_common.c uart.c -o main -static -lpthread
cp ./main /home/navyzhou/tftp/integration_server_test.bin
#cp ./main /home/navyzhou/nfs/rootfs_mpc/home/integration_server_test.bin
