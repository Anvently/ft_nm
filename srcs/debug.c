#include <libft.h>
#include <ft_nm.h>

void	print_option(t_options* options) {
	ft_printf("show debug symbols = %d\n", options->display_debug_syms);
	ft_printf("external symbols only = %d\n", options->filter_local);
	ft_printf("undefined symbols only = %d\n", options->filter_defined);
	ft_printf("reverse sort = %d\n", options->reverse_sort);
	ft_printf("sort_by = %s\n", ((char*[]){"numeric", "disable"})[options->sort_by]);
}
