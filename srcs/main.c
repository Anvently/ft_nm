#include <stdio.h>
#include <libft.h>
#include <ft_nm.h>

const char*	options_str = "\
The options are:\n\
  -a, --debug-syms       Display all symbols, even debuggger only symbols\n\
  -g, --extern-only      Display only external symbols\n\
  -u, --undefined-only   Display only undefined symbols (symbols whose value is not defined in a section within the file)\n\
  -r, --reverse-sort     Reverse the order of the sort (default is alphabetic)\n\
  -p, --no-sort          Do no attempt to sort entries, display them as they come\n\
  -D, --dynamic          Display dynamic symbols instead of normal symbols\n\
";

int	main(int argc, char **argv) {
	int				ret = 0;
	t_options		options = {0};
	unsigned int	nbr_input = 0;

	if (argc > 1) {
		ret = ft_options_retrieve(argc - 1, argv + 1, &options, &nbr_input);
		if (ret){
			write(1, options_str, ft_strlen(options_str));
			return (ret);	
		}
		// print_option(&options);
	}
	return (ft_nm(nbr_input, argv + 1, &options));
}
