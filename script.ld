SECTIONS
     {
       . = 0x8000;
       .data : { *(.data) }
       . = 0x9000;
       .bss : { *(.bss) }
       . = 0xa000;
       .text : { *(.text) }
       _end = . ;
     }
