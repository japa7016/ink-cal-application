# DEBUG = y

ifeq ($(DEBUG),y)
  DEBFLAGS := -O -g -DFB_EPD_DEBUG
else
  DEBFLAGS := -O2
endif
EXTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)
# call from kernel build system
obj-m	:= fb_epd.o
fb_epd-y := main.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
