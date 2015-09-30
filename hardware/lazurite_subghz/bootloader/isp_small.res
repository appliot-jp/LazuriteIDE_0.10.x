//Response file(sample.res)

BOOT_INIT_PROG.HEX /AL(0H, 0EFFFH, 0H)          //     0H : Sample user program(0 - 0EFFFH)

ISP_SMALL.HEX /AL(0H, 0BFFH, 0F000H)        // 0F000H : ISP program( 0 - 0BFFH)

BOOTLOADER_SMALL.HEX /AL(0H, 01DFH, 0FC00H) // 0FC00H : Bootloader program(0 - 01DFH)

//USER_SMALL.HEX /AL(0FDE0H, 0FDE1H, 0FDE0H)  // 0FDE0H : Code option data of sample user program
                                            // (Unnecessary, when not specifying code option data)

/TML620504                                  // MCU

/FML620Q504_000                             // Output file name

/OH                                         // Output format(OH : Intel HEX)
