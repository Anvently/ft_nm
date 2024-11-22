#include <libft.h>
#include <string.h>
#include <ft_nm.h>

extern const char*	executable_name;

int	error_open_file(const char* path, int errnum) {
	ft_dprintf(2, "%s: '%s': %s\n", executable_name, path, strerror(errnum));
	return (ERROR_SYS);
}
// static void	

