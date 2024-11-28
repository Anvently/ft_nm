# FT_NM

# Options

## -a / --debug-syms
	Display all symbols, even debuggger only symbols

## -g / --extern-only
	Display only external symbols

## -u / --undefined-only
	Display only undefined symbols (the symbols that were not defined in the object files themselves)

## -r / --reverse-sort
	Reverse the order of the sort (default is numeric)

## -p / --no-sort
	Do no attempt to sort entries, display them as they come

# Notes

## Corrupting entry size in symtab header
	Contrary to readelf, nm seems to use and rely on sh_entrysize to loop through symtab. When this value is corrupted, nm flag file format as invalid. However using the known size of Elf_Sym structures is enough to deal with such corrupted files. Another approach could be to check if the division of symtab section's size by the struct size is a whole number.
	