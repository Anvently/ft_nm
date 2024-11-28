#include <ft_nm.h>
#include <libft.h>

/// @brief Return type of the section at ```index```.
/// @param file_info 
/// @param index 
/// @return ```0``` if section does not exist.
uint32_t	nm_get_shdr_type(t_file_info* file_info, size_t index) {
	size_t	offset;

	if (IS32(file_info)) {
		offset =  file_info->elf_header.h32.e_shoff + (index * file_info->elf_header.h32.e_shentsize);
		if (offset >= file_info->size)
			return (0);
		return (((Elf32_Shdr*)(file_info->mapped_content + offset))->sh_type);
	}
	offset =  file_info->elf_header.h64.e_shoff + (index * file_info->elf_header.h64.e_shentsize);
	if (offset >= file_info->size)
		return (0);
	return (((Elf64_Shdr*)(file_info->mapped_content + offset))->sh_type);
}

Elf32_Shdr*	nm_get_shdr_32(t_file_info* file_info, size_t index) {
	size_t	offset;

	offset =  file_info->elf_header.h32.e_shoff + (index * file_info->elf_header.h32.e_shentsize);
	if (offset >= file_info->size) {
		error_bad_index(file_info->path, offset);
		return (NULL);
	}
	return ((Elf32_Shdr*)(file_info->mapped_content + offset));
}

Elf64_Shdr*	nm_get_shdr_64(t_file_info* file_info, size_t index) {
	size_t	offset;

	offset =  file_info->elf_header.h64.e_shoff + (index * file_info->elf_header.h64.e_shentsize);
	if (offset >= file_info->size) {
		error_bad_index(file_info->path, offset);
		return (NULL);
	}
	return ((Elf64_Shdr*)(file_info->mapped_content + offset));
}

/// @brief Retrieve a string by index in a string array separated with ```\0```.
/// @param file_info 
/// @param array 
/// @param array_size 
/// @param index 
/// @return ```NULL``` if string not found in array, ```<corrupt>``` if non-terminated array
static const char*	get_string_from_array(t_file_info* file_info, const char* array, size_t array_size, size_t index) {
	const char*	str;
	const char*	max_addr_array = array + array_size;
	const char*	max_addr_file = file_info->mapped_content + file_info->size;

	for (size_t i = 0; i < index; i++) {
		str = array;
		while (*str && str < max_addr_array && str < max_addr_file) {
			str++;
		}
		if (str == max_addr_file)
			return ("<corrupt>");
		if (str == max_addr_array)
			return (NULL);
		array = str + 1;
	}
	return (array);
}

/// @brief Return symbol string if found in str_table and if str_table is
/// actually a string table section
/// @param file_info 
/// @param idx 
/// @return ```NULL``` if idx not found in str_table, ```<corrupt>``` if non-terminated section
/// or if section is not a string section
const char*	nm_get_sym_str(t_file_info* file_info, size_t idx) {
	static const char*	corrupt_str = "<corrupt>";

	if (IS32(file_info)) {
		if (file_info->str_tbl_header.h32.sh_type != SHT_STRTAB)
			return corrupt_str;
		else
			return (get_string_from_array(file_info, file_info->mapped_content + file_info->str_tbl_header.h32.sh_offset,
				file_info->str_tbl_header.h32.sh_size, idx));
	}
	if (file_info->str_tbl_header.h64.sh_type != SHT_STRTAB)
		return corrupt_str;
	else
		return (get_string_from_array(file_info, file_info->mapped_content + file_info->str_tbl_header.h64.sh_offset,
			file_info->str_tbl_header.h64.sh_size, idx));
}
