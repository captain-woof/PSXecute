/* linker.ld */
ENTRY(_start)

SECTIONS {
    . = 0x40000000; /* Entrypoint as specified in Psx.h */

    .text : {
        _start = .; 
        *(.text.start)    
        *(.text)

        PROVIDE(_PSX_CALL = .);
            LONG(0x3402002F);             /* lui $v0, 0x2F (load upper immediate 47 / 0x2f) */
            LONG(0x0000000C);             /* syscall */
            LONG(0x00000000);             /* NOP */

        PROVIDE(_PSX_PRINT = .);
            LONG(0x34020000);             /* lui $v0, 0x00 (load upper immediate 0) */
            LONG(0x0000000C);             /* syscall */
            LONG(0x00000000);             /* NOP */

        PROVIDE(_PSX_PRINTF_1 = .);
            LONG(0x34020001);             /* lui $v0, 0x01 (load upper immediate 1) */
            LONG(0x0000000C);             /* syscall */
            LONG(0x00000000);             /* NOP */

        PROVIDE(_PSX_PRINTF_2 = .);
            LONG(0x34020002);             /* lui $v0, 0x02 (load upper immediate 2) */
            LONG(0x0000000C);             /* syscall */
            LONG(0x00000000);             /* NOP */

        PROVIDE(_PSX_PRINTF_3 = .);
            LONG(0x34020003);             /* lui $v0, 0x03 (load upper immediate 3) */
            LONG(0x0000000C);             /* syscall */
            LONG(0x00000000);             /* NOP */

        PROVIDE(_PSX_PRINTF_4 = .);
            LONG(0x34020004);             /* lui $v0, 0x04 (load upper immediate 4) */
            LONG(0x0000000C);             /* syscall */
            LONG(0x00000000);             /* NOP */

        *(.rodata*)
    } 

    /* To be discarded */
    .reginfo : { *(.reginfo) }
    .MIPS.abiflags : { *(.MIPS.abiflags) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .pdr : { *(.pdr) }

}