#ifndef FT_NM_H
# define FT_NM_H

# include <stdbool.h>
# include <stdlib.h>
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
	SORT_BY_NUMERIC,
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

typedef struct {
	t_elf_header	header;
	size_t			size;
	const char*		mapped_content;
}	t_file_info;

int	ft_nm(int nbr_arg, char** args, t_options* options);

/* ERRORS DEFINITION */

int	error_open_file(const char* path, int errnum);
int	error_file_format(const char* path);

/* ARGS */

/* DEBUG */

void	print_option(t_options* options);

#endif