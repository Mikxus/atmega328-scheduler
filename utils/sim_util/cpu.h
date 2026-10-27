#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>

#include "debug.h"

#include "sim_avr.h"

/**
 * @brief Read simulated CPU register  
 * @note  reg num 0 - 31
 *        where: 
 *          x register:  
 *          r26: x low pointer
 *          r28: x high pointer
 * 
 *          y register:
 *          r28: y low pointer
 *          r29: y high pointer
 *     
 *          z register: 
 *          r30: z low pointer
 *          r31: z high pointer 
 * @param  *avr: 
 * @param  reg_num: 
 * @retval 
 */
uint8_t read_register(avr_t *avr, uint8_t reg_num);

#endif // _CPU_H_