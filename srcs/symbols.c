#include <ft_nm.h>
#include <libft.h>

/** 
  Elf32_Off		e_shoff;		Section header table file offset 
  Elf32_Half	e_shentsize;	Section header table entry size 
  Elf32_Half	e_shnum;		Section header table entry count 
  Elf32_Half	e_shstrndx;		Section header string table index 
**/

# include <string.h>
static int	_comp_symbols_ascii(void* a, void* b) {
	t_symbol_entry*	sA = (t_symbol_entry*)a;
	t_symbol_entry*	sB = (t_symbol_entry*)b;

	return (ft_stricmp_ignore(sA->str, sB->str, " _	."));
}

static int	_comp_symbols_addr32(void* a, void* b) {
	t_symbol_entry*	sA = (t_symbol_entry*)a;
	t_symbol_entry*	sB = (t_symbol_entry*)b;

	if (sA->addr.s32->st_value < sB->addr.s32->st_value)
		return (-1);
	if (sA->addr.s32->st_value > sB->addr.s32->st_value)
		return (1);
	return (0);
}

static int	_comp_symbols_addr64(void* a, void* b) {
	t_symbol_entry*	sA = (t_symbol_entry*)a;
	t_symbol_entry*	sB = (t_symbol_entry*)b;

	if (sA->addr.s64->st_value < sB->addr.s64->st_value)
		return (-1);
	if (sA->addr.s64->st_value > sB->addr.s64->st_value)
		return (1);
	return (0);
}

static int	_retrieve_symbols_hdr_32(t_file_info* file_info) {
	Elf32_Off		index = file_info->elf_header.h32.e_shoff; //Section table offset
	Elf32_Half		entry_size = file_info->elf_header.h32.e_shentsize;
	Elf32_Half		entry_count = file_info->elf_header.h32.e_shnum;
	Elf32_Word		section_type;

	for (int i = 0; i < entry_count; index += entry_size, i++ ) {
		if (index >= file_info->size || (index + entry_size > file_info->size))
			return(error_bad_index(file_info->path, index));
		section_type = ((Elf32_Shdr*)(file_info->mapped_content + index))->sh_type;
		if (section_type == SHT_SYMTAB) { //
			file_info->syms_header.h32 = (Elf32_Shdr*)(file_info->mapped_content + index);
			return (0);
		}
	}
	return (0);
}

static int	_retrieve_symbols_hdr_64(t_file_info* file_info) {
	Elf64_Off	index = file_info->elf_header.h64.e_shoff; //Section table offset
	Elf64_Half	entry_size = file_info->elf_header.h64.e_shentsize;
	Elf64_Half	entry_count = file_info->elf_header.h64.e_shnum;
	Elf64_Word	section_type;

	for (int i = 0; i < entry_count; index += entry_size, i++ ) {
		if (index >= file_info->size || (index + entry_size > file_info->size))
			return(error_bad_index(file_info->path, index));
		section_type = ((Elf64_Shdr*)(file_info->mapped_content + index))->sh_type;
		if (section_type == SHT_SYMTAB) { //
			file_info->syms_header.h64 = (Elf64_Shdr*)(file_info->mapped_content + index);
			return (0);
		}
	}
	return (0);
}

/// @brief Retrieve symbol strtab from sh_link in symtab header.
/// Warning can be raised without returning errors.
/// Undefined or bad index value will result in undefined in ```NULL```,
/// if non-valid strtab, section will still be assigned to file_info 
/// @param file_info 
/// @return Always ```0```
static int	retrieve_str_table_header(t_file_info* file_info) {
	// Check sh_link value for overflow or non string sections
	if (IS32(file_info)) {
		file_info->str_tbl_header.h32 = nm_get_shdr_32(file_info, file_info->syms_header.h32->sh_link);
		if (!file_info->str_tbl_header.h32)
			return (0);
		if (file_info->str_tbl_header.h32->sh_type != SHT_STRTAB)
			warning_non_string_section(file_info->path, file_info->syms_header.h32->sh_link);
	} else if (IS64(file_info)) {
		file_info->str_tbl_header.h64 = nm_get_shdr_64(file_info, file_info->syms_header.h64->sh_link);
		if (!file_info->str_tbl_header.h64)
			return (0);
		if (file_info->str_tbl_header.h64->sh_type != SHT_STRTAB)
			warning_non_string_section(file_info->path, file_info->syms_header.h64->sh_link);
	}
	return (0);
}

