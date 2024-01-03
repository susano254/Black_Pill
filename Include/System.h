#ifndef 	CORE_H
#define 	CORE_H

#include <stm32f401xc.h>
#include <core_cm4.h>

struct NVIC_t {
	void enableInterrupt(uint8_t);
	void clearPending(uint8_t);
};

struct System_t {
	void Init();
	unsigned int get_CYCCNT_Cycles();
	void delay_us(uint32_t);
	uint32_t getMicros();
	float getDeltaT(uint32_t prev_micros);
	void enable_floating_point_unit();
	void enable_cyclic_counter();
	void disable_cyclic_counter();
	void reset_cyclic_counter();
	void enable_tracing_unit();
};

struct Systick_t {
	uint32_t reload;
	uint32_t millis;

	void Init(uint32_t reload = 80000-1);
	float getMicros();
	float getMillis();
	float getDeltaT(float last_millis);
	float getDeltaT();
};

namespace global {
	extern NVIC_t Nvic;
	extern Systick_t Systick;
	extern System_t System;
}

// #ifdef __cplusplus
// extern "C" {
// #endif                                      
// 	void NVIC_EnableInterrupt(uint8_t);
// 	void NVIC_ClearPending(uint8_t);
// #ifdef __cplusplus
// }
// #endif                                      

#endif