SECTIONS
{
  . = 0x2000;
  .text :
  {
	*(.text)
	*(.text*)
	*(.rodata)
	*(.rodata*)
  }
  .data :
  {
  	. = ALIGN(16);
  	*(.data)
  	*(.data*)
  	*(.sdata)
  	*(.sdata*)
  }
  .bss :
  {
  	. = ALIGN(16);
	_bssstart = .;
	*(.bss)
	*(.bss*)
	*(.sbss)
	*(.sbss*)
	*(COMMON);
	_bssend = . ;
  }
}
