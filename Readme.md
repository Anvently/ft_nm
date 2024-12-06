# FT_NM

A replica of **nm** command, featuring a more robust handling of corrupted file structure.

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

## Tests

This repo features in the tests subfolder a panel of corrupted elf files. It includes various corruption on ELF header part but also various corruption around symtab, strtab and shstrtab.

## Corrupted ELF files and objects

In opposition with GNU standard **nm**, this program is able, to a certain extent, to handle and work around various corruptions of relocable or executable files :
* missing strtab : symbols will be printed without their names, unless their names is contain within the section headers table (which is the case for debugging symbol, -a flag)
* missing shstrtab : symbols that have not their name in strtab (-a flag) will still be printed with a missing name
* corrupted index: it can be a corruption of the index of strtab or shstrtab itself, or a simple corruption of symbol's index in a string table. In such case, they will be printed with a <corrupted> indicator

As long as the symbols themselves are not corrupted, they will be printed, regardless if a name can be linked to them. The intended behaviour was based on how **readelf** command handles these situations. 
	
# TODO

* Return 1 when fatal error occurs (ex: too badly corrupted file or non-elf file)
* Implement types : '-', c/C, g, I, p, s/S

## Tests
* Define more than one symtab in a elf file