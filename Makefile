obj-m += kheap.o
CURRENT_PATH := $(shell pwd)
LINUX_KERNEL_PATH := /home/xiaozaya/rubbish/kernel-pwn/linux-6.2.16
all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
