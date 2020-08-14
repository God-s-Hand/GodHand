
CC = arm-none-linux-gnueabi-gcc

godhand : main.o device.o camera.o device_header.h common_header.h
	$(CC) -o $@ $^ -lpthread

main.o : main.c common_header.h device_header.h
	$(CC) -c $< 

device.o : device.c device_header.h common_header.h 
	$(CC) -c $<

camera.o : camera.c device_header.h common_header.h
	$(CC) -c $<

clean :
	rm -rf *.o
