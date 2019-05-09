
/* Entry Point */
ENTRY(Reset_Handler)

/* dfu.ld */
INCLUDE build/layout.ld

/* Define output sections */
SECTIONS
{
                            /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :                   /* The program code and other data goes into FLASH */
  {
    _stext = .;	            /* create a global symbol at data start */
    *startup*(.text.Reset_Handler)
    *(.text*)
    *(.rodata*)              /* .rodata sections (constants, strings, etc.) RO */
    *(.glue_7)              /* glue arm to thumb code */
    *(.glue_7t)             /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;             /* define a global symbols at end of code */
    _exit = .;
  } >FLASH


  /* used by the startup to initialize got */
  _sigot = .;
  .got : AT ( _sigot ) {
    . = ALIGN(4);
        _sgot = .;
                            /*  *(.got.plt)
                             *    We don't need plt segment
                             *    since we do not need dynamic library relocation
                             */
        *(.got)

    . = ALIGN(4);
    _egot = .;
  } >FLASH

  __exidx_start = .;
  .ARM.exidx :
  {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > FLASH
  __exidx_end = .;

  _sidata = .;              /* used by the startup to initialize data */
  .data : AT ( _sidata )    /* Initialized data sections goes into RAM, load LMA copy after code */
  {
    . = ALIGN(4);
    _sdata = .;             /* create a global symbol at data start */
    *(.data)                /* .data sections */
    *(.data*)               /* .data* sections */

    . = ALIGN(4);
    _edata = .;             /* define a global symbol at data end */
  } >RAM

                            /* Uninitialized data section */
  . = ALIGN(4);
  .bss :                    /*  Uninitialized global and static variables (zero value on startup) */
  {
                            /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;              /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *debug.o(.bss)
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;              /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM


  ._user_heap_stack :       /* User_heap_stack section, used to check that there is enough RAM left */
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM


                            /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) } /* See §4.3.6 Build Attributes. IHI0044F_aaelf.pdf */
}

