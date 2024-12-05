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

	return (ft_stricmp_ignore(sA->str, sB->str, " _	"));
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
			ft_memcpy(&file_info->syms_header, file_info->mapped_content + index, entry_size);
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
			ft_memcpy(&file_info->syms_header, file_info->mapped_content + index, entry_size);
			return (0);
		}
	}
	return (0);
}

/// @brief Check validity of symtable header. Warning can be raised without
/// returning errors. If symtab link a valid string table section, ```str_tbl_header```
/// is filled in ```file_info```.
/// @param file_info 
/// @return 
static int	retrieve_str_table_header(t_file_info* file_info) {
	Elf32_Shdr*	section32;
	Elf64_Shdr*	section64;

	// Check sh_link value for overflow or non string sections
	if (IS32(file_info)) {
		if (file_info->syms_header.h32.sh_link >= file_info->elf_header.h32.e_shnum)
			return (warning_bad_table_index(file_info->path, file_info->syms_header.h32.sh_link));
		section32 = nm_get_shdr_32(file_info, file_info->syms_header.h32.sh_link);
		if (!section32)
			return (ERROR_SYS);
		if (section32->sh_type != SHT_STRTAB)
			warning_non_string_section(file_info->path, file_info->syms_header.h32.sh_link);
		ft_memcpy(&file_info->str_tbl_header, section32, file_info->elf_header.h32.e_shentsize);
	} else if (IS64(file_info)) {
		if (file_info->syms_header.h64.sh_link >= file_info->elf_header.h64.e_shnum)
			return (warning_bad_table_index(file_info->path, file_info->syms_header.h64.sh_link));
		section64 = nm_get_shdr_64(file_info, file_info->syms_header.h64.sh_link);
		if (!section64)
			return (ERROR_SYS);
		if (section64->sh_type != SHT_STRTAB)
			warning_non_string_section(file_info->path, file_info->syms_header.h64.sh_link);
		ft_memcpy(&file_info->str_tbl_header, section64, file_info->elf_header.h64.e_shentsize);
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
	if (IS32(file_info) && file_info->syms_header.h32.sh_size % file_info->syms_header.h32.sh_entsize != 0)
		warning_corrupt_entry_size(file_info->path, file_info->syms_header.h32.sh_entsize, file_info->syms_header.h32.sh_size);
	else if (IS64(file_info) && file_info->syms_header.h64.sh_size % file_info->syms_header.h64.sh_entsize != 0)
		warning_corrupt_entry_size(file_info->path, file_info->syms_header.h64.sh_entsize, file_info->syms_header.h64.sh_size);
	if (retrieve_str_table_header(file_info))
		return (ERROR_SYS);
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
	Elf32_Sym*		symbol = (void*)file_info->mapped_content + file_info->syms_header.h32.sh_offset;
	t_symbol_entry	s_entry;
	(void) 			options;

	file_info->nbr_symbols = file_info->syms_header.h32.sh_size / sizeof(Elf32_Sym);
	file_info->symbols = ft_vector_create(sizeof(t_symbol_entry), file_info->nbr_symbols);
	if (!file_info->symbols)
		error_alloc("allocating symbols vector");
	for (size_t i = 0; i < file_info->nbr_symbols; symbol += 1, i++) {
		if ((void*)symbol > file_info->max_addr)
			return (error_bad_index(file_info->path, (void*)symbol - (void*)file_info->mapped_content));
		if (_filter_symbol_32(symbol, options) == false)
			continue;
		s_entry.addr.s32 = symbol;
		s_entry.str = nm_get_sym_str(file_info, symbol->st_name);
		ft_vector_push(&file_info->symbols, &s_entry);
	}
	return (0);
}

static int	_retrieve_symbols_64(t_file_info* file_info, t_options* options) {
	Elf64_Sym*		symbol = (void*)file_info->mapped_content + file_info->syms_header.h64.sh_offset;
	t_symbol_entry	s_entry;
	(void) 			options;

	file_info->nbr_symbols = file_info->syms_header.h64.sh_size / sizeof(Elf64_Sym);
	file_info->symbols = ft_vector_create(sizeof(t_symbol_entry), file_info->nbr_symbols);
	if (!file_info->symbols)
		error_alloc("allocating symbols vector");
	for (size_t i = 0; i < file_info->nbr_symbols; symbol += 1, i++) {
		if ((void*)symbol > file_info->max_addr)
			return (error_bad_index(file_info->path, (void*)symbol - (void*)file_info->mapped_content));
		if (_filter_symbol_64(symbol, options) == false)
			continue;
		s_entry.addr.s64 = symbol;
		s_entry.str = nm_get_sym_str(file_info, symbol->st_name);
		ft_vector_push(&file_info->symbols, &s_entry);
	}
	return (0);
}

// static int	_print_symbols_64(t_file_info* file_info, t_options* options) {
// 	size_t			sym_offset = file_info->syms_header.h64.sh_offset;
// 	size_t			max_offset = sym_offset + file_info->syms_header.h64.sh_size;
// 	const char*		sym_str;
// 	(void) 			options;

// 	// ft_hexdump(file_info->mapped_content, file_info->str_tbl_header.h64.sh_size, 1, file_info->str_tbl_header.h64.sh_offset);
// 	// ft_hexdump(file_info->mapped_content + file_info->str_tbl_header.h64.sh_offset, file_info->str_tbl_header.h64.sh_size, 1, 0);
// 	// ft_hexdump_color_zone(file_info->mapped_content + file_info->syms_header.h64.sh_offset, file_info->syms_header.h64.sh_size / 2, 2, 0, 12);
// 	for (; sym_offset < max_offset; sym_offset += sizeof(Elf64_Sym)) {
// 		if (sym_offset >= file_info->size)
// 			return (error_bad_index(file_info->path, sym_offset));
// 		sym_str = nm_get_sym_str(file_info, ((Elf64_Sym*)(file_info->mapped_content + sym_offset))->st_name);
// 		ft_printf("%s\n", sym_str);
// 	}

// 	return (0);
// }


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

// static int	print_one_symbol(t_symbol_entry symbol, size_t len_str) {
// 	char	output[16 + 3 + len_str + 2];
// 	// char* output = malloc(16 + 3 + len_str + 2);

// 	ft_memcpy(output, "0000000000000000 X ", 19);
// 	ft_putunbr_buffer((unsigned long)symbol.addr.s64->st_value, &output[0], 16);
// 	printf("%lu\n", symbol.addr.s64->st_value);
// 	// ft_memcpy(output, symbol.addr.s64->st_value, sizeof(symbol.addr.s64->st_value));
// 	ft_memcpy(output + 19, symbol.str, len_str);
// 	output[16 + 3 + len_str] = '\n';
// 	output[16 + 3 + len_str + 1] = '\0';
// 	write(1, output, 16 + 3 + len_str + 1);
// 	// free(output);
// 	return (0);
// }

// static unsigned char	get_symbol_type(t_symbol_entry* symbol) {
// 	const unsigned char	type[] = "?";
// }

/// @brief Print symbols from file_info symbols vector
/// @param file_info 
/// @return 
int	ft_nm_print_symbols(t_file_info* file_info) {
	t_symbol_entry*	symbol;

	for (size_t i = 0; i < file_info->nbr_symbols; i++) {
		symbol = (t_symbol_entry*)file_info->symbols + i;
		if (IS64(file_info))
			ft_printf("%016x X %s\n", symbol->addr.s64->st_value, symbol->str);
		else
			ft_printf("%016x X %s\n", symbol->addr.s32->st_value, symbol->str);
	}
	return (0);
}

