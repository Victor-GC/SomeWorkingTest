#DEBUG = y

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG
else
  DEBFLAGS = -O2 
endif

EXTRA_CFLAGS += $(DEBFLAGS)
CLFAGS += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)

#scull-objs := scull.o
obj-m	:= scull.o

else

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
KERNELDIR ?= ~/树莓派交叉编译工具/RpiLinux
PWD       := $(shell pwd)

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules

endif


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
