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

	if (index == SHN_ABS)
		return (NULL);
	if (file_info->syms_header.h32.sh_link >= file_info->elf_header.h32.e_shnum) {
		warning_bad_table_index(file_info->path, file_info->syms_header.h32.sh_link);
		return (NULL);
	}
	offset =  file_info->elf_header.h32.e_shoff + (index * file_info->elf_header.h32.e_shentsize);
	if (offset >= file_info->size) {
		error_bad_index(file_info->path, offset);
		return (NULL);
	}
	return ((Elf32_Shdr*)(file_info->mapped_content + offset));
}

/// @brief Retrieve section header name string table from elf header.
/// Non existing index or non-string section will print warning message but won't
/// stop the program.
/// @param file_info 
/// @return 
int	retrieve_shstr_table(t_file_info* file_info) {
	if (IS32(file_info)) {
		file_info->shstr_tbl_header.h32 = nm_get_shdr_32(file_info, file_info->elf_header.h32.e_shstrndx);
		if (IS_VALID_STRTAB(file_info->shstr_tbl_header.h32) == false)
			warning_non_string_section(file_info->path, file_info->elf_header.h32.e_shstrndx);
	} else {
		file_info->shstr_tbl_header.h64 = nm_get_shdr_64(file_info, file_info->elf_header.h64.e_shstrndx);
		if (IS_VALID_STRTAB(file_info->shstr_tbl_header.h64) == false)
			warning_non_string_section(file_info->path, file_info->elf_header.h64.e_shstrndx);
	}
	return (0);
}

/// @brief Return section header address of given address. ```SHN_ABS``` results in ```NULL```
/// without generating an error. Bad index error can be generated and printed.
/// @param file_info 
/// @param index 
/// @return 
Elf64_Shdr*	nm_get_shdr_64(t_file_info* file_info, size_t index) {
	size_t	offset;

	if (index == SHN_ABS)
		return (NULL);
	if (file_info->syms_header.h64.sh_link >= file_info->elf_header.h64.e_shnum) {
		warning_bad_table_index(file_info->path, file_info->syms_header.h64.sh_link);
		return (NULL);
	}
	offset =  file_info->elf_header.h64.e_shoff + (index * file_info->elf_header.h64.e_shentsize);
	if (offset >= file_info->size) {
		error_bad_index(file_info->path, offset);
		return (NULL);
	}
	return ((Elf64_Shdr*)(file_info->mapped_content + offset));
}

/// @warning DEPRECATED ! index value in st_name actually refer to absolute adress offset
/// in strtab and not index in string array
/// @brief Retrieve a string by index in a string array separated with ```\0```.
/// @param file_info 
/// @param array 
/// @param array_size 
/// @param index 
/// @return ```NULL``` if string not found in array, ```<corrupt>``` if non-terminated array
static inline const char*	get_string_from_array(t_file_info* file_info, const char* array, size_t array_size, size_t index) {
	const char*	str;
	const char*	max_addr_array = array + array_size;
	const char*	max_addr_file = file_info->mapped_content + file_info->size;

	// ft_printf("Looking for idx %#x. Array size = %#x\n", index, array_size);
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
/// @return ```NULL``` if idx is 0, ```<corrupt>```
/// if section is not a string section or if idx is outside section
/// @todo May want to implement a safe strlen check to make sure the string is NULL terminated
const char*	nm_get_sym_str(t_file_info* file_info, size_t idx) {
	static const char*	corrupt_str = "<corrupt>";

	if (idx == 0)
		return (NULL);
	if (IS32(file_info)) {
		if (IS_VALID_STRTAB(&file_info->str_tbl_header.h32) == false)
			return corrupt_str;
		else if (idx >= file_info->str_tbl_header.h32.sh_size)
			return corrupt_str;
		else
			return (file_info->mapped_content + file_info->str_tbl_header.h32.sh_offset + idx);
	}
	if (IS_VALID_STRTAB(&file_info->str_tbl_header.h64) == false)
		return corrupt_str;
	else if (idx >= file_info->str_tbl_header.h64.sh_size)
			return corrupt_str;
	else
		return (file_info->mapped_content + file_info->str_tbl_header.h64.sh_offset + idx);
}

static const char* _nm_get_section_str_32(t_file_info* file_info, size_t idx) {
	static const char*	corrupt_str = "<corrupt>";
	Elf32_Shdr*	shdr;

	if (IS_VALID_STRTAB(file_info->shstr_tbl_header.h32) == false)
		return corrupt_str;
	shdr = nm_get_shdr_32(file_info, idx);
	if (shdr == NULL)
		return (corrupt_str);
	else if (shdr->sh_name >= file_info->shstr_tbl_header.h32->sh_size)
		return corrupt_str;
	else
		return (file_info->mapped_content + file_info->shstr_tbl_header.h32->sh_offset + shdr->sh_name);
}

static const char* _nm_get_section_str_64(t_file_info* file_info, size_t idx) {
	static const char*	corrupt_str = "<corrupt>";
	Elf64_Shdr*	shdr;

	if (IS_VALID_STRTAB(file_info->shstr_tbl_header.h64) == false)
		return corrupt_str;
	shdr = nm_get_shdr_64(file_info, idx);
	if (shdr == NULL)
		return (corrupt_str);
	else if (shdr->sh_name >= file_info->shstr_tbl_header.h64->sh_size)
		return corrupt_str;
	else
		return (file_info->mapped_content + file_info->shstr_tbl_header.h64->sh_offset + shdr->sh_name);
}

/// @brief Retrieve the section header identified by ```idx``` and if found
/// return its name string if it is referenced in shstr_tab and if shstr_tab is
/// actually a string table section.
/// @param file_info 
/// @param idx ```SHN_ABS``` index results in empty string
/// @return ```NULL``` if the section has no name, ```<corrupt>``` if the section 
/// does not exists or if it does not point towards an actual string section
/// or if its index value is outside section
/// @todo May want to implement a safe strlen check to make sure the string is NULL terminated
const char*	nm_get_section_str(t_file_info* file_info, size_t idx) {
	if (idx == SHN_ABS)
		return ("");
	if (IS32(file_info))
		return (_nm_get_section_str_32(file_info, idx));
	return (_nm_get_section_str_64(file_info, idx));
}
