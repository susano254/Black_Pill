#include <RCC.h>
#include <FLASH.h>
#include <System.h>
#include <USART.h>

namespace global {
	NVIC_t Nvic;
	Systick_t Systick;
	System_t System;
}

using namespace global;

void NVIC_t::clearPending(uint8_t interruptNumber){
	uint8_t reg = interruptNumber/32;
	uint8_t bitNumber = interruptNumber%32;

	NVIC->ICPR[reg] |= (1 << bitNumber);
}

void NVIC_t::enableInterrupt(uint8_t interruptNumber){
	uint8_t reg = interruptNumber/32;
	uint8_t bitNumber = interruptNumber%32;

	NVIC->ISER[reg] |= 1 <<  bitNumber;
}


void Systick_t::Init(uint32_t reload){
	this->reload = reload;
	//init systick
	SysTick->LOAD = reload;
	SysTick->VAL = reload;
	
	
	SysTick->CTRL |= 7 << 0;
}

float Systick_t::getDeltaT(float prev_millis){
	float dt;
	dt = (millis - prev_millis) * 0.001f + (reload - SysTick->VAL)*1.0f/80000000.0f;

	return dt;
}

float Systick_t::getMillis(){
	return Systick.millis;
}

extern "C" {
	void SysTick_Handler(){
		Systick.millis++;
	}
}

void System_t::Init() {
	// enable fpu
	enable_floating_point_unit();

	RCC_Init_Max_Clock();

	return;
}

void System_t::enable_floating_point_unit(){
	SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
}

void System_t::enable_tracing_unit(){
    CoreDebug->DEMCR  |= CoreDebug_DEMCR_TRCENA_Msk;
}

void System_t::reset_cyclic_counter(){
    DWT->CYCCNT  = 0; // reset the counter
	disable_cyclic_counter();
}

void System_t::enable_cyclic_counter(){
	enable_tracing_unit();
    DWT->CTRL |= 1 ; // enable the counter
}

void System_t::disable_cyclic_counter(){
    DWT->CTRL |= 0 ; // disable the counter    
}
uint32_t System_t::getMicros() {
	return get_CYCCNT_Cycles()/80;
}

float System_t::getDeltaT(uint32_t prev_micros) {
	float dt;
	uint32_t cur_micros = getMicros();
	if(cur_micros > prev_micros)
		dt = (cur_micros - prev_micros)*0.000001f;
	else if (cur_micros < prev_micros)
		dt = (cur_micros + (uint32_t)((0xffffffff/80) - prev_micros))*0.000001f;
	return dt;
}
void System_t::delay_us(uint32_t us) {
	uint32_t start_us = getMicros();
	while(getMicros()-start_us < us);
}

unsigned int System_t::get_CYCCNT_Cycles(){
    return DWT->CYCCNT;
}