#include <ft_nm.h>

static int	option_set_debug_syms(t_options* options, char*);
static int	option_set_extern_only(t_options* options, char*);
static int	option_set_undefined_only(t_options* options, char*);
static int	option_set_reverse_sort(t_options* options, char*);
static int	option_disable_sort(t_options* options, char*);
static int	option_set_dyn_syms(t_options* options, char*);

t_opt_flag options_list[__FT_NBR_OPTIONS] = {
	[OPT_DEBUG_SYMS]		= (t_opt_flag) {
		.short_id = 'a',
		.long_id = "debug-syms",
		.handler = &option_set_debug_syms
	},
	[OPT_EXTERN_ONLY]	= (t_opt_flag) {
		.short_id = 'g',
		.long_id = "extern-only",
		.handler = &option_set_extern_only
	},
	[OPT_UNDEFINED_ONLY]	= (t_opt_flag) {
		.short_id = 'u',
		.long_id = "undefined-only",
		.handler = &option_set_undefined_only
	},
	[OPT_REV_SORT]			= (t_opt_flag) {
		.short_id = 'r',
		.long_id = "reverse-sort",
		.handler = &option_set_reverse_sort
	},
	[OPT_DISABLE_SORT]		= (t_opt_flag) {
		.short_id = 'p',
		.long_id = "no-sort",
		.handler = &option_disable_sort
	},
	[OPT_DYN_SYMS]			= (t_opt_flag) {
		.short_id = 'D',
		.long_id = "dynamic",
		.handler = &option_set_dyn_syms
	}
};

t_opt_flag*	options_map = &options_list[0];
int			NBR_OPTIONS = __FT_NBR_OPTIONS;
const char*	executable_name = "ft_nm";

static int	option_set_debug_syms(t_options* options, char* arg) {
	(void) arg;

	options->display_debug_syms = true;
	return (0);
}

static int	option_set_extern_only(t_options* options, char* arg) {
	(void) arg;

	options->filter_local = true;
	return (0);
}

static int	option_set_undefined_only(t_options* options, char* arg) {
	(void) arg;

	options->filter_defined = true;
	return (0);
}

static int	option_set_reverse_sort(t_options* options, char* arg) {
	(void) arg;

	options->reverse_sort = true;
	return (0);
}

static int	option_disable_sort(t_options* options, char* arg) {
	(void) arg;

	options->sort_by = SORT_BY_DISABLE;
	return (0);
}

static int	option_set_dyn_syms(t_options* options, char* arg) {
	(void) arg;

	options->dyn_syms = true;
	return (0);
}