/// @brief Retrieve symbols table section header and associated string table
/// Also retrieve string symbol table
/// @param file_info 
/// @param options 
/// @return ```1```if not found
int	retrieve_syms_table_header(t_file_info* file_info) {
	if (IS32(file_info) && _retrieve_symbols_hdr_32(file_info))
		return (ERROR_SYS);
	else if (IS64(file_info) && _retrieve_symbols_hdr_64(file_info))
		return (ERROR_SYS);
	if (!file_info->syms_header.h32 && !file_info->syms_header.h64)
		return (0);
	if (IS32(file_info) && file_info->syms_header.h32->sh_size % file_info->syms_header.h32->sh_entsize != 0)
		warning_corrupt_entry_size(file_info->path, file_info->syms_header.h32->sh_entsize, file_info->syms_header.h32->sh_size);
	else if (IS64(file_info) && file_info->syms_header.h64->sh_size % file_info->syms_header.h64->sh_entsize != 0)
		warning_corrupt_entry_size(file_info->path, file_info->syms_header.h64->sh_entsize, file_info->syms_header.h64->sh_size);
	retrieve_str_table_header(file_info);
	return (0);
}

/// @brief 
/// @param symbol 
/// @param options 
/// @return ```false``` if symbol must be filtered
static bool	_filter_symbol_32(Elf32_Sym* symbol, t_options* options) {
	Elf32_Sym	null_sym = {0};

	// Filter empty symbol
	if (ft_memcmp(symbol, &null_sym, sizeof(null_sym)) == 0)
		return (false);
	return (true);

	//Filter section symbol (debug)
	if (options->display_debug_syms == false && (
		(ELF32_ST_TYPE(symbol->st_info) == STT_SECTION) || (ELF32_ST_TYPE(symbol->st_info) == STT_FILE)))
		return (false);

	//Filter local symbol
	if (options->filter_local && ELF32_ST_BIND(symbol->st_info) == STB_LOCAL)
		return (false);

	//Filter defined symbol (symbol that find their value in a section contained in the file)
	if (options->filter_defined == true && symbol->st_shndx != SHN_UNDEF)
		return (false);

	return (true);
}

/// @brief 
/// @param symbol 
/// @param options 
/// @return ```false``` if symbol must be filtered
static bool	_filter_symbol_64(Elf64_Sym* symbol, t_options* options) {
	Elf64_Sym	null_sym = {0};

	// Filter empty symbol
	if (ft_memcmp(symbol, &null_sym, sizeof(null_sym)) == 0)
		return (false);

	//Filter section symbol (debug)
	if (options->display_debug_syms == false && (
		(ELF64_ST_TYPE(symbol->st_info) == STT_SECTION) || (ELF64_ST_TYPE(symbol->st_info) == STT_FILE)))
		return (false);

	//Filter local symbol
	if (options->filter_local && ELF64_ST_BIND(symbol->st_info) == STB_LOCAL)
		return (false);

	//Filter defined symbol (symbol that find their value in a section contained in the file)
	if (options->filter_defined == true && symbol->st_shndx != SHN_UNDEF)
		return (false);

	return (true);
}

static int	_retrieve_symbols_32(t_file_info* file_info, t_options* options) {
	Elf32_Sym*		symbol = (void*)file_info->mapped_content + file_info->syms_header.h32->sh_offset;
	t_symbol_entry	s_entry;
	(void) 			options;

	file_info->nbr_symbols = file_info->syms_header.h32->sh_size / sizeof(Elf32_Sym);
	file_info->symbols = ft_vector_create(sizeof(t_symbol_entry), file_info->nbr_symbols);
	if (!file_info->symbols)
		error_alloc("allocating symbols vector");
	for (size_t i = 0; i < file_info->nbr_symbols; symbol += 1, i++) {
		if ((void*)symbol > file_info->max_addr)
			return (error_bad_index(file_info->path, (void*)symbol - (void*)file_info->mapped_content));
		if (_filter_symbol_32(symbol, options) == false)
			continue;
		s_entry.addr.s32 = symbol;
		s_entry.shdr.s32 = nm_get_shdr_32(file_info, symbol->st_shndx);
		if (symbol->st_name)
			s_entry.str = nm_get_sym_str(file_info, symbol->st_name);
		else
			s_entry.str = nm_get_section_str(file_info, symbol->st_shndx);
		ft_vector_push(&file_info->symbols, &s_entry);
	}
	return (0);
}

static int	_retrieve_symbols_64(t_file_info* file_info, t_options* options) {
	Elf64_Sym*		symbol = (void*)file_info->mapped_content + file_info->syms_header.h64->sh_offset;
	t_symbol_entry	s_entry;
	(void) 			options;

	file_info->nbr_symbols = file_info->syms_header.h64->sh_size / sizeof(Elf64_Sym);
	file_info->symbols = ft_vector_create(sizeof(t_symbol_entry), file_info->nbr_symbols);
	if (!file_info->symbols)
		error_alloc("allocating symbols vector");
	for (size_t i = 0; i < file_info->nbr_symbols; symbol += 1, i++) {
		if ((void*)symbol > file_info->max_addr)
			return (error_bad_index(file_info->path, (void*)symbol - (void*)file_info->mapped_content));
		if (_filter_symbol_64(symbol, options) == false)
			continue;
		s_entry.addr.s64 = symbol;
		s_entry.shdr.s64 = nm_get_shdr_64(file_info, symbol->st_shndx);
		if (symbol->st_name)
			s_entry.str = nm_get_sym_str(file_info, symbol->st_name);
		else
			s_entry.str = nm_get_section_str(file_info, symbol->st_shndx);
		ft_vector_push(&file_info->symbols, &s_entry);
	}
	return (0);
}

