OUTPUT_FORMAT("elf64-littleaarch64", "elf64-bigaarch64",
	      "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)

MEMORY
{
	flash (rx)  : ORIGIN = 0x0, LENGTH = 0x08000000
	ram   (rwx) : ORIGIN = 0x40000000, LENGTH = 0x80000000
}


ENTRY(_start)
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  . = SEGMENT_START("text-segment", 0x40000000);
  .text :
  {
    _text = .;
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(.text .stub .text.* .gnu.linkonce.t.*)
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
  } > ram
  PROVIDE(__etext = .);
  PROVIDE(_etext = .);
  PROVIDE(etext = .);
  .rodata :
  {
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  } > ram

  .data :
  {
    __data_start = .;
    *(.data .data.* .gnu.linkonce.d.*)
  } > ram
  _edata = .;
  PROVIDE (edata = .);
  . = .;
  __bss_start = .;
  __bss_start__ = .;
  .bss :
  {
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.
      FIXME: Why do we need it? When there is no .bss section, we do not
      pad the .data section.  */
   . = ALIGN(. != 0 ? 64 / 8 : 1);
  } > ram
  _bss_end__ = .; __bss_end__ = .;
  . = ALIGN(64 / 8);
  __end__ = .;
  _end = .;
  PROVIDE (end = .);
  .stack :
  {
    _stack = .;
    *(.stack)
  } > ram
}
