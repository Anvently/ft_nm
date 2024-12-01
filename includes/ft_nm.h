#ifndef FT_NM_H
# define FT_NM_H

# include <stdbool.h>
# include <stdlib.h>
# include <libft.h>
# include <elf.h>

enum	OPTIONS {
			OPT_DEBUG_SYMS = 0, // -a
			OPT_EXTERN_ONLY, // -g
			OPT_UNDEFINED_ONLY, // -u
			OPT_REV_SORT, // -r
			OPT_DISABLE_SORT, // -p
			OPT_NBR_OPTIONS
};

# define __FT_NBR_OPTIONS 5
// int	NBR_OPTIONS = 5;
# include <ft_options.h>

# ifndef __FT_ERRORS
#  define __FT_ERRORS
enum	E_ERRORS {
		ERROR_FATAL = -1,
		SUCCESS = 0,
		ERROR_SYS = 1,
		ERROR_INPUT = 2
};
# endif

enum SORT_BY {
	SORT_BY_ASCII,
	SORT_BY_ADDR,
	SORT_BY_DISABLE
};

typedef struct s_options {
	enum SORT_BY	sort_by;
	bool			reverse_sort;
	bool			display_debug_syms; // -a option
	bool			filter_local; // -g options, external only
	bool			filter_defined; // -u options, undefined only
}	t_options;

/* Main functions */

// # define GET_CLASS(header) (header.h32)

typedef union {
	Elf64_Ehdr	h64;
	Elf32_Ehdr	h32;
}	t_elf_header;

typedef union {
	Elf64_Shdr	h64;
	Elf32_Shdr	h32;
}	t_section_header;

typedef struct {
	t_elf_header		elf_header;
	t_section_header	syms_header;
	t_section_header	str_tbl_header;
	size_t				size;
	const char*			mapped_content;
	const char*			path;
	t_vector*			symbols;
	size_t				nbr_symbols;
	void*				max_addr;
}	t_file_info;

typedef struct {
	union {
		Elf32_Sym*	s32;
		Elf64_Sym*	s64;
	}				addr;
	const char*		str;
}	t_symbol_entry;

// Macro to cast given address
#define GET_CLASS(file_ptr) (((t_file_info*)file_ptr)->elf_header.h32.e_ident[EI_CLASS])
#define IS32(file_ptr) (GET_CLASS(file_ptr) == ELFCLASS32)
#define IS64(file_ptr) (GET_CLASS(file_ptr) == ELFCLASS64)

int	ft_nm(int nbr_arg, char** args, t_options* options);
int	ft_nm_retrieve_symbols(t_file_info* file_info, t_options* options);
int	ft_nm_print_symbols(t_file_info* file_info);

/* UTILS */

uint32_t	nm_get_shdr_type(t_file_info* file_info, size_t index);
Elf32_Shdr*	nm_get_shdr_32(t_file_info* file_info, size_t index);
Elf64_Shdr*	nm_get_shdr_64(t_file_info* file_info, size_t index);
const char*	nm_get_sym_str(t_file_info* file_info, size_t idx);

/* ERRORS DEFINITION */

int		error_open_file(const char* path, int errnum);
int		error_file_format(const char* path, const char* detail);
int		error_bad_index(const char* path, size_t index);
int		warning_bad_table_index(const char* path, size_t index);
int		warning_non_string_section(const char* path, size_t index);
int		warning_corrupt_entry_size(const char* path, size_t entry_size, size_t section_size);
void	error_alloc(const char* context);

/* ARGS */

/* DEBUG */

void	print_option(t_options* options);
void	print_section_header(t_section_header* header, unsigned char class);

#endif