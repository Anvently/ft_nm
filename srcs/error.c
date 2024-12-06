#include <libft.h>
#include <string.h>
#include <ft_nm.h>

extern const char*	executable_name;

int	error_open_file(const char* path, int errnum) {
	ft_dprintf(2, "%s: '%s': %s\n", executable_name, path, strerror(errnum));
	return (ERROR_SYS);
}

int	error_file_format(const char* path, const char* detail) {
	ft_dprintf(2, "%s: '%s': %s (%s)\n", executable_name, path,
		"file format not recognized", detail ? detail : "");
	return (ERROR_SYS);
}
// static void	

int	error_bad_index(const char* path, size_t index) {
	ft_dprintf(2, "warning: %s: '%s': %s (%#x)\n", executable_name, path,
		"offset is outside file", (unsigned int)index);
	return (ERROR_SYS);
}

int	warning_bad_table_index(const char* path, size_t index) {
	ft_dprintf(2, "warning: %s: '%s': %s (%#x)\n", executable_name, path,
		"corrupt string table index", (unsigned int)index);
	return (ERROR_SYS);
}

int	warning_non_string_section(const char* path, size_t index) {
	ft_dprintf(2, "warning: %s: '%s': %s (%#x)\n", executable_name, path,
		"index should link a string section", (unsigned int)index);
	return (ERROR_SYS);
}

int	warning_corrupt_entry_size(const char* path, size_t entry_size, size_t section_size) {
	ft_dprintf(2, "warning: %s: '%s': %s (%#x/%#x)\n", executable_name, path,
		"corrupted symtab entry size", (unsigned int)entry_size, (unsigned int)section_size);
	return (ERROR_SYS);
}

void	error_alloc(const char* context) {
	static const char*	msg = "ft_nm: fatal error: failed to alloc in context: ";
	write(2, msg, ft_strlen(msg));
	if (context)
		write(2, context, ft_strlen(context));
	write(2, "\n", 1);
	exit(ERROR_FATAL);
}
