#include "elf.h"

bool _init_elf(
    const char *file,
    int *file_desc,
    Elf **elf_obj,
    Elf_Kind *elf_k)
{
    if (elf_version(EV_CURRENT) == EV_NONE) {
        ERROR("%s: Elf library initialization failed: %s", 
           __func__, elf_errmsg(-1));
        return 1;
    }

    if ((*file_desc = open(file, O_RDONLY)) == -1)
    {
        ERROR("%s: Failed to open file: %s", __func__, strerror(*file_desc));
        return 1;
    }

    if ((*elf_obj = elf_begin(*file_desc, ELF_C_READ, NULL)) == NULL)
    {
        ERROR("%s: elf_begin() failed: %s", __func__, elf_errmsg(-1));
        close(*file_desc);
        return 1;
    }

    if ((*elf_k = elf_kind(*elf_obj)) != ELF_K_ELF)
    {
        ERROR("%s: Not an ELF object", __func__);
        elf_end(*elf_obj);
        close(*file_desc);
        return 1;
    }

    return 0;
}

/**
 * @brief   Get section header 
 * @note    scn & shdr are will be updated to point to the found section 
 * @param  *elf_obj: 
 * @param  **out_scn: 
 * @param  *out_shdr: 
 * @param  section_type: 
 * @retval 0: success, 1: error
 */
bool _get_shdr_by_type(
    Elf *elf_obj,
    Elf_Scn **out_scn,
    GElf_Shdr *out_shdr,
    uint64_t section_type)
{
    if (!elf_obj || !out_shdr)
    {
        ERROR("%s: Invalid parameters", __func__);
        return 1;
    }

    while ((*out_scn = elf_nextscn(elf_obj, *out_scn)) != NULL)
    {
        if (gelf_getshdr(*out_scn, out_shdr) != out_shdr) {
            ERROR("%s: Failed to get section header: %s", __func__, elf_errmsg(-1));
            continue;
        }

        if (out_shdr->sh_type == section_type)
            return 0;
    }

    ERROR("%s: Section type %llu not found", __func__, section_type);
    return 1;
}

void elf_resolve_flash_address_to_function(const char *file,
    uint32_t address,
    char *func_name,
    size_t name_size)
{
    int file_desc;
    Elf *elf_obj;
    Elf_Kind elf_k;     // recognized Elf type
    Elf_Data *data;     // section data
    Elf_Scn *scn;       // section iterator
    size_t shstrndx;    // Section header string table index
    GElf_Shdr shdr;     // section header
    char *section_name;

    if (!file || !func_name || name_size == 0) {
        ERROR("%s: Invalid parameters", __func__);
        snprintf(func_name, name_size, "Unknown");
        return;
    }

    if (_init_elf(file, &file_desc, &elf_obj, &elf_k)) {
        snprintf(func_name, name_size, "Unknown");
        return;
    }

    if (elf_getshdrstrndx(elf_obj, &shstrndx) != 0) {
        ERROR("%s: elf_getshdrstrndx() failed: %s", __func__, elf_errmsg(-1));
        snprintf(func_name, name_size, "Unknown");
        elf_end(elf_obj);
        close(file_desc);
        return;
    }

    /* find .symtab section */
    scn = NULL;
    if (_get_shdr_by_type(elf_obj, &scn, &shdr, SHT_SYMTAB)) {
        snprintf(func_name, name_size, "Unknown");
        elf_end(elf_obj);
        close(file_desc);
        return;
    }

    /* Read data from .symtab */
    data = NULL;
    if (!(data = elf_getdata(scn, data))) {
        ERROR("%s: elf_getdata() failed: %s", __func__, elf_errmsg(-1));
        snprintf(func_name, name_size, "Unknown");
        elf_end(elf_obj);
        close(file_desc);
        return;
    }

    /*
     * Search for closest function symbol
     */
    size_t symbol_count = shdr.sh_size / shdr.sh_entsize;
    uint64_t closest_addr = 0;
    GElf_Sym sym;
    for (size_t i = 0; i < symbol_count; i++)
    {
        if (gelf_getsym(data, (int)i, &sym) == NULL) {
            ERROR("%s: gelf_getsym() failed: %s", __func__, elf_errmsg(-1));
            continue;
        }

        if (GELF_ST_TYPE(sym.st_info) != STT_FUNC)
            continue;

        if (sym.st_value <= address && sym.st_value >= closest_addr)
            closest_addr = sym.st_value;
    }
     
    /* Closest function found. Update func_name */
    snprintf(func_name, name_size, "%s:%#x",
        elf_strptr(elf_obj, shdr.sh_link, sym.st_name),
        (unsigned int) closest_addr);

    elf_end(elf_obj); 
    close(file_desc);
    return;
}