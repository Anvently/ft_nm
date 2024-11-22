#include <ft_nm.h>
#include <libft.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

static int map_file(const char* path, const char** mapped_dest, size_t* size) {
	int			fd;
	struct stat	file_stats;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (error_open_file(path, errno), ERROR_SYS);
	if (fstat(fd, &file_stats) < 0) {
		close (fd);
		return (error_open_file(path, errno), ERROR_SYS);
	}
	*size = file_stats.st_size;
	*mapped_dest = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (*mapped_dest == MAP_FAILED) {
		close (fd);
		return (error_open_file(path, errno), ERROR_SYS);
	}
	close (fd);
	return (SUCCESS);
}

static int	nm_file(char* path, t_options* options) {
	const char*	mapped;
	size_t		size;

	(void) options;
	if (map_file(path, &mapped, &size))
		return (ERROR_SYS);
	ft_printf("\n%s:\n", path);
	for (size_t i = 0; i < size; i++) {
		write(1, mapped + i, 1);
	}
	return (0);
}

int	ft_nm(int nbr_arg, char** args, t_options* options) {
	int		ret = 0;
	bool	no_arg = true;

	for (int i = 0; i < nbr_arg; i++) {
		if (*args == NULL) continue;
		no_arg = false;
		switch (nm_file(*(args + i), options))
		{
			case SUCCESS:
				break;

			case ERROR_SYS:
				ret = ERROR_SYS;
				break;
			
			default:
				return (ERROR_FATAL);
		}
	}
	if (no_arg)
		return (nm_file("a.out", options));
	return (ret);
}
