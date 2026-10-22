#include <kernel/drivers/ws2812b.h>
#include <kernel/atomic.h>

void ws2812b_write(
	const rgb_t * 		leds,
	const uint16_t 		size,
	volatile uint8_t *  portx,
	const uint8_t 		pin
) {
	static_assert(
		F_CPU == 16000000UL,
		"ws2812b_write assumes cpu frequency of 16 Mhz"
	);

	if (size == 0)
		return;

	ATOMIC_BLOCK() {
		/* 
		 * Pre calculate high and low port values.
		 * So we can simplify port update from: read modify write
		 * to just write, since at 16 MHz we only have budget of ~6 cycles
		 * for the T0H state.
		 */
		uint16_t total_bytes	= size * 3;
		uint8_t low_state 		= *portx & ~(1U << pin);
		uint8_t high_state 		= *portx | (1U << pin);
		uint8_t current_byte;

		/*
		 * Time for sending 1 bit: 
		 *  	High state + low state = 1.25 us +- 600ns = 20 cycles
		 * 
		 * For 20 cycles there is no time to do loop logic and bit position 
		 * logic. We could just do the logic after sending bit. The spec
		 * allows up to 50 us pause before the leds latch on to their state. By 
		 * unrolling the loop we can "interleave" the logic without affecting
		 * timings.
		 */
		__asm__ volatile (
			// Preload first byte of rgb_t array
			"ld		%[byte], %a[ptr]+		\n\t" // [2]

			"loop_start%=:					\n\t" // start
			// #################################################################
			// Bit 7 
			// #################################################################
			"st		%a[port], %[high]		\n\t" // [2] set high 
			"lsl	%[byte]					\n\t" // [1]
			"brcs	send_7b_one%=			\n\t" // [False: 1, true: 2]
			// Bit 7 is zero
			// we've used ~ 125  ns with T0H state. 
			// we need to enter T0L state within 400 ns +-150
			"nop							\n\t" // [1] 
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"rjmp	end_7b%=				\n\t" // [2]
			// Bit 7 is one
			// Cycles used before 2 + 1 + 2 = 5 ~ 312.5ns
			// Cycles left T1L state = ~7.8
			"send_7b_one%=:					\n\t" // 3 cycles before
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// Both branches have used 12 cycles with 8 remaining 
			// before bit 6
			"end_7b%=:						\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[high]		\n\t" // [2] set high
			"lsl	%[byte]					\n\t" // [1]

			// #################################################################
			// Bit 6 
			// #################################################################
			"brcs	send_6b_one%=			\n\t" // [False: 1, true: 2]
			// #################################################################
			// Bit 6 is zero
			// cycl used 2. T0H end 5. To merge 10
			// #################################################################
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"rjmp	end_6b%=				\n\t" // [2] -- 12 Cycles merge
			// #################################################################
			// Bit 6 is one
			// cycl used 3, T1H end 9, To merge 9
			// #################################################################
			"send_6b_one%=:					\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// #################################################################
			// Bit 6 merge
			// 8 cycles remaining before bit 5
			// #################################################################
			"end_6b%=:						\n\t" // merge
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[high]		\n\t" // [2] set high
			"lsl	%[byte]					\n\t" // [1]

			// #################################################################
			// Bit 5 
			// #################################################################
			"brcs	send_5b_one%=			\n\t" // [False: 1, true: 2]
			// #################################################################
			// Bit 5 is zero
			// cycl used 2. T0H end 5. To merge 10
			// #################################################################
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"rjmp	end_5b%=				\n\t" // [2] -- 12 Cycles merge
			// #################################################################
			// Bit 5 is one
			// cycl used 3, T1H end 9, To merge 9
			// #################################################################
			"send_5b_one%=:					\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// #################################################################
			// Bit 5 merge
			// 8 cycles remaining before bit 4
			// #################################################################
			"end_5b%=:						\n\t" // merge
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[high]		\n\t" // [2] set high
			"lsl	%[byte]					\n\t" // [1]

			// #################################################################
			// Bit 4 
			// #################################################################
			"brcs	send_4b_one%=			\n\t" // [False: 1, true: 2]
			// #################################################################
			// Bit 4 is zero
			// cycl used 2. T0H end 5. To merge 10
			// #################################################################
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"rjmp	end_4b%=				\n\t" // [2] -- 12 Cycles merge
			// #################################################################
			// Bit 4 is one
			// cycl used 3, T1H end 9, To merge 9
			// #################################################################
			"send_4b_one%=:					\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// #################################################################
			// Bit 4 merge
			// 8 cycles remaining before bit 3
			// #################################################################
			"end_4b%=:						\n\t" // merge
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[high]		\n\t" // [2] set high
			"lsl	%[byte]					\n\t" // [1]

			// #################################################################
			// Bit 3 
			// #################################################################
			"brcs	send_3b_one%=			\n\t" // [False: 1, true: 2]
			// #################################################################
			// Bit 3 is zero
			// cycl used 2. T0H end 5. To merge 10
			// #################################################################
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"rjmp	end_3b%=				\n\t" // [2] -- 12 Cycles merge
			// #################################################################
			// Bit 3 is one
			// cycl used 3, T1H end 9, To merge 9
			// #################################################################
			"send_3b_one%=:					\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// #################################################################
			// Bit 3 merge
			// 8 cycles remaining before bit 2
			// #################################################################
			"end_3b%=:						\n\t" // merge
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[high]		\n\t" // [2] set high
			"lsl	%[byte]					\n\t" // [1]

			// #################################################################
			// Bit 2 
			// #################################################################
			"brcs	send_2b_one%=			\n\t" // [False: 1, true: 2]
			// #################################################################
			// Bit 2 is zero
			// cycl used 2. T0H end 5. To merge 10
			// #################################################################
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"rjmp	end_2b%=				\n\t" // [2] -- 12 Cycles merge
			// #################################################################
			// Bit 2 is one
			// cycl used 3, T1H end 9, To merge 9
			// #################################################################
			"send_2b_one%=:					\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// #################################################################
			// Bit 2 merge
			// 8 cycles remaining before bit 1
			// #################################################################
			"end_2b%=:						\n\t" // merge
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[high]		\n\t" // [2] set high
			"lsl	%[byte]					\n\t" // [1]

			// #################################################################
			// Bit 1 
			// #################################################################
			"brcs	send_1b_one%=			\n\t" // [False: 1, true: 2]
			// #################################################################
			// Bit 1 is zero
			// cycl used 2. T0H end 5. To merge 10
			// #################################################################
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"rjmp	end_1b%=				\n\t" // [2] -- 12 Cycles merge
			// #################################################################
			// Bit 1 is one
			// cycl used 3, T1H end 9, To merge 9
			// #################################################################
			"send_1b_one%=:					\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// #################################################################
			// Bit 1 merge
			// 8 cycles remaining before bit 0
			// #################################################################
			"end_1b%=:						\n\t" // merge
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[high]		\n\t" // [2] set high
			"lsl	%[byte]					\n\t" // [1]

			// #################################################################
			// Bit 0 
			// We'll use the time between waiting states to do loop logic
			// #################################################################
			"brcs	send_0b_one%=			\n\t" // [False: 1, true: 2]
			// #################################################################
			// Bit 0 is zero
			// cycl used 2. T0H end 5. To merge 10
			// #################################################################
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"st		%a[port], %[low]		\n\t" // [2] T0H end  7 cycles ~ 437ns
			"nop							\n\t" // [1]
			// Prepare by subtracting byte_cnt since bit 0 merge
			// doesn't have enough cycles
			"sbiw	%[byte_cnt], 1			\n\t" // [2]
			"rjmp	end_0b%=				\n\t" // [2] -- 12 Cycles merge
			// #################################################################
			// Bit 0 is one
			// cycl used 3, T1H end 9, To merge 9
			// #################################################################
			"send_0b_one%=:					\n\t"
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			"nop							\n\t" // [1]
			// Prepare by subtracting byte_cnt since bit 0 merge
			// doesn't have enough cycles
			"sbiw	%[byte_cnt], 1			\n\t" // [2] 
			"st		%a[port], %[low]		\n\t" // [2] T1H end 12 cycles ~ 750 ns
			// #################################################################
			// Bit 0 merge
			// 8 - 2 (loop_start 2 cycles) = 6 cycles remaining before loop_start
			// Do loop iteration
			// #################################################################
			"end_0b%=:						\n\t" // merge
			"breq	exit_loop%=				\n\t" // [1 - 2] if zero 2 cycles
			"nop							\n\t" // [1]
			"ld		%[byte], %a[ptr]+ 		\n\t" // [2] load next byte
			"rjmp 	loop_start%=			\n\t" // [2] goto start loop
			"exit_loop%=:					\n\t" // exit loop
			:	[byte] 		"=&r"	(current_byte),
				[byte_cnt] 	"+w"	(total_bytes),
				[ptr] 		"+e"	(leds)
			:	[port]		"e"		(portx),
				[high]		"r"		(high_state),
				[low]		"r"		(low_state)
			: "memory"
		);
	}
	return;
}