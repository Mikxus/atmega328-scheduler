/*
 *
 */
#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> 
#include <string.h> 

#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"



int validate_args(const int argc, char *argv[]);

/**
 * @brief Initializes simavr for the given elf file  
 * @note   
 * @param  *elf_name: 
 * @param  *mcu:
 * @param  frequency:
 * @retval Returns a pointer to the initialized avr struct
 */
avr_t *init_avr(const char *elf_name, const char *mcu, const int frequency);


#ifdef __cplusplus
}
#endif

#endif