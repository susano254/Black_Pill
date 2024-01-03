#include <Flash.h>

void FLASH_SET_LATENCY(uint32_t latency){
	FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;

	if(latency == FLASH_ACR_LATENCY_1WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
	else if(latency == FLASH_ACR_LATENCY_2WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	else if(latency == FLASH_ACR_LATENCY_3WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
	else if(latency == FLASH_ACR_LATENCY_4WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
	else
		FLASH->ACR |= FLASH_ACR_LATENCY_0WS;
}

void Flash::set_latency(uint32_t latency) {
	FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;

	if(latency == FLASH_ACR_LATENCY_1WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
	else if(latency == FLASH_ACR_LATENCY_2WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	else if(latency == FLASH_ACR_LATENCY_3WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
	else if(latency == FLASH_ACR_LATENCY_4WS)
		FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
	else
		FLASH->ACR |= FLASH_ACR_LATENCY_0WS;
}

void Flash::unlock() {
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
}
void Flash::lock() {
	FLASH->CR |= FLASH_CR_LOCK;
}

void Flash::write_data(uint32_t address, uint32_t * data, uint16_t size) {
	while(FLASH->SR & FLASH_SR_BSY);
	set_programming_bit();
	uint32_t *pointer = (uint32_t*) address;
	for(int i = 0; i < size; i++){
		pointer[i] = data[i];
	}
}

void Flash::read_data(uint32_t address, uint32_t * buffer, uint16_t size) {
	while(FLASH->SR & FLASH_SR_BSY);
	uint32_t *data = (uint32_t*) address;
	for(int i = 0; i < size; i++){
		buffer[i] = data[i];
	}
}

void Flash::sector_erase(uint8_t sector) {
	while(FLASH->SR & FLASH_SR_BSY);

	FLASH->CR |= FLASH_CR_SER;

	if(sector > LAST_SECTOR_NUM) return;

	FLASH->CR &= ~FLASH_CR_SNB;
	FLASH->CR |= sector << FLASH_CR_SNB_Pos; 

	FLASH->CR |= FLASH_CR_STRT;

	while(FLASH->SR & FLASH_SR_BSY);
}

void Flash::mass_erase() {
	while(FLASH->SR & FLASH_SR_BSY);

	FLASH->CR |= FLASH_CR_MER;

	FLASH->CR |= FLASH_CR_STRT;

	while(FLASH->SR & FLASH_SR_BSY);
}

void Flash::set_parallelism(uint32_t parallelism) {
	FLASH->CR &= ~(FLASH_CR_PSIZE);

	switch (parallelism) {
		case FLASH_CR_PSIZE_x64:
			FLASH->CR |= FLASH_CR_PSIZE_x64;
			break;
		case FLASH_CR_PSIZE_x32:
			FLASH->CR |= FLASH_CR_PSIZE_x32;
			break;
		case FLASH_CR_PSIZE_x16:
			FLASH->CR |= FLASH_CR_PSIZE_x16;
			break;
		default:
			FLASH->CR |= FLASH_CR_PSIZE_x8;
			break;
	}
}

inline void Flash::set_programming_bit() {
	FLASH->CR |= FLASH_CR_PG;
}

void Flash::enable_data_cache() {
	FLASH->ACR |= FLASH_ACR_DCEN;
}
void Flash::disable_data_cache() {
	FLASH->ACR &= ~FLASH_ACR_DCEN;
}

void Flash::enable_instruction_cache() {
	FLASH->ACR |= FLASH_ACR_ICEN;
}
void Flash::disable_instruction_cache() {
	FLASH->ACR &= ~FLASH_ACR_ICEN;
}

void Flash::enable_prefetch() {
	FLASH->ACR |= FLASH_ACR_PRFTEN;
}
void Flash::disable_prefetch() {
	FLASH->ACR &= ~FLASH_ACR_PRFTEN;
}

