#ifndef FT_OPTIONS_H
# define FT_OPTIONS_H

# ifndef __FT_ERRORS
#  define __FT_ERRORS
enum	E_ERRORS {
		ERROR_FATAL = -1,
		SUCCESS = 0,
		ERROR_SYS = 1,
		ERROR_INPUT = 2
};
# endif

// # ifndef __FT_NBR_OPTIONS
// #  error "__FT_NBR_OPTIONS must be define before any inclusion of ft_options.h"

// # endif

// A data structure that will be given to the handler, containing for example the list of enable
// flags
typedef struct s_options t_options;

enum	ARG_TYPE {
			ARG_NONE,
			ARG_OPTIONNAL,
			ARG_REQUIRED
};

typedef struct s_option_flag {
	char			short_id;
	char*			long_id;
	enum ARG_TYPE	arg;
	int				(*handler)(t_options*, char*);
} t_opt_flag;

int	ft_options_retrieve(int nbr, char** args, t_options* options);
int	ft_options_err_invalid_argument(const char* option, const char* arg, const char*** valids);
int	ft_options_err_ambiguous_argument(const char* option, const char* arg, const char*** valids);

#endif

