/**
 * @file ws2812b.h
 * @brief Simple ws2812b bitbang driver 
 * 
 */
#ifndef _WS2812B_H_
#define _WS2812B_H_

#include <inttypes.h>

/**
 * @brief ws2812_rgb data
 */
typedef struct wsb2812_rgb { 
	union 
	{
		uint8_t data[3]; // [green, red, blue]
		struct {
			uint8_t green;
			uint8_t red;
			uint8_t blue;
		};
	};
} rgb_t;


/**
 * @brief Send rgb_t buffer to leds.
 * 		Takes roughly 30 us per pixel.
 * @note Notes:
 * 			- Blocks interrupts until all of the buffer is written.
 * 			- Doesn't block for end latch which is 50us or 300us on
 * 			  newer chipsets. Calling code must ensure that this function isn't
 * 			  called again for the same strip under the latch time.
 * 			- Assumes cpu frequency of 16 MHz.
 * 
 * @param  leds:	Pointer to first element of rgb_t array
 * @param  size: 	Length of rgb_t array
 * @param  *portx: 	Pointer to physical port to be used
 * @param  pin: 	port pin [0, 7]
 * @retval None
 */
void ws2812b_write(
	const rgb_t * 		leds,
	const uint16_t 		size,
	volatile uint8_t *  portx,
	const uint8_t 		pin
);


#endif  // _WS2812B_H_