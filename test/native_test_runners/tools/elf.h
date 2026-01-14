#ifndef _ELF_H_
#define _ELF_H_

#include <libelf.h>
#include <gelf.h>
#include <fcntl.h>

#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include "debug.h"
#include "sim_elf.h"

void elf_resolve_flash_address_to_function(const char *file,
    uint32_t address,
    char *func_name,
    size_t name_size);

#endif /* _ELF_H_ */