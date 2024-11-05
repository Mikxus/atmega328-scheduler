/*
 *
 */
#ifndef TEST_H
#define TEST_H

#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"

/**
 * @brief Initializes simavr for the given elf file  
 * @note   
 * @param  *elf_name: 
 * @retval Returns a pointer to the avr struct 
 */
avr_t *init_avr(const char *elf_name);

/*
 *  
 */
int avr_run(avr_t *avr); 

#endif