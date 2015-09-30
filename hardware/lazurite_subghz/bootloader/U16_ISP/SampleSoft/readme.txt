SampleSoft
|- BOOTLOADER
|  |- _output
|  |  |- _obj
|  |     |- main.asm
|  |- main
|  |  |- main.c
|  |  |- vector_bootloader.h
|  |- bootloader_small.map
|  |- bootloader_small.PID
|- common
|  |- mcu_large
|  |  |- mcu.h
|  |- mcu_small
|  |  |- mcu.h
|  |- isp.h
|  |- led.h
|  |- remap.c
|  |- remap.h
|  |- sys_clock.c
|  |- sys_clock.h
|  |- sys_uart.c
|  |- sys_uart.h
|- driver
|  |- clock
|  |  |- clock_i.h
|  |- common
|  |  |- rdwr_reg.h
|  |- flash
|  |  |- flash.c
|  |  |- flash.h
|  |- irq
|  |  |- irq_i.h
|  |- tbc
|  |  |- tbc.c
|  |  |- tbc.h
|  |- uart
|  |  |- uart_common_i.h
|  |  |- uart0_i.c
|  |  |- uart0_i.h
|  |- wdt
|     |- wdt.c
|     |- wdt.h
|- ISP
|  |- _output
|  |  |- _obj
|  |     |- crc.asm
|  |     |- flash.asm
|  |     |- isp.asm
|  |     |- main.asm
|  |     |- remap.asm
|  |     |- smpl_common.asm
|  |     |- sys_clock.asm
|  |     |- sys_uart.asm
|  |     |- tbc.asm
|  |     |- uart0_i.asm
|  |     |- wdt.asm
|  |     |- xmodem.asm
|  |- main
|  |  |- crc.c
|  |  |- crc.h
|  |  |- isp.c
|  |  |- main.c
|  |  |- main.h
|  |  |- vector_isp.h
|  |  |- xmodem.c
|  |  |- xmodem.h
|  |- isp_small.map
|  |- isp_small.PID
|- obj
|  |- isp_small.res
|  |- ML620Q154_000RA.HEX
|  |- ML620Q155_000RA.HEX
|  |- ML620Q156_000RA.HEX
|  |- ML620Q504_000RA.HEX
|  |- update_user_ML620Q154.bin
|  |- update_user_ML620Q155.bin
|  |- update_user_ML620Q156.bin
|  |- update_user_ML620Q504.bin
|  |- updated_user_small.res
|- SAMPLE_USER
|  |- _output
|  |  |- _obj
|  |     |- flash.asm
|  |     |- main.asm
|  |     |- remap.asm
|  |     |- smpl_common.asm
|  |     |- sys_clock.asm
|  |     |- sys_uart.asm
|  |     |- tbc.asm
|  |     |- uart0_i.asm
|  |     |- wdt.asm
|  |- main
|  |  |- main.c
|  |  |- main.h
|  |  |- vector_user.h
|  |- update_user_small.map
|  |- update_user_small.PID
|  |- user_small.map
|  |- user_small.PID
|- startup
|  |- ML620154.ASM
|  |- ML620155.ASM
|  |- ML620156.ASM
|  |- ML620504.ASM
|  |- ML620504_BOOTLOADER.ASM
|- readme.txt
