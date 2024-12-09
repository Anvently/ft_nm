#include <stdio.h>
#include <libft.h>
#include <ft_nm.h>

int	main(int argc, char **argv) {
	int				ret = 0;
	t_options		options = {0};
	unsigned int	nbr_input = 0;

	if (argc > 1) {
		ret = ft_options_retrieve(argc - 1, argv + 1, &options, &nbr_input);
		if (ret) return (ret);
		// print_option(&options);
	}
	return (ft_nm(nbr_input, argv + 1, &options));
}
