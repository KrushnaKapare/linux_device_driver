obj-m := mygpio.o

KDIR := /home/krushna/linux/linux/
PWD :=$(shell pwd)

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) modules

clean: 
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean

