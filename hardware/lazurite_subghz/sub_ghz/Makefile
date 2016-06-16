KERNEL_SRC=/lib/modules/$(shell uname -r)/build
CFILES = drv-lazurite.c subghz_api.c CTI/hwif/i2c-lzpi.c CTI/hwif/spi-lzpi.c CTI/hwif/hal-lzpi.c CTI/api/bp3596.c CTI/core/ml7396.c CTI/hwif/ml7396_hwif_hal.c CTI/hwif/random.c

obj-m += lazdriver.o
lazdriver-objs := $(CFILES:.c=.o)

all:
	make -C $(KERNEL_SRC) SUBDIRS=$(PWD) modules

clean:
	make -C $(KERNEL_SRC) SUBDIRS=$(PWD) clean

clean-files := *.o *.ko *.mod.[co] *~