/// @brief Retrieve symbols from symtab, filtering them.
/// Sort them and ignore some according to options.
/// @param file_info 
/// @param options 
/// @return ```0``` for success.
int	ft_nm_retrieve_symbols(t_file_info* file_info, t_options* options) {
	if (IS32(file_info) && _retrieve_symbols_32(file_info, options))
			return (ERROR_SYS);
	else if (IS64(file_info) && _retrieve_symbols_64(file_info, options))
			return (ERROR_SYS);

	file_info->nbr_symbols = ft_vector_size(file_info->symbols);
	switch (options->sort_by) {
		case SORT_BY_ASCII:
			if (ft_merge_sort((t_symbol_entry*)file_info->symbols, file_info->nbr_symbols, _comp_symbols_ascii, options->reverse_sort))
				error_alloc("sorting symbol vector by name");
			break;
		
		case SORT_BY_ADDR:
			if (IS32(file_info) && ft_merge_sort((t_symbol_entry*)file_info->symbols,
				file_info->nbr_symbols, _comp_symbols_addr32, options->reverse_sort))
					error_alloc("sorting symbol vector by address");
			else if (IS64(file_info) && ft_merge_sort((t_symbol_entry*)file_info->symbols,
				file_info->nbr_symbols, _comp_symbols_addr64, options->reverse_sort))
					error_alloc("sorting symbol vector by address");
			break;
	
		default:
			break;
	}
	return (0);
}


/// @brief 
/// @param symbol 
/// @return
/// @todo Need to understantd :
/// - ```-```
/// - ```c/C```
/// - ```g```
/// - ```i/I```
/// - ```p```
/// - difference between ```n``` and ```N```
/// - ```s/S```
static unsigned char	_get_sym_type_64(t_symbol_entry* symbol) {
	unsigned char	ret = '?';

	if (symbol->addr.s64->st_shndx == SHN_UNDEF)
		ret = 'U';
	else if (symbol->addr.s64->st_shndx == SHN_ABS)
		ret = 'a';
	else if (symbol->shdr.s64->sh_type == SHT_NOBITS) //bss section, unitialized data
		ret = 'b';
	else if (ELF64_ST_TYPE(symbol->addr.s64->st_info) == STT_COMMON)
		ret = 'c';
	else if ((symbol->shdr.s64->sh_flags & SHF_ALLOC && symbol->shdr.s64->sh_flags & SHF_WRITE))
		ret = 'd';
	else if (symbol->addr.s64->st_name == SHN_UNDEF && ft_strncmp(".debug", symbol->str, 6) == 0)
		ret = 'N';
	else if (symbol->shdr.s64->sh_flags & SHF_ALLOC && !(symbol->shdr.s64->sh_flags & (SHF_WRITE | SHF_EXECINSTR)))
		ret = 'r';
	else if (symbol->shdr.s64->sh_type == SHT_PROGBITS && symbol->shdr.s64->sh_flags & SHF_EXECINSTR)
		ret = 't';
	if (ft_islower(ret) && ELF64_ST_BIND(symbol->addr.s64->st_info) == STB_GLOBAL)
		ret -= ('a' - 'A');
	else if (ELF64_ST_BIND(symbol->addr.s64->st_info) == STB_WEAK) {
		ret = 'w';
		if (ELF64_ST_TYPE(symbol->addr.s64->st_info) == STT_OBJECT)
			ret = 'v';
		if (symbol->addr.s64->st_shndx)
			ret -= ('a' - 'A');
	}
	return (ret);
}

/// @brief Print symbols from file_info symbols vector
/// @param file_info 
/// @return 
int	ft_nm_print_symbols(t_file_info* file_info) {
	t_symbol_entry*	symbol;

	for (size_t i = 0; i < file_info->nbr_symbols; i++) {
		symbol = (t_symbol_entry*)file_info->symbols + i;
		if (IS64(file_info)) {
			if (symbol->addr.s64->st_shndx == SHN_UNDEF)
				ft_printf("%16s %c %s\n", "", _get_sym_type_64(symbol), symbol->str);
			else
				ft_printf("%016x %c %s\n", symbol->addr.s64->st_value, _get_sym_type_64(symbol), symbol->str);
		}
		else
			ft_printf("%016x X %s\n", symbol->addr.s32->st_value, symbol->str);
	}
	return (0);
}

