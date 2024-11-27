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
	// Check file type
	*size = file_stats.st_size;
	*mapped_dest = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (*mapped_dest == MAP_FAILED) {
		close (fd);
		return (error_open_file(path, errno), ERROR_SYS);
	}
	close (fd);
	return (SUCCESS);
}

/// @brief NOT checked : EI_OSABI, EI_ABIVERSION, e_type, e_machine, 
/// @param file_info 
/// @return 
static int	check_header(t_file_info* file_info) {
	// Check data type
	if (file_info->header.h32.e_ident[EI_DATA] < ELFDATA2LSB || file_info->header.h32.e_ident[EI_DATA] > ELFDATA2MSB)
		return (ERROR_SYS);
	// Check version / always = 1
	if (file_info->header.h32.e_ident[EI_VERSION] != EV_CURRENT)
		return (ERROR_SYS);
	
}

/// @brief Fill header member according to file class (x32 or x64).
/// @param file_info 
/// @return ```1``` for incorrect format
static int	retrieve_header(t_file_info* file_info) {
	if (file_info->size < EI_NIDENT)
		return (ERROR_SYS);
	if (ft_memcmp(ELFMAG, file_info->mapped_content, SELFMAG)) //Check magic number
		return (ERROR_SYS);
	if (file_info->mapped_content[EI_CLASS] == ELFCLASS32)
		ft_memcpy(&file_info->header, file_info->mapped_content, sizeof(Elf32_Ehdr));
	else if (file_info->mapped_content[EI_CLASS] == ELFCLASS64)
		ft_memcpy(&file_info->header, file_info->mapped_content, sizeof(Elf64_Ehdr));
	else
		return (ERROR_SYS);
	
	return (SUCCESS);
}

static int	nm_file(char* path, t_options* options) {
	// const char*	mapped;
	// size_t		size;
	t_file_info	file_info = {0};

	(void) options;
	if (map_file(path, &file_info.mapped_content, &file_info.size))
		return (ERROR_SYS);
	if (retrieve_header(&file_info))
		return (error_file_format(path), ERROR_SYS);
	ft_printf("\n%s:\n", path);
	ft_printf("ok!\n");
	
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
