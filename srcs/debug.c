#include <libft.h>
#include <ft_nm.h>

void	print_option(t_options* options) {
	ft_printf("show debug symbols = %d\n", options->display_debug_syms);
	ft_printf("external symbols only = %d\n", options->filter_local);
	ft_printf("undefined symbols only = %d\n", options->filter_defined);
	ft_printf("reverse sort = %d\n", options->reverse_sort);
	ft_printf("sort_by = %s\n", ((char*[]){"numeric", "disable"})[options->sort_by]);
}

static void	print_section_header_32(t_section_header* header) {
	ft_printf("name (index in string table) = %#x\n", header->h32.sh_name);
	ft_printf("type = %#x\n", header->h32.sh_type);
	ft_printf("flags = %#x\n", header->h32.sh_flags);
	ft_printf("virtual addr = %#x\n", header->h32.sh_addr);
	ft_printf("offset = %#x\n", header->h32.sh_offset);
	ft_printf("size = %#x\n", header->h32.sh_size);
	ft_printf("link = %#x\n", header->h32.sh_link);
	ft_printf("info = %#x\n", header->h32.sh_info);
	ft_printf("section alignment = %#x\n", header->h32.sh_addralign);
	ft_printf("entry size if table = %#x\n", header->h32.sh_entsize);
}

static void	print_section_header_64(t_section_header* header) {
	ft_printf("name (index in string table) = %#x\n", header->h64.sh_name);
	ft_printf("type = %#x\n", header->h64.sh_type);
	ft_printf("flags = %#x\n", header->h64.sh_flags);
	ft_printf("virtual addr = %#x\n", header->h64.sh_addr);
	ft_printf("offset = %#x\n", header->h64.sh_offset);
	ft_printf("size = %#x\n", header->h64.sh_size);
	ft_printf("link = %#x\n", header->h64.sh_link);
	ft_printf("info = %#x\n", header->h64.sh_info);
	ft_printf("section alignment = %#x\n", header->h64.sh_addralign);
	ft_printf("entry size if table = %#x\n", header->h64.sh_entsize);

}

void	print_section_header(t_section_header* header, unsigned char class) {
	if (class == ELFCLASS32)
		print_section_header_32(header);
	else if (class == ELFCLASS64)
		print_section_header_64(header);
	else
		ft_dprintf(2, "ERROR INVALID CLASS IN DEBUG\n");
}

